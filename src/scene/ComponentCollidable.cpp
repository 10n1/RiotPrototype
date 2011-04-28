/*********************************************************\
File:           ComponentCollidable.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/28/2011 4:06:33 PM
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

    uint CComponentCollidable::nTemp = 0;

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentCollidable::CComponentCollidable()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;
        Memset( m_pTerrainTriangles, 0, sizeof(m_pTerrainTriangles) );

        m_pGraph = NULL;
    }

    CComponentCollidable::~CComponentCollidable() 
    {
        SAFE_DELETE( m_pGraph );
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
        Memset( &m_Volume[m_nIndex], 0, sizeof(BoundingVolume) );
        m_nVolumeType[m_nIndex]   = BoundingSphere;

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
        COMPONENT_USE_PREV_DATA( m_nVolumeType );

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
        COMPONENT_REORDER_DATA( m_nVolumeType );

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
        COMPONENT_REORDER_SAVE_DATA( m_nVolumeType );

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
        COMPONENT_REMOVE_PREV_DATA( m_nVolumeType );

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
        CRenderer*      pRenderer = Engine::GetRenderer();

        //////////////////////////////////////////
        // Build the scene graph
        if( m_pGraph )
        {
            nTemp = 0;
            m_pGraph->DrawNode( pRenderer, RVector3( 0.0f, 0.0f, 0.0f ), 4 );
        }

#if PARALLEL_UPDATE
        task_handle_t   nHandle = pTaskManager->PushTask( ProcessBatch, this, m_nNumActiveComponents, 4 );
        pTaskManager->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumActiveComponents );
#endif
    }

    // Reference: http://www.peroxide.dk/papers/collision/collision.pdf
    void CComponentCollidable::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CRenderer*              pRenderer = Engine::GetRenderer();
        CComponentCollidable*   pComponent = (CComponentCollidable*)pData;
        CObjectManager*         pManager = Engine::GetObjectManager();

        uint nEnd = nStart + nCount;

        for( uint i = nStart; i < nEnd; ++i )
        {

            RVector3    fPosition = RVector3(pComponent->m_Volume[i].sphere.position);
            float       fRadius   = pComponent->m_Volume[i].sphere.radius;

            if( gs_bShowBoundingVolumes )
            {
                RVector4    debugSphere( fPosition );
                debugSphere.w = sqrtf(fRadius);

                pRenderer->DrawDebugSphere( debugSphere );
            }

            if( DoesSphereHitTriangle( pComponent->m_pGraph, pComponent->m_Volume[i].sphere ) )
            {
                uint x = 0;
                pManager->PostMessage( eComponentMessageCollision, pComponent->m_pObjectIndices[ i ], x, pComponent->ComponentType );
            }

            // Then against all other objects
            for( uint j = 0; j < pComponent->m_nNumActiveComponents; ++j )
            {
                if( i == j ) continue;

                float fRadSq = fRadius + pComponent->m_Volume[j].sphere.radius;

                RVector3 pos1( fPosition );
                RVector3 pos2( pComponent->m_Volume[j].sphere.position );

                RVector3 diff = pos2-pos1;

                float fDistance = Abs( MagnitudeSq( diff ) );

                if( fDistance <= fRadSq )
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

        m_pInstance->m_Volume[nIndex].sphere.radius = MagnitudeSq( RVector3(fExtents) );
    }

    //-----------------------------------------------------------------------------
    //  SetTerrainData
    //  Sets the terrain data so objects can collide with it
    //-----------------------------------------------------------------------------
    void CComponentCollidable::SetTerrainData( const VPosNormalTex* pTerrainVerts, uint nNumVerts, const uint16* pIndices, uint nNumIndices )
    {
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

    void CComponentCollidable::BuildLeafNodes( SceneNode* pNode, Triangle* pTriangles )
    {
        if( abs(pNode->vMax.x-pNode->vMin.x) > 1.0f )
        {
            SceneNode* pNewNode = NULL;
            float fNewX = (pNode->vMax.x + pNode->vMin.x) / 2.0f;
            float fNewZ = (pNode->vMax.z + pNode->vMin.z) / 2.0f;

            pNode->pChildren = new SceneNode[4];

            pNewNode = &pNode->pChildren[0];
            pNewNode->vMin = pNode->vMin;
            pNewNode->vMax = pNode->vMax;
            pNewNode->vMax.x = fNewX;
            pNewNode->vMax.z = fNewZ;

            BuildLeafNodes( pNewNode, pTriangles );
            
            pNewNode = &pNode->pChildren[1];
            pNewNode->vMin = pNode->vMin;
            pNewNode->vMax = pNode->vMax;
            pNewNode->vMin.z = fNewZ;
            pNewNode->vMax.x = fNewX;

            BuildLeafNodes( pNewNode, pTriangles );
            
            pNewNode = &pNode->pChildren[2];
            pNewNode->vMin = pNode->vMin;
            pNewNode->vMax = pNode->vMax;
            pNewNode->vMin.x = fNewX;
            pNewNode->vMin.z = fNewZ;

            BuildLeafNodes( pNewNode, pTriangles );

            pNewNode = &pNode->pChildren[3];
            pNewNode->vMin = pNode->vMin;
            pNewNode->vMax = pNode->vMax;
            pNewNode->vMin.x = fNewX;
            pNewNode->vMax.z = fNewZ;

            BuildLeafNodes( pNewNode, pTriangles );
        }
        else
        {
            if( pNode->pChildren != NULL )
            {
                int x = 0;
            }
        }
    }

    void RecomputeSceneNode( CComponentCollidable::SceneNode* pNode )
    {
        if( pNode->pChildren )
        {
            if( reinterpret_cast<uint64>(pNode->pChildren) & 0xFFFF000000000000 )
                int x = 0;
            pNode->vMax = RVector3( -10000.0f, -10000.0f, -10000.0f );
            pNode->vMin = RVector3( 10000.0f, 10000.0f, 10000.0f );
            for( uint i = 0; i < 4; ++i )
            {
                RecomputeSceneNode( &pNode->pChildren[i] );

                pNode->vMax.x = Max( pNode->pChildren[i].vMax.x, pNode->vMax.x );
                pNode->vMax.y = Max( pNode->pChildren[i].vMax.y, pNode->vMax.y );
                pNode->vMax.z = Max( pNode->pChildren[i].vMax.z, pNode->vMax.z );

                pNode->vMin.x = Min( pNode->pChildren[i].vMin.x, pNode->vMin.x );
                pNode->vMin.y = Min( pNode->pChildren[i].vMin.y, pNode->vMin.y );
                pNode->vMin.z = Min( pNode->pChildren[i].vMin.z, pNode->vMin.z );
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  BuildSceneGraph
    //  Builds the scene graph
    //-----------------------------------------------------------------------------
    void CComponentCollidable::BuildSceneGraph( void )
    {
        SAFE_DELETE( m_pInstance->m_pGraph );

        m_pInstance->m_pGraph = new SceneNode;
        m_pInstance->m_pGraph->vMin = RVector3( -(CTerrain::TERRAIN_WIDTH >> 1), -30000.0f, -(CTerrain::TERRAIN_HEIGHT >> 1) );
        m_pInstance->m_pGraph->vMax = RVector3( (CTerrain::TERRAIN_WIDTH >> 1), 30000.0f, (CTerrain::TERRAIN_HEIGHT >> 1) );

        BuildLeafNodes( m_pInstance->m_pGraph, m_pInstance->m_pTerrainTriangles );
        
        // Fill with triangles
        for( uint i = 0; i < nNumTriangles; ++i )
        {
            m_pInstance->m_pGraph->AddTriangleToNode( &m_pInstance->m_pTerrainTriangles[i] );
        }

        RecomputeSceneNode( m_pInstance->m_pGraph );
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

                switch( m_nVolumeType[nSlot] )
                {
                case BoundingSphere:
                    {
                        m_Volume[nSlot].sphere.position[0] = transform.position[0];
                        m_Volume[nSlot].sphere.position[1] = transform.position[1];
                        m_Volume[nSlot].sphere.position[2] = transform.position[2];
                    }
                    break;
                case AABB:
                    {
                    }
                    break;
                default:
                    {
                    }
                };
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

#define in(a) ((uint32&) a)
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


        return (( in(z) & ~(in(x)|in(y)) ) & 0x80000000);
    }

    bool CComponentCollidable::DoesSphereHitTriangle( SceneNode* pNode, const BoundingVolume::_sphere& s )
    {
        if( pNode->DoesSphereHitBox( s ) )
        {
            if( pNode->pChildren )
            {
                if( DoesSphereHitTriangle( &pNode->pChildren[0], s ) )
                    return true;
                if( DoesSphereHitTriangle( &pNode->pChildren[1], s ) )
                    return true;
                if( DoesSphereHitTriangle( &pNode->pChildren[2], s ) )
                    return true;
                if( DoesSphereHitTriangle( &pNode->pChildren[3], s ) )
                    return true;
            }
            else
            {
                return pNode->DoesSphereHitTriangle( s );
            }
        }

        return false;
    }



} // namespace Riot
