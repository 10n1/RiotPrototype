/*********************************************************\
File:           ComponentCollidable.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/10/2011 3:07:28 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentCollidable.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "TaskManager.h"
#include "Renderer.h"
#include "Terrain.h"
#include <float.h>

/*
TODO:   Fix messaging threading issue
        Make sure objects are in every box they touch, not just their origin
        Eliminate the news and deletes in the nodes
*/

/*
CComponentCollidable
3
eComponentMessageTransform
eComponentMessageBoundingVolumeType
eComponentMessageObjectCollision
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
        eComponentMessageObjectCollision,
        eComponentMessageTerrainCollision,
    };
    const uint CComponentCollidable::NumMessagesReceived = (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType );

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentCollidable::CComponentCollidable()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;

        // Make sure we don't have any leaves
        m_pTerrain = NULL;

        // Create the head of the object graph
        m_pObjectGraph  = new TObjectParentNode;
        m_pObjectGraph->max = RVector3(  64.0f,  64.0f,  64.0f );
        m_pObjectGraph->min = RVector3( -64.0f, -64.0f, -64.0f );
        m_pObjectGraph->m_pParent = NULL;

        // Initialize the end points with min and max values
        m_EndPointsX[0].fValue = -FLT_MAX;
        m_EndPointsX[0].nIndex |= 0x7FFFFFFF;
        m_EndPointsX[1].fValue =  FLT_MAX;
        m_EndPointsX[1].nIndex |= 0xFFFFFFFF;

        m_EndPointsY[0].fValue = -FLT_MAX;
        m_EndPointsY[0].nIndex |= 0x7FFFFFFF;
        m_EndPointsY[1].fValue =  FLT_MAX;
        m_EndPointsY[1].nIndex |= 0xFFFFFFFF;

        m_EndPointsZ[0].fValue = -FLT_MAX;
        m_EndPointsZ[0].nIndex |= 0x7FFFFFFF;
        m_EndPointsZ[1].fValue =  FLT_MAX;
        m_EndPointsZ[1].nIndex |= 0xFFFFFFFF;

        m_nNumPairs = 0;
    }

    uint CComponentCollidable::AddObject( const RAABB& box, uint nObject )
    {
        return 0;
    }

    void CComponentCollidable::UpdateObject( const RAABB& box, uint nBox )
    {
        TEndPoint& endA = m_EndPointsX[0];
        TEndPoint& endB = m_EndPointsX[1];

        //////////////////////////////////////////
        // X Axis first
        float fMinX = box.min.x;
        float fMaxX = box.max.x;
        uint  nMinXIndex = m_Boxes[ nBox ].nMin[0];
        uint  nMaxXIndex = m_Boxes[ nBox ].nMax[0];
        uint  nIndex;

        // Update the values
        m_EndPointsX[ nMinXIndex ].fValue = fMinX;
        m_EndPointsX[ nMaxXIndex ].fValue = fMaxX;

        // Now sort
        //////////////////////////////////////////
        // Mins first
        nIndex = nMinXIndex;

        endA = m_EndPointsX[ nIndex ];
        endB = m_EndPointsX[ nIndex-1 ];
        // First move left
        while( endA.fValue < endB.fValue )
        {
            if( !IsMin( endB.nIndex ) )
            {
                // The min passed a max, report a collision
                AddPair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            --nIndex;

            endA = m_EndPointsX[ nIndex ];
            endB = m_EndPointsX[ nIndex-1 ];
        }
        // Then right
        endA = m_EndPointsX[ nIndex ];
        endB = m_EndPointsX[ nIndex+1 ];

        while( endA.fValue > endB.fValue )
        {
            if( !IsMin( endB.nIndex ) )
            {
                // The min passed a max, remove the collision
                RemovePair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            ++nIndex;

            endA = m_EndPointsX[ nIndex ];
            endB = m_EndPointsX[ nIndex+1 ];
        }

        m_Boxes[ nBox ].nMin[0] = nIndex;
        
        //////////////////////////////////////////
        // Then maxes
        nIndex = nMaxXIndex;

        endA = m_EndPointsX[ nIndex ];
        endB = m_EndPointsX[ nIndex-1 ];
        // First move left
        while( endA.fValue < endB.fValue )
        {
            if( IsMin( endB.nIndex ) )
            {
                // The max passed a max, report a collision
                RemovePair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            --nIndex;

            endA = m_EndPointsX[ nIndex ];
            endB = m_EndPointsX[ nIndex-1 ];
        }
        // Then right
        endA = m_EndPointsX[ nIndex ];
        endB = m_EndPointsX[ nIndex+1 ];

        while( endA.fValue > endB.fValue )
        {
            if( IsMin( endB.nIndex ) )
            {
                // The max passed a max, remove the collision
                AddPair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            ++nIndex;

            endA = m_EndPointsX[ nIndex ];
            endB = m_EndPointsX[ nIndex+1 ];
        }

        m_Boxes[ nBox ].nMax[0] = nIndex;

        
        //////////////////////////////////////////
        // Y Axis first
        float fMinY = box.min.x;
        float fMaxY = box.max.x;
        uint  nMinYIndex = m_Boxes[ nBox ].nMin[1];
        uint  nMaxYIndex = m_Boxes[ nBox ].nMax[1];

        // Update the values
        m_EndPointsY[ nMinYIndex ].fValue = fMinY;
        m_EndPointsY[ nMaxYIndex ].fValue = fMaxY;

        // Now sort
        nIndex = nMinYIndex;

        // First move left
        while( m_EndPointsY[ nIndex ].fValue < m_EndPointsY[ nIndex-1 ].fValue )
        {
            Swap( m_EndPointsY[ nIndex ], m_EndPointsY[ nIndex-1 ] );
            nIndex--;
        }
        // Then right
        while( m_EndPointsY[ nIndex ].fValue > m_EndPointsY[ nIndex+1 ].fValue )
        {
            Swap( m_EndPointsY[ nIndex ], m_EndPointsY[ nIndex+1 ] );
            nIndex++;
        }
        m_Boxes[ nBox ].nMin[1] = nIndex;
        

        // First move left
        nIndex = nMaxYIndex;
        while( m_EndPointsY[ nIndex ].fValue < m_EndPointsY[ nIndex-1 ].fValue )
        {
            Swap( m_EndPointsY[ nIndex ], m_EndPointsY[ nIndex-1 ] );
            nIndex--;
        }
        // Then right
        while( m_EndPointsY[ nIndex ].fValue > m_EndPointsY[ nIndex+1 ].fValue )
        {
            Swap( m_EndPointsY[ nIndex ], m_EndPointsY[ nIndex+1 ] );
            nIndex++;
        }
        m_Boxes[ nBox ].nMax[1] = nIndex;
        
        
        //////////////////////////////////////////
        // Z Axis first
        float fMinZ = box.min.x;
        float fMaxZ = box.max.x;
        uint  nMinZIndex = m_Boxes[ nBox ].nMin[2];
        uint  nMaxZIndex = m_Boxes[ nBox ].nMax[2];

        // Update the values
        m_EndPointsZ[ nMinZIndex ].fValue = fMinZ;
        m_EndPointsZ[ nMaxZIndex ].fValue = fMaxZ;

        // Now sort
        nIndex = nMinZIndex;

        // First move left
        while( m_EndPointsZ[ nIndex ].fValue < m_EndPointsZ[ nIndex-1 ].fValue )
        {
            Swap( m_EndPointsZ[ nIndex ], m_EndPointsZ[ nIndex-1 ] );
            nIndex--;
        }
        // Then right
        while( m_EndPointsZ[ nIndex ].fValue > m_EndPointsZ[ nIndex+1 ].fValue )
        {
            Swap( m_EndPointsZ[ nIndex ], m_EndPointsZ[ nIndex+1 ] );
            nIndex++;
        }
        m_Boxes[ nBox ].nMin[2] = nIndex;


        // First move left
        nIndex = nMaxZIndex;
        while( m_EndPointsZ[ nIndex ].fValue < m_EndPointsZ[ nIndex-1 ].fValue )
        {
            Swap( m_EndPointsZ[ nIndex ], m_EndPointsZ[ nIndex-1 ] );
            nIndex--;
        }
        // Then right
        while( m_EndPointsZ[ nIndex ].fValue > m_EndPointsZ[ nIndex+1 ].fValue )
        {
            Swap( m_EndPointsZ[ nIndex ], m_EndPointsZ[ nIndex+1 ] );
            nIndex++;
        }
        m_Boxes[ nBox ].nMax[2] = nIndex;
    }

    void CComponentCollidable::RemoveObject( uint nBox )
    {
    }

    void CComponentCollidable::AddPair( uint nObject0, uint nObject1 )
    {
        uint64 nPair = (nObject0 << 32) & nObject1;
    }

    void CComponentCollidable::RemovePair( uint nObject0, uint nObject1 )
    {
        uint64 nPair = (nObject0 << 32) & nObject1;
    }

    CComponentCollidable::~CComponentCollidable() 
    {
        SAFE_DELETE( m_pObjectGraph );
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
        m_ObjectSceneNodes[m_nIndex].m_nObject = m_nIndex;
        // Set its parent to null, we don't
        // want to add uninitialized objects to
        // the graph
        m_ObjectSceneNodes[m_nIndex].m_pParent = NULL;

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
        
        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Volume );
        COMPONENT_USE_PREV_DATA( m_ObjectSceneNodes );

        // Perform any custom reattchment
        m_ObjectSceneNodes[m_nIndex].m_pParent = NULL;

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
        TObjectParentNode* pParent = (TObjectParentNode*)m_ObjectSceneNodes[m_nIndex].m_pParent;
        if( pParent )
        {
            pParent->RemoveObject( &m_ObjectSceneNodes[m_nIndex] );
        }

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
        TObjectParentNode* pParent = (TObjectParentNode*)m_ObjectSceneNodes[m_nIndex].m_pParent;
        if( pParent )
        {
            pParent->RemoveObject( &m_ObjectSceneNodes[m_nIndex] );
        }

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
        if( gnShowBoundingVolumes )
        {
            m_pObjectGraph->DrawNode( Engine::GetRenderer(), RVector3( 1.0f, 1.0f, 1.0f ) );
        }

        // Remove dead leaves
        m_pObjectGraph->Prune();

#if PARALLEL_UPDATE
        task_handle_t   nHandle = pTaskManager->PushTask( ProcessBatch, this, m_nNumActiveComponents, 32 );
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
            if( gnShowBoundingVolumes )
            {
                pRenderer->DrawDebugSphere( pComponent->m_Volume[i] );
            }

            // Check against the ground first
            if( pComponent->m_pTerrain->SphereTerrainCollision( pComponent->m_Volume[i] ) )
            {
                int x = 0;
                pManager->PostMessage( eComponentMessageTerrainCollision, pComponent->m_pObjectIndices[ i ], x, pComponent->ComponentType );
            }

            // Then against other objects
            pComponent->ObjectObjectCollision( pComponent->m_pObjectGraph, &pComponent->m_ObjectSceneNodes[i] );
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
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponentCollidable::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        case eComponentMessageTransform:
            {
                RTransform&         newTransform = *((RTransform*)msg.m_pData);
                TObjectLeafNode*    pThisNode = &m_ObjectSceneNodes[nSlot];

                m_Volume[nSlot].position = newTransform.position;  
                float fRad = m_Volume[nSlot].radius;

                RVector3 vMax = RVector3(  fRad,  fRad,  fRad );
                RVector3 vMin = RVector3( -fRad, -fRad, -fRad );

                pThisNode->max = vMax + newTransform.position;
                pThisNode->min = vMin + newTransform.position;


                // If we don't have a parent, see if we can go in the graph
                if( pThisNode->m_pParent == NULL )
                {
                    if( AABBCollision( *m_pObjectGraph, *pThisNode ) )
                    {
                        m_pObjectGraph->AddObjectLeaf( pThisNode );
                    }
                }
                else if( !AABBCollision( *pThisNode->m_pParent, *pThisNode ) )
                {
                    TObjectParentNode* pParent = (TObjectParentNode*)pThisNode->m_pParent;
                    // We're no longer in our parent, remove ourself and
                    //  readd to the list
                    if( pParent->RemoveObject( pThisNode ) )
                    {
                        // We were successfully removed, readd ourselves
                        m_pObjectGraph->AddObjectLeaf( pThisNode );
                    }
                }
            }
            break;
        case eComponentMessageBoundingVolumeType:
            {
            }
            break;
        case eComponentMessageObjectCollision:
            {
            }
            break;
        default:
            {
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  ObjectObjectCollision
    //  Performs object-object collision
    //-----------------------------------------------------------------------------
    void CComponentCollidable::ObjectObjectCollision( TSceneNode* pGraph, TSceneNode* pNode )
    {
        TObjectParentNode* pParentNode = ((TObjectParentNode*)pGraph);

        if( AABBCollision( *pParentNode, *pNode ) )
        {
            if( !pParentNode->m_nLowestParent )
            {
                for( uint i = 0; i < pParentNode->m_nNumChildren; ++i )
                {
                    ObjectObjectCollision( pParentNode->m_pChildren[i], pNode );
                }
            }
            else
            {
                for( uint i = 0; i < pParentNode->m_nNumChildren; ++i )
                {
                    // Don't check against yourself
                    if( pParentNode->m_pChildren[i] == pNode ) continue;

                    // The two objects leaf nodes collide, perform "real" object
                    //   collision between them
                    if( AABBCollision( *pParentNode->m_pChildren[i], *pNode ) )
                    {
                        uint nThisObject = ((TObjectLeafNode*)pNode)->m_nObject;
                        uint nThatObject = ((TObjectLeafNode*)pParentNode->m_pChildren[i])->m_nObject;

                        if( SphereSphereCollision( m_Volume[nThisObject], m_Volume[nThatObject] ) )
                        {
                            Engine::GetObjectManager()->PostMessage( eComponentMessageObjectCollision, m_pObjectIndices[ nThisObject ], nThatObject, ComponentType );
                        }
                    }
                }
            }
        }
    }


} // namespace Riot
