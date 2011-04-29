/*********************************************************\
File:           ComponentCollidable.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/29/2011 1:01:30 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentCollidable.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "TaskManager.h"
#include "Renderer.h"

/*
CComponentCollidable
3
eComponentMessageTransform
eComponentMessageBoundingVolumeType
eComponentMessageCollision
2
BoundingVolume m_Volume
VolumeType m_nVolumeType
*/

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    CComponentCollidable* CComponentCollidable::m_pInstance = NULL;

    const eComponentMessageType CComponentCollidable::MessagesReceived[] =
    {
        eComponentMessageTransform,
        eComponentMessageBoundingVolumeType,
        eComponentMessageCollision,
    };
    const uint CComponentCollidable::NumMessagesReceived = (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType );

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentCollidable::CComponentCollidable()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;
        Memset( m_pTerrainTriangles, 0, sizeof(m_pTerrainTriangles) );

        // Make sure we don't have any leaves
        m_pParentNodes      = NULL;
        m_pTerrainLeaves    = NULL;
        m_pTerrainGraph     = NULL;

        // Create the head of the object graph
        m_pObjectGraph  = new TObjectParentNode;
        m_pObjectGraph->max = RVector3(  64.0f,  64.0f,  64.0f );
        m_pObjectGraph->min = RVector3( -64.0f, -64.0f, -64.0f );
    }

    CComponentCollidable::~CComponentCollidable() 
    {
        SAFE_DELETE( m_pObjectGraph );
        SAFE_DELETE_ARRAY( m_pParentNodes );
        SAFE_DELETE_ARRAY( m_pTerrainLeaves );
    } 

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponentCollidable::Attach( uint nObject )
    {
        PreAttach( nObject );
        /********************************/

        // Now initialize this component
        Memset( &m_Volume[m_nIndex], 0, sizeof(RSphere) );

        // Add it to the node
        m_pObjectGraph->AddObjectLeaf( &m_ObjectSceneNodes[m_nIndex] );

        /********************************/
        PostAttach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentCollidable::Reattach( uint nObject )
    {
        PreReattach( nObject );
        /********************************/

        // Perform any custom reattchment

        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Volume );
        COMPONENT_USE_PREV_DATA( m_ObjectSceneNodes );

        /********************************/
        PostReattach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentCollidable::Detach( uint nObject )
    {
        PreDetach( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Volume );
        COMPONENT_REORDER_DATA( m_ObjectSceneNodes );

        /********************************/
        PostDetach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentCollidable::DetachAndSave( uint nObject )
    {
        PreDetachAndSave( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Volume );
        COMPONENT_REORDER_SAVE_DATA( m_ObjectSceneNodes );

        /********************************/
        PostDetachAndSave( nObject );
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentCollidable::RemoveInactive( uint nObject )
    {
        PreRemoveInactive( nObject );
        /********************************/

        // Perform any custom removal stuff

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Volume );
        COMPONENT_REMOVE_PREV_DATA( m_ObjectSceneNodes );

        /********************************/
        PostRemoveInactive( nObject );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CComponentCollidable::ProcessComponent( void )
    {
        CTaskManager*   pTaskManager = CTaskManager::GetInstance();

        //////////////////////////////////////////
        // Build the scene graph

        // Draw the graphs
        if( gs_bShowBoundingVolumes )
        {
            DrawNodes( m_pTerrainGraph, 4 );
            m_pObjectGraph->DrawNode( Engine::GetRenderer(), RVector3( 1.0f, 1.0f, 1.0f ) );
        }

        // First update the object graph
        m_pObjectGraph->RecalculateBounds();

#if PARALLEL_UPDATE
        task_handle_t   nHandle = pTaskManager->PushTask( ProcessBatch, this, m_nNumActiveComponents, 4 );
        pTaskManager->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumActiveComponents );
#endif
    }

    void CComponentCollidable::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CRenderer*              pRenderer = Engine::GetRenderer();
        CComponentCollidable*   pComponent = (CComponentCollidable*)pData;
        CObjectManager*         pManager = Engine::GetObjectManager();

        uint nEnd = nStart + nCount;

        for( uint i = nStart; i < nEnd; ++i )
        {
            if( gs_bShowBoundingVolumes )
            {
                pRenderer->DrawDebugSphere( pComponent->m_Volume[i] );

                //pComponent->m_ObjectSceneNodes[i].DrawNode( pRenderer, RVector3( 0.5f, 1.0f, 1.0f ) );
            }

            // Check against the ground first
            if( pComponent->SphereTerrainCollision( pComponent->m_pTerrainGraph, pComponent->m_Volume[i] ) )
            {
                int x = 0;
                pManager->PostMessage( eComponentMessageCollision, pComponent->m_pObjectIndices[ i ], x, pComponent->ComponentType );
            }

            // Then against all other objects
            for( uint j = 0; j < pComponent->m_nNumActiveComponents; ++j )
            {
                if( i == j ) continue;

                if( SphereSphereCollision( pComponent->m_Volume[i], pComponent->m_Volume[j] ) )
                {
                    pManager->PostMessage( eComponentMessageCollision, pComponent->m_pObjectIndices[ i ], j, pComponent->ComponentType );
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  CalculateBoundingSphere
    //  Calculates a bounding sphere to surround the input vertices
    //-----------------------------------------------------------------------------
    void CComponentCollidable::CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, uint nIndex )
    {
        ASSERT( m_pInstance );

        float3 fExtents = {  0 };

        for( uint i = 0; i < nNumVerts; ++i )
        {
            for( uint j = 0; j < 3; ++j )
            {
                if( Abs(pVerts[i].Pos[j]) > fExtents[j] )
                {
                    fExtents[j] = Abs(pVerts[i].Pos[j]);
                }
            }
        }

        m_pInstance->m_Volume[nIndex].radius = Magnitude( RVector3(fExtents) );
    }

    //-----------------------------------------------------------------------------
    //  SetTerrainData
    //  Sets the terrain data so objects can collide with it
    //-----------------------------------------------------------------------------
    void CComponentCollidable::SetTerrainData( const VPosNormalTex* pTerrainVerts, uint nNumVerts, const uint16* pIndices, uint nNumIndices )
    {
        // Calculate the tree's depth
        uint nTotalLeaves = (nNumIndices/3) >> 1;
        uint nTotalParents = 0;
        uint nLevel = 0;
        uint nThisLevel = nTotalLeaves;

        while( nThisLevel != 1 )
        {
            nThisLevel = nThisLevel/4;
            ++nLevel;
            nTotalParents += nThisLevel;
        }


        SAFE_DELETE_ARRAY( m_pInstance->m_pParentNodes );
        SAFE_DELETE_ARRAY( m_pInstance->m_pTerrainLeaves );
        m_pInstance->m_pParentNodes      = new TTerrainParentNode[nTotalParents];
        m_pInstance->m_pTerrainLeaves    = new TTerrainLeafNode[nTotalLeaves];
        m_pInstance->m_nNumParentNodes   = 0;
        m_pInstance->m_nNumTerrainLeaves = 0;

        // Make the tree
        m_pInstance->m_pTerrainGraph = m_pInstance->m_pParentNodes + m_pInstance->m_nNumParentNodes++;
        m_pInstance->m_pTerrainGraph->min = RVector3( -(CTerrain::TERRAIN_WIDTH >> 1), -30000.0f, -(CTerrain::TERRAIN_HEIGHT >> 1) );
        m_pInstance->m_pTerrainGraph->max = RVector3( (CTerrain::TERRAIN_WIDTH >> 1), 30000.0f, (CTerrain::TERRAIN_HEIGHT >> 1) );
        m_pInstance->BuildParentNodes( m_pInstance->m_pTerrainGraph, 0 );

        for( uint i = 0; i < nNumIndices/3; ++i )
        {
            m_pInstance->m_pTerrainTriangles[i].vVerts[0] = pTerrainVerts[ pIndices[ (i*3) + 0 ] ].Pos;
            m_pInstance->m_pTerrainTriangles[i].vVerts[1] = pTerrainVerts[ pIndices[ (i*3) + 1 ] ].Pos;
            m_pInstance->m_pTerrainTriangles[i].vVerts[2] = pTerrainVerts[ pIndices[ (i*3) + 2 ] ].Pos;
            
            RVector3 vSide1 = m_pInstance->m_pTerrainTriangles[i].vVerts[0] - m_pInstance->m_pTerrainTriangles[i].vVerts[1];
            RVector3 vSide2 = m_pInstance->m_pTerrainTriangles[i].vVerts[1] - m_pInstance->m_pTerrainTriangles[i].vVerts[2];

            m_pInstance->m_pTerrainTriangles[i].vNormal = Normalize( CrossProduct( vSide1, vSide2 ) );
        }

        BuildSceneGraph();
    }

    //-----------------------------------------------------------------------------
    //  BuildParentNodes
    //  Constructs the top of the tree
    //-----------------------------------------------------------------------------
    void CComponentCollidable::BuildParentNodes( TTerrainParentNode* pNode, TTerrainParentNode* pParent )
    {
        pNode->m_pParent = pParent;
        float fNewX = (pNode->max.x + pNode->min.x) / 2.0f;
        float fNewZ = (pNode->max.z + pNode->min.z) / 2.0f;

        if( Abs(pNode->max.x - pNode->min.x) > 2.0f )
        {
            TTerrainParentNode* pNewNode = NULL;

            pNewNode = &m_pParentNodes[m_nNumParentNodes++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->max.x = fNewX;
            pNewNode->max.z = fNewZ;
            pNode->m_pChildren[0] = pNewNode;

            BuildParentNodes( pNewNode, pNode );
            
            pNewNode = &m_pParentNodes[m_nNumParentNodes++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->min.z = fNewZ;
            pNewNode->max.x = fNewX;
            pNode->m_pChildren[1] = pNewNode;

            BuildParentNodes( pNewNode, pNode );
            
            pNewNode = &m_pParentNodes[m_nNumParentNodes++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->min.x = fNewX;
            pNewNode->min.z = fNewZ;
            pNode->m_pChildren[2] = pNewNode;

            BuildParentNodes( pNewNode, pNode );

            pNewNode = &m_pParentNodes[m_nNumParentNodes++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->min.x = fNewX;
            pNewNode->max.z = fNewZ;
            pNode->m_pChildren[3] = pNewNode;

            BuildParentNodes( pNewNode, pNode );
        }
        else
        {
            TTerrainLeafNode* pLeafNode = NULL;
            pNode->m_nLowestParent = 1;
            
            pLeafNode = &m_pTerrainLeaves[m_nNumTerrainLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->max.x = fNewX;
            pLeafNode->max.z = fNewZ;
            pNode->m_pChildren[0] = pLeafNode;
            
            pLeafNode = &m_pTerrainLeaves[m_nNumTerrainLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->min.z = fNewZ;
            pLeafNode->max.x = fNewX;
            pNode->m_pChildren[1] = pLeafNode;
            
            pLeafNode = &m_pTerrainLeaves[m_nNumTerrainLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->min.x = fNewX;
            pLeafNode->min.z = fNewZ;
            pNode->m_pChildren[2] = pLeafNode;

            pLeafNode = &m_pTerrainLeaves[m_nNumTerrainLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->min.x = fNewX;
            pLeafNode->max.z = fNewZ;
            pNode->m_pChildren[3] = pLeafNode;
        }
    }

    //-----------------------------------------------------------------------------
    //  RecomputeSceneGraphBounds
    //  Recomputes the bounds of the top of the tree
    //-----------------------------------------------------------------------------
    void CComponentCollidable::RecomputeSceneGraphBounds( CComponentCollidable::TSceneNode* pNode )
    {
        CComponentCollidable::TTerrainParentNode* pParentNode = (CComponentCollidable::TTerrainParentNode*)pNode;

        pParentNode->max = RVector3( -10000.0f, -10000.0f, -10000.0f );
        pParentNode->min = RVector3( 10000.0f, 10000.0f, 10000.0f );
        for( uint i = 0; i < 4; ++i )
        {
            if( !pParentNode->m_nLowestParent )
            {
                // We're a grandparent, calculate the children
                RecomputeSceneGraphBounds( pParentNode->m_pChildren[i] );
            }

            pNode->max.x = Max( pParentNode->m_pChildren[i]->max.x, pNode->max.x );
            pNode->max.y = Max( pParentNode->m_pChildren[i]->max.y, pNode->max.y );
            pNode->max.z = Max( pParentNode->m_pChildren[i]->max.z, pNode->max.z );

            pNode->min.x = Min( pParentNode->m_pChildren[i]->min.x, pNode->min.x );
            pNode->min.y = Min( pParentNode->m_pChildren[i]->min.y, pNode->min.y );
            pNode->min.z = Min( pParentNode->m_pChildren[i]->min.z, pNode->min.z );
        }
    }

    //-----------------------------------------------------------------------------
    //  BuildSceneGraph
    //  Builds the scene graph
    //-----------------------------------------------------------------------------
    void CComponentCollidable::BuildSceneGraph( void )
    {
        // Remove the triangles from the leaves
        for( uint i = 0; i < m_pInstance->m_nNumTerrainLeaves; ++i )
        {
            m_pInstance->m_pTerrainLeaves[i].m_nNumTri = 0;
        }

        // Fill with triangles
        for( uint i = 0; i < nNumTriangles; ++i )
        {
            m_pInstance->AddTriangleToGraph( m_pInstance->m_pTerrainGraph, &m_pInstance->m_pTerrainTriangles[i] );
        }

        // Rebalance the top of the tree
        m_pInstance->RecomputeSceneGraphBounds( m_pInstance->m_pTerrainGraph );
    }

    //-----------------------------------------------------------------------------
    //  DrawNodes
    //  Draws all nodes down to a specific depth
    //-----------------------------------------------------------------------------
    void CComponentCollidable::DrawNodes( TTerrainParentNode* pNode, uint nDepth )
    {
        CRenderer*  pRenderer = Engine::GetRenderer();
        static const RVector3    vColors[] =
        {
            RVector3( 1.0f, 1.0f, 1.0f ),
            RVector3( 0.0f, 1.0f, 1.0f ),
            RVector3( 1.0f, 0.0f, 1.0f ),
            RVector3( 1.0f, 1.0f, 0.0f ),
            RVector3( 0.0f, 1.0f, 0.0f ),
            RVector3( 0.0f, 0.0f, 1.0f ),
            RVector3( 1.0f, 0.0f, 0.0f ),
            RVector3( 0.0f, 0.0f, 0.0f ),
            RVector3( 0.5f, 0.5f, 0.5f ),
            RVector3( 1.0f, 0.5f, 0.0f ),
            RVector3( 0.0f, 1.0f, 0.5f ),
            RVector3( 0.5f, 0.0f, 1.0f ),
        };

        if( nDepth )
        {
            ((TSceneNode*)pNode)->DrawNode( pRenderer, vColors[nDepth] );

            if( !pNode->m_nLowestParent )
            {
                for( uint i = 0; i < 4; ++i )
                {
                    DrawNodes( (TTerrainParentNode*)pNode->m_pChildren[i], nDepth-1 );
                }
            }
            else
            {
                for( uint i = 0; i < 4; ++i )
                {
                    pNode->m_pChildren[i]->DrawNode( pRenderer, vColors[nDepth-1] );
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  AddTriangleToGraph
    //  Adds a triangle to the graph
    //-----------------------------------------------------------------------------
    void CComponentCollidable::AddTriangleToGraph( TSceneNode* pNode, Triangle* pTriangle, bool bLeaf )
    {
        for( int i = 0; i < 3; ++i )
        {
            if(    pTriangle->vVerts[i].x > pNode->max.x
                || pTriangle->vVerts[i].y > pNode->max.y
                || pTriangle->vVerts[i].z > pNode->max.z
                || pTriangle->vVerts[i].x < pNode->min.x
                || pTriangle->vVerts[i].y < pNode->min.y
                || pTriangle->vVerts[i].z < pNode->min.z )
            {
                return;
            }
        }

        if( !bLeaf )
        {
            // There are still children below us, add it
            TTerrainParentNode* pParentNode = (TTerrainParentNode*)pNode;
            if( !pParentNode->m_nLowestParent )
            {
                for( int i = 0; i < 4; ++i )
                {
                    AddTriangleToGraph( pParentNode->m_pChildren[i], pTriangle );
                }
                return;
            }
            else
            {
                for( int i = 0; i < 4; ++i )
                {
                    AddTriangleToGraph( pParentNode->m_pChildren[i], pTriangle, true );
                }
                return;
            }
        }

        // This is a leaf, see which child the triangle goes in
        TTerrainLeafNode* pLeaf = (TTerrainLeafNode*)pNode;
        pLeaf->m_pTri[ pLeaf->m_nNumTri++ ] = pTriangle;

        if( pLeaf->m_nNumTri == 2 )
        {
            pLeaf->max = RVector3( -10000.0f, -10000.0f, -10000.0f );
            pLeaf->min = RVector3( 10000.0f, 10000.0f, 10000.0f );
            for( uint i = 0; i < 3; ++i )
            {
                pLeaf->max.x = Max( pLeaf->m_pTri[0]->vVerts[i].x, Max( pLeaf->m_pTri[1]->vVerts[i].x, pLeaf->max.x ) );
                pLeaf->max.y = Max( pLeaf->m_pTri[0]->vVerts[i].y, Max( pLeaf->m_pTri[1]->vVerts[i].y, pLeaf->max.y ) );
                pLeaf->max.z = Max( pLeaf->m_pTri[0]->vVerts[i].z, Max( pLeaf->m_pTri[1]->vVerts[i].z, pLeaf->max.z ) );
                pLeaf->min.x = Min( pLeaf->m_pTri[0]->vVerts[i].x, Min( pLeaf->m_pTri[1]->vVerts[i].x, pLeaf->min.x ) );
                pLeaf->min.y = Min( pLeaf->m_pTri[0]->vVerts[i].y, Min( pLeaf->m_pTri[1]->vVerts[i].y, pLeaf->min.y ) );
                pLeaf->min.z = Min( pLeaf->m_pTri[0]->vVerts[i].z, Min( pLeaf->m_pTri[1]->vVerts[i].z, pLeaf->min.z ) );
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponentCollidable::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                m_Volume[nSlot].position = transform.position;  
                float fRad = m_Volume[nSlot].radius;
                
                RVector3 vMax = RVector3(  fRad,  fRad,  fRad );
                RVector3 vMin = RVector3( -fRad, -fRad, -fRad );
                
                m_ObjectSceneNodes[nSlot].max = vMax + transform.position;
                m_ObjectSceneNodes[nSlot].min = vMin + transform.position;

                ((TObjectParentNode*)m_ObjectSceneNodes[nSlot].m_pParent)->m_nInvalid = 1;
            }
            break;
        case eComponentMessageBoundingVolumeType:
            {
            }
            break;
        case eComponentMessageCollision:
            {
            }
            break;
        default:
            {
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  IsPointInTriangle
    //  Determines if a point is inside a particular triangle
    //-----------------------------------------------------------------------------
    // Reference: http://www.peroxide.dk/papers/collision/collision.pdf
#define FloatBitwiseToInt(a) ((uint32&) a)
    bool CComponentCollidable::IsPointInTriangle( const RVector3& point, const Triangle& triangle )
    {
        RVector3 vSide1 = triangle.vVerts[1] - triangle.vVerts[0];
        RVector3 vSide2 = triangle.vVerts[2] - triangle.vVerts[0];

        //Plane

        float a = DotProduct( vSide1, vSide1 );
        float b = DotProduct( vSide1, vSide2 );
        float c = DotProduct( vSide2, vSide2 );

        float ac_bb = (a*c) - (b*b);

        RVector3 vp( point.x - triangle.vVerts[0].x, point.y - triangle.vVerts[0].y, point.z - triangle.vVerts[0].z );

        float d = DotProduct( vp, vSide1 );
        float e = DotProduct( vp, vSide2 );

        float x = (d*c) - (e*b);
        float y = (e*a) - (d*b);
        float z = x+y-ac_bb;


        return (( FloatBitwiseToInt(z) & ~(FloatBitwiseToInt(x)|FloatBitwiseToInt(y)) ) & 0x80000000);
    }

    //-----------------------------------------------------------------------------
    //  SphereTerrainCollision
    //  Determines if a tree hits any triangles within the node
    //-----------------------------------------------------------------------------
    bool CComponentCollidable::SphereTerrainCollision( TSceneNode* pNode, const RSphere& s )
    {
        TTerrainParentNode* pParentNode = (TTerrainParentNode*)pNode;
        if( SphereAABBCollision( *pNode, s ) )
        {
            if( !pParentNode->m_nLowestParent )
            {
                for( uint i = 0; i < 4; ++i )
                {
                    if( SphereTerrainCollision( pParentNode->m_pChildren[i], s ) )
                        return true;
                }
            }
            else
            {
                for( uint i = 0; i < 4; ++i )
                {
                    TTerrainLeafNode* pLeaf = (TTerrainLeafNode*)pParentNode->m_pChildren[i];
                    if( pLeaf->SphereTriangleCollision( s ) )
                        return true;
                }
            }
        }

        return false;
    }

    //-----------------------------------------------------------------------------
    //  ObjectObjectCollision
    //  Performs object-object collision
    //-----------------------------------------------------------------------------
    bool CComponentCollidable::ObjectObjectCollision( TSceneNode* pGraph, TSceneNode* pNode )
    {
       
    }


} // namespace Riot
