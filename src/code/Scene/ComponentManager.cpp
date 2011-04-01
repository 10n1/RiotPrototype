/*********************************************************\
File:           ComponentManager.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/31/2011 2:03:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentManager.h"
#include "Object.h"

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

#define LOAD_COMPONENT( TheComponent ) LoadComponent<TheComponent>()
#else
#define LOAD_COMPONENT( TheComponent )                                  \
    m_ppComponents[ TheComponent::ComponentType ] = new TheComponent;   \
    for( uint i = 0; i < TheComponent::NumMessagesReceived; ++i )       \
    {                                                                   \
        m_bRegistered[ TheComponent::MessagesReceived[i] ][ TheComponent::ComponentType ] = true; \
    }
#endif


// CComponentManager constructor
CComponentManager::CComponentManager()
    : m_nNumMessages( 0 )
{
    memset( m_bRegistered, 0, sizeof( m_bRegistered ) );
    memset( m_ppComponents, 0, sizeof( CComponent* ) * eNUMCOMPONENTS );


    // Do this for each component
    LOAD_COMPONENT( CUpdateComponent );
    LOAD_COMPONENT( CRenderComponent );
}

// CComponentManager destructor
CComponentManager::~CComponentManager()
{
    for( uint i = 0; i < eNUMCOMPONENTS; ++i )
    {
        SAFE_RELEASE( m_ppComponents[i] );
    }
}


//-----------------------------------------------------------------------------
//  GetInstance
//  Singleton creation
//-----------------------------------------------------------------------------
CComponentManager* CComponentManager::GetInstance( void )
{
    static CComponentManager pManager;
    return &pManager;
}

//-----------------------------------------------------------------------------
//  AddComponent
//  Adds a component of the specified type
//-----------------------------------------------------------------------------
uint CComponentManager::AddComponent( eComponentType nType, CObject* pObject )
{
    return m_ppComponents[ nType ]->AddComponent( pObject );
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
    m_pMessages[ m_nNumMessages++ ] = msg;
}

void CComponentManager::PostMessage( eComponentMessageType nType, CObject* pObject, pvoid pData, eComponentType nOrigin )
{
    PostMessage( nType, pObject, (nativeuint)pData, nOrigin );
}

void CComponentManager::PostMessage( eComponentMessageType nType, CObject* pObject, nativeuint nData, eComponentType nOrigin )
{
    m_pMessages[ m_nNumMessages ].m_nData = nData;
    m_pMessages[ m_nNumMessages ].m_nMessageType = nType;
    m_pMessages[ m_nNumMessages ].m_pTargetObject = pObject;
    m_pMessages[ m_nNumMessages ].m_nOrigin = nOrigin;

    m_nNumMessages++;
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

        sint nIndex = msg.m_pTargetObject->GetComponentIndex( (eComponentType)nComponent );

        m_ppComponents[ nComponent ]->ReceiveMessage( nIndex, msg );
    }
}

void CComponentManager::SendMessage( eComponentMessageType nType, CObject* pObject, pvoid pData, eComponentType nOrigin )
{
    CComponentMessage msg = { nType, pObject, nOrigin, (nativeuint)pData };
    SendMessage( msg );
}

void CComponentManager::SendMessage( eComponentMessageType nType, CObject* pObject, nativeuint nData, eComponentType nOrigin )
{
    CComponentMessage msg = { nType, pObject, nOrigin, nData };
    SendMessage( msg );
}
