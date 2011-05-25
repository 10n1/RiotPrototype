/*********************************************************\
File:           ComponentCollidable.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/11/2011 10:34:57 PM
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

        m_nNumPairs = 0;
        m_nNumBoxes = 1;

        Memset( m_EndPointsX, 0, sizeof( m_EndPointsX ) );
        Memset( m_EndPointsY, 0, sizeof( m_EndPointsY ) );
        Memset( m_EndPointsZ, 0, sizeof( m_EndPointsZ ) );
        Memset( m_Boxes, 0, sizeof( m_Boxes ) );

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
    }

    uint CComponentCollidable::AddObject( const RAABB& box, uint nObject )
    {
        uint nArrayEnd = m_nNumBoxes * 2;

        //////////////////////////////////////////
        // Add the box to the axes
        m_EndPointsX[ nArrayEnd + 1 ] = m_EndPointsX[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsX[ nArrayEnd - 1 ].fValue = box.min.x;
        m_EndPointsX[ nArrayEnd - 1 ].nIndex = nObject | eSAPMinMask;
        m_EndPointsX[ nArrayEnd     ].fValue = box.max.x;
        m_EndPointsX[ nArrayEnd     ].nIndex = nObject;
        
        m_EndPointsY[ nArrayEnd + 1 ] = m_EndPointsY[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsY[ nArrayEnd - 1 ].fValue = box.min.y;
        m_EndPointsY[ nArrayEnd - 1 ].nIndex = nObject | eSAPMinMask;
        m_EndPointsY[ nArrayEnd     ].fValue = box.max.y;
        m_EndPointsY[ nArrayEnd     ].nIndex = nObject;
        
        m_EndPointsZ[ nArrayEnd + 1 ] = m_EndPointsZ[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsZ[ nArrayEnd - 1 ].fValue = box.min.z;
        m_EndPointsZ[ nArrayEnd - 1 ].nIndex = nObject | eSAPMinMask;
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
        static CMutex m;
        CScopedMutex lock( &m );
        // Update the 3 axes
        UpdateXAxis( box.min.x, box.max.x, nBox );
        UpdateYAxis( box.min.y, box.max.y, nBox );
        UpdateZAxis( box.min.z, box.max.z, nBox );
    }

    void CComponentCollidable::UpdateXAxis( float fMin, float fMax, uint nBox )
    {
        //////////////////////////////////////////
        // X Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[0];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[0];
        sint  nIndex;

        float fOldMin = m_EndPointsX[ nMinIndex ].fValue;
        float fOldMax = m_EndPointsX[ nMaxIndex ].fValue;

        m_EndPointsX[ nMinIndex ].fValue = fMin;
        m_EndPointsX[ nMaxIndex ].fValue = fMax;

        //////////////////////////////////////////
        // First move the min, left or right
        if( fMin < fOldMin )
        {
            sint nThat = nMinIndex-1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue < m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMax( m_EndPointsX[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }

                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                --nThis;
                --nThat;
            }

            m_Boxes[ nBox ].nMin[0] = nThis;
        }
        else
        {
            sint nThat = nMinIndex+1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue > m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMax( m_EndPointsX[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }


                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                ++nThis;
                ++nThat;
            }

            m_Boxes[ nBox ].nMin[0] = nThis;
        }
        
        //////////////////////////////////////////
        // Then the max, left or right
        if( fMax < fOldMax )
        {
            sint nThat = nMaxIndex-1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue < m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMin( m_EndPointsX[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }

                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                --nThis;
                --nThat;
            }
            m_Boxes[ nBox ].nMax[0] = nThis;
        }
        else
        {
            sint nThat = nMaxIndex+1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue > m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMin( m_EndPointsX[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }

                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                ++nThis;
                ++nThat;
            }
            m_Boxes[ nBox ].nMax[0] = nThis;
        }
    }

    void CComponentCollidable::UpdateYAxis( float fMin, float fMax, uint nBox )
    {
        //////////////////////////////////////////
        // Y Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[1];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[1];
        sint  nIndex;

        float fOldMin = m_EndPointsY[ nMinIndex ].fValue;
        float fOldMax = m_EndPointsY[ nMaxIndex ].fValue;

        m_EndPointsY[ nMinIndex ].fValue = fMin;
        m_EndPointsY[ nMaxIndex ].fValue = fMax;

        //////////////////////////////////////////
        // First move the min, left or right
        if( fMin < fOldMin )
        {
            sint nThat = nMinIndex-1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue < m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMax( m_EndPointsY[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }

                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                --nThis;
                --nThat;
            }

            m_Boxes[ nBox ].nMin[1] = nThis;
        }
        else
        {
            sint nThat = nMinIndex+1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue > m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMax( m_EndPointsY[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }


                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                ++nThis;
                ++nThat;
            }

            m_Boxes[ nBox ].nMin[1] = nThis;
        }
        
        //////////////////////////////////////////
        // Then the max, left or right
        if( fMax < fOldMax )
        {
            sint nThat = nMaxIndex-1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue < m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMin( m_EndPointsY[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }

                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                --nThis;
                --nThat;
            }
            m_Boxes[ nBox ].nMax[1] = nThis;
        }
        else
        {
            sint nThat = nMaxIndex+1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue > m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMin( m_EndPointsY[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }

                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                ++nThis;
                ++nThat;
            }
            m_Boxes[ nBox ].nMax[1] = nThis;
        }
    }

    void CComponentCollidable::UpdateZAxis( float fMin, float fMax, uint nBox )
    {
        //////////////////////////////////////////
        // Z Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[2];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[2];
        sint  nIndex;

        float fOldMin = m_EndPointsZ[ nMinIndex ].fValue;
        float fOldMax = m_EndPointsZ[ nMaxIndex ].fValue;

        m_EndPointsZ[ nMinIndex ].fValue = fMin;
        m_EndPointsZ[ nMaxIndex ].fValue = fMax;

        //////////////////////////////////////////
        // First move the min, left or right
        if( fMin < fOldMin )
        {
            sint nThat = nMinIndex-1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue < m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMax( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }

                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                --nThis;
                --nThat;
            }

            m_Boxes[ nBox ].nMin[2] = nThis;
        }
        else
        {
            sint nThat = nMinIndex+1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue > m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMax( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }


                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                ++nThis;
                ++nThat;
            }

            m_Boxes[ nBox ].nMin[2] = nThis;
        }
        
        //////////////////////////////////////////
        // Then the max, left or right
        if( fMax < fOldMax )
        {
            sint nThat = nMaxIndex-1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue < m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMin( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }

                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                --nThis;
                --nThat;
            }
            m_Boxes[ nBox ].nMax[2] = nThis;
        }
        else
        {
            sint nThat = nMaxIndex+1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue > m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMin( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }

                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                ++nThis;
                ++nThat;
            }
            m_Boxes[ nBox ].nMax[2] = nThis;
        }
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
        // See if they even overlap first
        if( !Overlap( nObject0, nObject1 ) )
        {
            return;
        }


        uint64 nPairA = nObject0;
        nPairA = ( nPairA << 32) | nObject1;
        uint64 nPairB = nObject1;
        nPairB = ( nPairB << 32) | nObject0;

        // XOR them together so that A colliding with B is the same as
        //  B colliding with A
        uint64 nPair = nPairA + nPairB;

        // See if the collision already exists
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( CheckPair( m_Pairs[i], nObject0, nObject1 ) )
            {
                return;
            }
        }

        // It doesn't, add it
        m_Pairs[ m_nNumPairs ].nPair = nPair;
        m_Pairs[ m_nNumPairs ].nObject0 = nObject0;
        m_Pairs[ m_nNumPairs ].nObject1 = nObject1;

        ++m_nNumPairs;
    }

    void CComponentCollidable::RemovePair( uint nObject0, uint nObject1 )
    {
        uint64 nPairA = nObject0;
        nPairA = ( nPairA << 32) | nObject1;
        uint64 nPairB = nObject1;
        nPairB = ( nPairB << 32) | nObject0;

        // XOR them together so that A colliding with B is the same as
        //  B colliding with A
        uint64 nPair = nPairA + nPairB;

        // Make sure the collision already exists
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( CheckPair( m_Pairs[i], nObject0, nObject1 ) )
            {
                Swap( m_Pairs[i], m_Pairs[ --m_nNumPairs ] );
                return;
            }
        }

        // It doesn't, assert for now
        //ASSERT( false );
    }

    CComponentCollidable::~CComponentCollidable() 
    {
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

        static const RAABB inf( RVector3( FLT_MAX - 2.0f, FLT_MAX - 2.0f, FLT_MAX - 2.0f ), RVector3( FLT_MAX - 1.0f, FLT_MAX - 1.0f, FLT_MAX - 1.0f ) );

        AddObject( inf, m_nIndex );

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

        // Perform any custom reattchment

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
        RemoveObject( nObject );

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Volume );

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
        // Process pairs
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            //if( m_Pairs[i].nCount == 3 )
            {
                //Engine::GetObjectManager()->PostMessage( eComponentMessageObjectCollision, m_pObjectIndices[ m_Pairs[i].nObject0 ], m_Pairs[i].nObject1, ComponentType );
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
                //pManager->PostMessage( eComponentMessageTerrainCollision, pComponent->m_pObjectIndices[ i ], x, pComponent->ComponentType );
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

                m_Volume[nSlot].position = newTransform.position;  
                float fRad = m_Volume[nSlot].radius;

                RVector3 vMax = RVector3(  fRad,  fRad,  fRad );
                RVector3 vMin = RVector3( -fRad, -fRad, -fRad );

                vMax = vMax + newTransform.position;
                vMin = vMin + newTransform.position;

                UpdateObject( RAABB( vMin, vMax ), nSlot );
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

} // namespace Riot
