/*********************************************************\
File:           ComponentCollidable.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/26/2011 3:18:26 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentCollidable.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "TaskManager.h"

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
    }

    CComponentCollidable::~CComponentCollidable() { } 

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
        CTaskManager*      pTaskManager = CTaskManager::GetInstance();

#if PARALLEL_UPDATE
        task_handle_t   nHandle = pTaskManager->PushTask( ProcessBatch, this, m_nNumActiveComponents, 4 );
        pTaskManager->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumActiveComponents );
#endif
    }
    void CComponentCollidable::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CComponentCollidable*   pComponent = (CComponentCollidable*)pData;
        CObjectManager*         pManager = Engine::GetObjectManager();

        uint nEnd = nStart + nCount;
        
        for( sint i = nStart; i < nEnd; ++i )
        {
            for( sint j = 0; j < pComponent->m_nNumActiveComponents; ++j )
            {
                if( i == j ) continue;

                float fRadSq = pComponent->m_Volume[i].sphere.radius + pComponent->m_Volume[j].sphere.radius;

                RVector3 pos1( pComponent->m_Volume[i].sphere.position );
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

} // namespace Riot
