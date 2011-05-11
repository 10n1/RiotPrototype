/*********************************************************\
File:           ComponentCollidable.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/10/2011 9:56:42 PM
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
        m_nNumBoxes = 1;
    }

    uint CComponentCollidable::AddObject( const RAABB& box, uint nObject )
    {
        uint nArrayEnd = m_nNumBoxes * 2;

        //////////////////////////////////////////
        // Add the box to the axes
        m_EndPointsX[ nArrayEnd + 1 ] = m_EndPointsX[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsX[ nArrayEnd - 1 ].fValue = box.min.x;
        m_EndPointsX[ nArrayEnd - 1 ].nIndex = nObject & eSAPMinMask;
        m_EndPointsX[ nArrayEnd     ].fValue = box.max.x;
        m_EndPointsX[ nArrayEnd     ].nIndex = nObject;
        
        m_EndPointsY[ nArrayEnd + 1 ] = m_EndPointsY[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsY[ nArrayEnd - 1 ].fValue = box.min.y;
        m_EndPointsY[ nArrayEnd - 1 ].nIndex = nObject & eSAPMinMask;
        m_EndPointsY[ nArrayEnd     ].fValue = box.max.y;
        m_EndPointsY[ nArrayEnd     ].nIndex = nObject;
        
        m_EndPointsZ[ nArrayEnd + 1 ] = m_EndPointsZ[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsZ[ nArrayEnd - 1 ].fValue = box.min.z;
        m_EndPointsZ[ nArrayEnd - 1 ].nIndex = nObject & eSAPMinMask;
        m_EndPointsZ[ nArrayEnd     ].fValue = box.max.z;
        m_EndPointsZ[ nArrayEnd     ].nIndex = nObject;

        //////////////////////////////////////////
        // Now allocate a box
        TSAPBox& SAPbox = m_Boxes[ nObject ];
        SAPbox.nMin[0] = nArrayEnd-1;
        SAPbox.nMin[1] = nArrayEnd-1;
        SAPbox.nMin[2] = nArrayEnd-1;
        SAPbox.nMax[0] = nArrayEnd;
        SAPbox.nMax[1] = nArrayEnd;
        SAPbox.nMax[2] = nArrayEnd;

        SAPbox.nObject = nObject;

        //////////////////////////////////////////
        // Then force an update
        UpdateObject( box, nObject );

        return m_nNumBoxes++;
    }

    void CComponentCollidable::UpdateObject( const RAABB& box, uint nBox )
    {
        // Update the 3 axes
        UpdateXAxis( box.min.x, box.max.x, nBox );
        UpdateYAxis( box.min.y, box.max.y, nBox );
        UpdateZAxis( box.min.z, box.max.z, nBox );
    }

    void CComponentCollidable::UpdateXAxis( float fMin, float fMax, uint nBox )
    {
        TEndPoint& endA = m_EndPointsX[0];
        TEndPoint& endB = m_EndPointsX[1];

        //////////////////////////////////////////
        // X Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[0];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[0];
        sint  nIndex;

        // Update the values
        m_EndPointsX[ nMinIndex ].fValue = fMin;
        m_EndPointsX[ nMaxIndex ].fValue = fMax;

        // Now sort
        //////////////////////////////////////////
        // Mins first
        nIndex = nMinIndex;

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
        nIndex = nMaxIndex;

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
    }

    void CComponentCollidable::UpdateYAxis( float fMin, float fMax, uint nBox )
    {
        TEndPoint& endA = m_EndPointsY[0];
        TEndPoint& endB = m_EndPointsY[1];

        //////////////////////////////////////////
        // Y Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[1];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[1];
        sint  nIndex;

        // Update the values
        m_EndPointsY[ nMinIndex ].fValue = fMin;
        m_EndPointsY[ nMaxIndex ].fValue = fMax;

        // Now sort
        //////////////////////////////////////////
        // Mins first
        nIndex = nMinIndex;

        endA = m_EndPointsY[ nIndex ];
        endB = m_EndPointsY[ nIndex-1 ];
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

            endA = m_EndPointsY[ nIndex ];
            endB = m_EndPointsY[ nIndex-1 ];
        }
        // Then right
        endA = m_EndPointsY[ nIndex ];
        endB = m_EndPointsY[ nIndex+1 ];

        while( endA.fValue > endB.fValue )
        {
            if( !IsMin( endB.nIndex ) )
            {
                // The min passed a max, remove the collision
                RemovePair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            ++nIndex;

            endA = m_EndPointsY[ nIndex ];
            endB = m_EndPointsY[ nIndex+1 ];
        }

        m_Boxes[ nBox ].nMin[1] = nIndex;
        
        //////////////////////////////////////////
        // Then maxes
        nIndex = nMaxIndex;

        endA = m_EndPointsY[ nIndex ];
        endB = m_EndPointsY[ nIndex-1 ];
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

            endA = m_EndPointsY[ nIndex ];
            endB = m_EndPointsY[ nIndex-1 ];
        }
        // Then right
        endA = m_EndPointsY[ nIndex ];
        endB = m_EndPointsY[ nIndex+1 ];

        while( endA.fValue > endB.fValue )
        {
            if( IsMin( endB.nIndex ) )
            {
                // The max passed a max, remove the collision
                AddPair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            ++nIndex;

            endA = m_EndPointsY[ nIndex ];
            endB = m_EndPointsY[ nIndex+1 ];
        }

        m_Boxes[ nBox ].nMax[1] = nIndex;
    }

    void CComponentCollidable::UpdateZAxis( float fMin, float fMax, uint nBox )
    {
        TEndPoint& endA = m_EndPointsZ[0];
        TEndPoint& endB = m_EndPointsZ[1];

        //////////////////////////////////////////
        // Z Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[2];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[2];
        sint  nIndex;

        // Update the values
        m_EndPointsZ[ nMinIndex ].fValue = fMin;
        m_EndPointsZ[ nMaxIndex ].fValue = fMax;

        // Now sort
        //////////////////////////////////////////
        // Mins first
        nIndex = nMinIndex;

        endA = m_EndPointsZ[ nIndex ];
        endB = m_EndPointsZ[ nIndex-1 ];
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

            endA = m_EndPointsZ[ nIndex ];
            endB = m_EndPointsZ[ nIndex-1 ];
        }
        // Then right
        endA = m_EndPointsZ[ nIndex ];
        endB = m_EndPointsZ[ nIndex+1 ];

        while( endA.fValue > endB.fValue )
        {
            if( !IsMin( endB.nIndex ) )
            {
                // The min passed a max, remove the collision
                RemovePair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            ++nIndex;

            endA = m_EndPointsZ[ nIndex ];
            endB = m_EndPointsZ[ nIndex+1 ];
        }

        m_Boxes[ nBox ].nMin[2] = nIndex;
        
        //////////////////////////////////////////
        // Then maxes
        nIndex = nMaxIndex;

        endA = m_EndPointsZ[ nIndex ];
        endB = m_EndPointsZ[ nIndex-1 ];
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

            endA = m_EndPointsZ[ nIndex ];
            endB = m_EndPointsZ[ nIndex-1 ];
        }
        // Then right
        endA = m_EndPointsZ[ nIndex ];
        endB = m_EndPointsZ[ nIndex+1 ];

        while( endA.fValue > endB.fValue )
        {
            if( IsMin( endB.nIndex ) )
            {
                // The max passed a max, remove the collision
                AddPair( endA.nIndex & eSAPClearMask, endB.nIndex & eSAPClearMask );
            }
            Swap( endA, endB );
            ++nIndex;

            endA = m_EndPointsZ[ nIndex ];
            endB = m_EndPointsZ[ nIndex+1 ];
        }

        m_Boxes[ nBox ].nMax[2] = nIndex;
    }

    void CComponentCollidable::RemoveObject( uint nBox )
    {
        --m_nNumBoxes;
        uint nArrayEnd = m_nNumBoxes * 2;

        RAABB inf( RVector3( FLT_MAX - 2.0f, FLT_MAX - 2.0f, FLT_MAX - 2.0f ), RVector3( FLT_MAX - 1.0f, FLT_MAX - 1.0f, FLT_MAX - 1.0f ) );

        // Move the box to infinity, automatically removing all pairs
        UpdateObject( inf, nBox );

        // Now remove it from the array
        m_EndPointsX[ nArrayEnd ] = m_EndPointsX[ nArrayEnd+2 ];
        m_EndPointsY[ nArrayEnd ] = m_EndPointsY[ nArrayEnd+2 ];
        m_EndPointsZ[ nArrayEnd ] = m_EndPointsZ[ nArrayEnd+2 ];
    }

    void CComponentCollidable::AddPair( uint nObject0, uint nObject1 )
    {
        uint64 nPairA = (nObject0 << 32) | nObject1;
        uint64 nPairB = (nObject1 << 32) | nObject0;

        // XOR them together so that A colliding with B is the same as
        //  B colliding with A
        uint64 nPair = nPairA ^ nPairB;

        // See if the collision already exists
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( m_Pairs[i].nPair == nPair )
            {
                m_Pairs[i].nCount++;
                return;
            }
        }

        // It doesn't, add it
        m_Pairs[ m_nNumPairs ].nCount = 1;
        m_Pairs[ m_nNumPairs ].nPair = nPair;
        m_Pairs[ m_nNumPairs ].nObject0 = nObject0;
        m_Pairs[ m_nNumPairs ].nObject1 = nObject1;

        ++m_nNumPairs;
    }

    void CComponentCollidable::RemovePair( uint nObject0, uint nObject1 )
    {
        uint64 nPairA = (nObject0 << 32) | nObject1;
        uint64 nPairB = (nObject1 << 32) | nObject0;

        // XOR them together so that A colliding with B is the same as
        //  B colliding with A
        uint64 nPair = nPairA ^ nPairB;

        // Make sure the collision already exists
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( m_Pairs[i].nPair == nPair )
            {
                --m_Pairs[i].nCount;

                // Theres no more collision on any axis
                if( m_Pairs[i].nCount == 0 )
                {
                    Swap( m_Pairs[i], m_Pairs[ --m_nNumPairs ] );
                }
                return;
            }
        }

        // It doesn't, assert for now
        ASSERT( false );
    }

        //bool Overlap( uint nObject0, uint nObject1 );

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

        RAABB inf( RVector3( FLT_MAX - 2.0f, FLT_MAX - 2.0f, FLT_MAX - 2.0f ), RVector3( FLT_MAX - 1.0f, FLT_MAX - 1.0f, FLT_MAX - 1.0f ) );

        AddObject( inf, nObject );

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

        RemoveObject( nObject );

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

        //////////////////////////////////////////
        // Now process them
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( m_Pairs[i].nCount == 3 )
            {
                Engine::GetObjectManager()->PostMessage( eComponentMessageObjectCollision, m_pObjectIndices[ m_Pairs[i].nObject0 ], m_Pairs[i].nObject1, ComponentType );
            }
        }

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
            //pComponent->ObjectObjectCollision( pComponent->m_pObjectGraph, &pComponent->m_ObjectSceneNodes[i] );
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

                UpdateObject( RAABB( pThisNode->min, pThisNode->max ), m_pObjectIndices[ nSlot ] );
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
