/*********************************************************\
File:           ComponentManager.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/19/2011 10:56:47 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentManager.h"
#include "Engine.h"
#include "ObjectManager.h"

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

#define LOAD_COMPONENT( Name ) LoadComponent<C##Name##Component>()
#else
#define LOAD_COMPONENT( Name )                                  \
    m_ppComponents[ C##Name##Component::ComponentType ] = new C##Name##Component;   \
    for( uint i = 0; i < C##Name##Component::NumMessagesReceived; ++i )       \
    {                                                                   \
    m_bRegistered[ C##Name##Component::MessagesReceived[i] ][ C##Name##Component::ComponentType ] = true; \
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

        m_nNumMessages = 0;

        // Do this for each component
        LOAD_COMPONENT( Update );
        LOAD_COMPONENT( Render );
        LOAD_COMPONENT( Light );

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
        // First update the components...
        for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        {
            m_ppComponents[i]->ProcessComponent();
        }

        // ...then resolve any discrepencies and handle messages
        for( uint nMessage = 0; nMessage < m_nNumMessages; ++nMessage )
        {   // Loop through the messages        
            SendMessage( m_pMessages[ nMessage ] );
        }

        m_nNumMessages = 0;
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
        for( uint nComponent = 0; nComponent < eNUMCOMPONENTS; ++nComponent )
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
