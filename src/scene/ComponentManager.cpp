/*********************************************************\
File:           ComponentManager.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/24/2011 12:24:02 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentManager.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "TaskManager.h"

namespace Riot
{
    
    /*****************************************************************************\
    \*****************************************************************************/
#ifdef DEBUG
    template< class TheComponent>
    void CComponentManager::LoadComponent( void )
    {
        m_ppComponents[ TheComponent::ComponentType ] = new TheComponent;
        for( uint i = 0; i < TheComponent::NumMessagesReceived; ++i )
        {
            m_bRegistered[ TheComponent::MessagesReceived[i] ][ TheComponent::ComponentType ] = true;
        }
    }

#define LOAD_COMPONENT( Component ) LoadComponent<Component>()
#else
#define LOAD_COMPONENT( Component )                                  \
    m_ppComponents[ Component::ComponentType ] = new Component;   \
    for( uint i = 0; i < Component::NumMessagesReceived; ++i )       \
    {                                                                   \
    m_bRegistered[ Component::MessagesReceived[i] ][ Component::ComponentType ] = true; \
}
#endif    
    /*****************************************************************************\
    \*****************************************************************************/
    
    /***************************************\
    | class members                         |
    \***************************************/
    const MessageType    CComponentManager::MessagesReceived[] =
    {
        mNULLMESSAGE,
    };
    const uint           CComponentManager::NumMessagesReceived = 0; //ARRAY_LENGTH(MessagesReceived);


    // CComponentManager constructor
    CComponentManager::CComponentManager()
    {
    }

    // CComponentManager destructor
    CComponentManager::~CComponentManager()
    {
    }


    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CComponentManager::Initialize( void )
    {
        Memset( m_bRegistered, 0, sizeof( m_bRegistered ) );
        Memset( m_ppComponents, 0, sizeof( CComponent* ) * eNUMCOMPONENTS );
        Memset( m_pMessages, 0, sizeof( m_pMessages ) );

        m_nNumMessages = 0;

        // Do this for each component
        LOAD_COMPONENT( CRenderComponent );
        LOAD_COMPONENT( CLightComponent );
        LOAD_COMPONENT( CNewtonPhysicsComponent );
        LOAD_COMPONENT( CCollidableComponent );
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CComponentManager::Shutdown( void )
    {
        for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        {
            SAFE_DELETE( m_ppComponents[i] );
        }
    }


    //-----------------------------------------------------------------------------
    //  AddComponent
    //  Adds a component of the specified type
    //-----------------------------------------------------------------------------
    sint CComponentManager::AddComponent( eComponentType nType, uint nObject )
    {
        ASSERT( m_ppComponents[nType] );

        return m_ppComponents[ nType ]->AddComponent( nObject );
    }


    //-----------------------------------------------------------------------------
    //  RemoveComponent
    //  Removes a component of the specified type
    //-----------------------------------------------------------------------------
    void CComponentManager::RemoveComponent( eComponentType nType, uint nIndex )
    {
        m_ppComponents[ nType ]->RemoveComponent( nIndex );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponents
    //  Updates all the components, then resolves issues
    //-----------------------------------------------------------------------------
    void CComponentManager::ProcessComponents( void )
    {
#if PARALLEL_UPDATE
        static CTaskManager* pTaskManager = CTaskManager::GetInstance();

        // First update the components...
        task_handle_t nProcessTask = pTaskManager->PushTask( ParallelProcessComponents, this, eNUMCOMPONENTS, 1 );
        pTaskManager->WaitForCompletion( nProcessTask );

        // ...then resolve any discrepencies and handle messages
        task_handle_t nMessageTask = pTaskManager->PushTask( ParallelProcessComponentMessages, this, m_nNumMessages, 4 );
        pTaskManager->WaitForCompletion( nMessageTask );
#else
        ParallelProcessComponents( this, 0, 0, eNUMCOMPONENTS );
        ParallelProcessComponentMessages( this, 0, 0, m_nNumMessages );
#endif
        m_nNumMessages = 0;
    }
    void CComponentManager::ParallelProcessComponents( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CComponentManager* pManager = (CComponentManager*)pData;

        uint nEnd = nStart + nCount;
        for( uint i = nStart; i < nEnd; ++i )
        {
            pManager->m_ppComponents[i]->ProcessComponent();
        }
    }

    void CComponentManager::ParallelProcessComponentMessages( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CComponentManager* pManager = (CComponentManager*)pData;

        uint nEnd = nStart + nCount;
        for( uint nMessage = nStart; nMessage < nEnd; ++nMessage )
        {   // Loop through the messages        
            pManager->SendMessage( pManager->m_pMessages[ nMessage ] );
        }
    }

    //-----------------------------------------------------------------------------
    //  PostMessage
    //  Posts a message to be processed
    //-----------------------------------------------------------------------------
    void CComponentManager::PostMessage( CComponentMessage& msg )
    {
        ASSERT( m_nNumMessages < MAX_COMPONENT_MESSAGES );

        sint nIndex = AtomicIncrement( &m_nNumMessages ) - 1;
        m_pMessages[ nIndex ] = msg;
    }

    void CComponentManager::PostMessage( eComponentMessageType nType, uint nObject, pvoid pData, eComponentType nOrigin )
    {
        PostMessage( nType, nObject, (nativeuint)pData, nOrigin );
    }

    void CComponentManager::PostMessage( eComponentMessageType nType, uint nObject, nativeuint nData, eComponentType nOrigin )
    {
        ASSERT( m_nNumMessages < MAX_COMPONENT_MESSAGES );

        sint nIndex = AtomicIncrement( &m_nNumMessages ) - 1;

        m_pMessages[ nIndex ].m_nData = nData;
        m_pMessages[ nIndex ].m_nMessageType = nType;
        m_pMessages[ nIndex ].m_nTargetObject = nObject;
        m_pMessages[ nIndex ].m_nOrigin = nOrigin;
    }


    //-----------------------------------------------------------------------------
    //  SendMessage
    //  Sends the message
    //-----------------------------------------------------------------------------
    void CComponentManager::SendMessage( CComponentMessage& msg )
    {
        for( sint nComponent = 0; nComponent < eNUMCOMPONENTS; ++nComponent )
            //for( eComponentType nComponent = 0; nComponent < eNUMCOMPONENTS; ++nComponent ) <-- this would be ideal
        {
            if(     m_bRegistered[ msg.m_nMessageType ][ nComponent ] == false
                ||  msg.m_nOrigin == nComponent )
            {
                // This component hasn't registered for this message type
                // OR this is the originating component
                continue;
            }

            sint nIndex = Engine::GetObjectManager()->GetComponentIndex( msg.m_nTargetObject, (eComponentType)nComponent );

            if( nIndex != -1 )
            {
                m_ppComponents[ nComponent ]->ReceiveMessage( nIndex, msg );
            }
        }
    }

    void CComponentManager::SendMessage( eComponentMessageType nType, uint nObject, pvoid pData, eComponentType nOrigin )
    {
        CComponentMessage msg = { nType, nObject, nOrigin, (nativeuint)pData };
        SendMessage( msg );
    }

    void CComponentManager::SendMessage( eComponentMessageType nType, uint nObject, nativeuint nData, eComponentType nOrigin )
    {
        CComponentMessage msg = { nType, nObject, nOrigin, nData };
        SendMessage( msg );
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CComponentManager::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        default:
            {
                break;
            }
        }
    }

} // namespace Riot
