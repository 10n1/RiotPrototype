/*********************************************************\
File:           ComponentRigidBody.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 9:56:18 PM
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
        COMPONENT_DEFAULT_PRE_ATTACH;
        /********************************/

        // Now initialize this component
        m_Transform[nIndex] = RTransform();
        m_vVelocity[nIndex] = RVector3Zero();
        m_bGravity[nIndex]  = true;

        /********************************/
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::Reattach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;
        /********************************/

        // Perform any custom reattchment

        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_USE_PREV_DATA( m_vVelocity );
        COMPONENT_USE_PREV_DATA( m_bGravity );

        /********************************/
        COMPONENT_DEFAULT_POST_REATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::Detach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH;
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_vVelocity );
        COMPONENT_REORDER_DATA( m_bGravity );

        /********************************/
        COMPONENT_DEFAULT_POST_DETACH;
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_REORDER_SAVE_DATA( m_vVelocity );
        COMPONENT_REORDER_SAVE_DATA( m_bGravity );

        /********************************/
        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentRigidBody::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;
        /********************************/

        // Perform any custom removal stuff

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_REMOVE_PREV_DATA( m_vVelocity );
        COMPONENT_REMOVE_PREV_DATA( m_bGravity );

        /********************************/
        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
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
