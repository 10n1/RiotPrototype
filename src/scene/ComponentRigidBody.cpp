/*********************************************************\
File:           ComponentRigidBody.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/26/2011 3:18:25 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentRigidBody.h"
#include "Engine.h"
#include "ObjectManager.h"

/*
CComponentRigidBody
2
eComponentMessageTransform
eComponentMessageCollision
3
RTransform m_Transform
RVector3 m_vVelocity
bool m_bGravity
*/

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    CComponentRigidBody* CComponentRigidBody::m_pInstance = NULL;

    const eComponentMessageType CComponentRigidBody::MessagesReceived[] =
    {
        eComponentMessageTransform,
        eComponentMessageCollision,
    };
    const uint CComponentRigidBody::NumMessagesReceived = (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType );

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentRigidBody::CComponentRigidBody()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;
    }

    CComponentRigidBody::~CComponentRigidBody() { } 

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::Attach( uint nObject )
    {
        PreAttach( nObject );
        /********************************/

        // Now initialize this component
        m_Transform[m_nIndex] = RTransform();
        m_vVelocity[m_nIndex] = RVector3Zero();
        m_bGravity[m_nIndex]  = true;

        /********************************/
        PostAttach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::Reattach( uint nObject )
    {
        PreReattach( nObject );
        PreAttach( nObject );
        /********************************/

        // Perform any custom reattchment

        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_USE_PREV_DATA( m_vVelocity );
        COMPONENT_USE_PREV_DATA( m_bGravity );

        /********************************/
        PostReattach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::Detach( uint nObject )
    {
        PreDetach( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_vVelocity );
        COMPONENT_REORDER_DATA( m_bGravity );

        /********************************/
        PostDetach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::DetachAndSave( uint nObject )
    {
        PreDetachAndSave( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_REORDER_SAVE_DATA( m_vVelocity );
        COMPONENT_REORDER_SAVE_DATA( m_bGravity );

        /********************************/
        PostDetachAndSave( nObject );
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::RemoveInactive( uint nObject )
    {
        PreRemoveInactive( nObject );
        /********************************/

        // Perform any custom removal stuff

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_REMOVE_PREV_DATA( m_vVelocity );
        COMPONENT_REMOVE_PREV_DATA( m_bGravity );

        /********************************/
        PostRemoveInactive( nObject );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::ProcessComponent( void )
    {
#if 0 //PARALLEL_UPDATE
        task_handle_t nHandle = Engine::GetTaskManager()->PushTask( ProcessBatch, this, m_nNumActiveComponents, 16 );
        Engine::GetTaskManager()->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumActiveComponents );
#endif
    }
    void CComponentRigidBody::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        static const RVector3 vGravity = RVector3( 0.0f, -9.8f, 0.0f );
        CComponentRigidBody*    pComponent = (CComponentRigidBody*)pData;
        CObjectManager*         pManager = Engine::GetObjectManager();
    
        uint nEnd = nStart + nCount;
        
        for( sint i = nStart; i < nEnd; ++i )
        {
            if( pComponent->m_bGravity[i] )
            {
                pComponent->m_vVelocity[i] += vGravity * Engine::m_fElapsedTime;
            }
            else
            {
                pComponent->m_vVelocity[i] = RVector3( 0.0f, 0.0f, 0.0f );
            }

            pComponent->m_Transform[i].position += pComponent->m_vVelocity[i] * Engine::m_fElapsedTime;

            pManager->PostMessage( eComponentMessageTransform, pComponent->m_pObjectIndices[ i ], &pComponent->m_Transform[i], ComponentType );
        }
    }

    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                m_Transform[nSlot] = transform;
            }
            break;
        case eComponentMessageCollision:
            {
                m_bGravity[nSlot] = false;
            }
            break;
        default:
            {
            }
        }
    }

} // namespace Riot
