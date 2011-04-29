/*********************************************************\
File:           ComponentRigidBody.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/29/2011 1:26:36 PM
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

    static float fMathTest = 0.0f;
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
        
        for( uint i = nStart; i < nEnd; ++i )
        {
            if( i == 1 )
            {
                fMathTest += Engine::m_fElapsedTime;
            }
            if( pComponent->m_bGravity[i] )
            {
                float fDt = Engine::m_fElapsedTime;
                RVector3 pos0 = pComponent->m_Transform[i].position;
                RVector3 vel0 = pComponent->m_vVelocity[i];
                RVector3 acc0 = vGravity;
                RVector3 vel05;
                RVector3 pos1;
                RVector3 vel1;
                RVector3 acc1;
                
                //  1. Calcualte new position
                pos1    = pos0 + vel0*fDt + (0.5f * vGravity * Square(fDt));

                //  2. Calculate 1/2 of the new velocity
                vel05   = vel0 + 0.5f * acc0 * fDt;

                //  3. Calculate the new acceleration
                acc1    = vGravity;

                //  4. Calculate the other half of the new velocity
                vel1    = vel05 + 0.5 * vGravity * fDt;

                pComponent->m_Transform[i].position = pos1;
                pComponent->m_vVelocity[i] = vel1;

                pManager->PostMessage( eComponentMessageTransform, pComponent->m_pObjectIndices[ i ], &pComponent->m_Transform[i], ComponentType );
            }
            else
            {
                pComponent->m_vVelocity[i] = RVector3( 0.0f, 0.0f, 0.0f );
            }
        }
    }

#include <stdio.h>
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
                if( nSlot == 1 )
                {
                    static bool b = false;
                    if( !b )
                    {
                        printf( "Time: %f\n", fMathTest );
                        b = true;
                    }
                }
            }
            break;
        default:
            {
            }
        }
    }

} // namespace Riot
