/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/29/2011 11:59:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "ComponentManager.h"
#include <memory>

// CComponent constructor
CComponent::CComponent()
    : m_nNumFreeSlots( 0 )
    , m_pFreeSlots( NULL )
    , m_ppObjects( NULL )
    , m_nNumComponents( 0 )
{
    m_pFreeSlots = new uint[MAX_OBJECTS];
    memset( m_pFreeSlots, 0, sizeof(uint) * MAX_OBJECTS );
    m_ppObjects = new CObject*[MAX_OBJECTS];
    memset( m_ppObjects, 0, sizeof(CObject*) * MAX_OBJECTS );
}

// CComponent destructor
CComponent::~CComponent()
{
    SAFE_DELETE_ARRAY( m_pFreeSlots );
    SAFE_DELETE_ARRAY( m_ppObjects );
}

//-----------------------------------------------------------------------------
//  ProcessComponent
//  Processes the component as necessary
//-----------------------------------------------------------------------------
void CComponent::ProcessComponent( void )
{
}

//-----------------------------------------------------------------------------
//  AddComponent
//  "Adds" a component to an object
//-----------------------------------------------------------------------------
uint CComponent::AddComponent( CObject* pObject )
{    
    // Calculate the free spot for this component
    uint nIndex = m_nNumComponents;
    if( m_nNumFreeSlots != 0 )
    {
        nIndex = m_pFreeSlots[ --m_nNumFreeSlots ];
    }
    else
    {
        nIndex = m_nNumComponents++;
    }

    m_ppObjects[ nIndex ] = pObject;

    return nIndex;
}

//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{
}

/*********************************************************************************\
|*********************************************************************************|
|*********************************************************************************|
|*********************************************************************************|
\*********************************************************************************/
//-----------------------------------------------------------------------------
const eComponentMessageType CPositionComponent::MessagesSent[] = 
{
    eComponentMessagePosition,
    eComponentMessageOrientation,
};
const eComponentMessageType CPositionComponent::MessagesReceived[] = 
{
    eComponentMessagePosition,
};
const uint CPositionComponent::NumMessagesSent       = sizeof( MessagesSent ) / sizeof( eComponentMessageType );
const uint CPositionComponent::NumMessagesReceived   = sizeof( MessagesReceived ) / sizeof( eComponentMessageType );
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPositionComponent constructor
CPositionComponent::CPositionComponent()
{    
}

// CPositionComponent destructor
CPositionComponent::~CPositionComponent()
{
}

//-----------------------------------------------------------------------------
//  AddComponent
//  "Adds" a component to an object
//-----------------------------------------------------------------------------
uint CPositionComponent::AddComponent( CObject* pObject )
{
    // Get the index of the new component
    uint nIndex = CComponent::AddComponent( pObject );

    // Now initialize this component
    m_vPosition[nIndex] = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );

    return nIndex;
}

//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CPositionComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{

    switch( msg.m_nMessageType )
    {
    //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
    case eComponentMessagePosition:
        {
            XMVECTOR& vNewPosition = *((XMVECTOR*)msg.m_pData);

            m_vPosition[ nSlot ] = vNewPosition;

            delete ((XMVECTOR*)msg.m_pData);
        }
        break;

    default:
        {
        }
    }
}


/*********************************************************************************\
|*********************************************************************************|
|*********************************************************************************|
|*********************************************************************************|
\*********************************************************************************/
//-----------------------------------------------------------------------------
const eComponentMessageType CUpdateComponent::MessagesSent[] = 
{
    eComponentMessagePosition,
};
const eComponentMessageType CUpdateComponent::MessagesReceived[] = 
{
    // TODO: Allow zero-sized arrays
    eComponentMessagePosition
};
const uint CUpdateComponent::NumMessagesSent       = sizeof( MessagesSent ) / sizeof( eComponentMessageType );
const uint CUpdateComponent::NumMessagesReceived   = sizeof( MessagesReceived ) / sizeof( eComponentMessageType );
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CUpdateComponent constructor
CUpdateComponent::CUpdateComponent()
{    
}

// CUpdateComponent destructor
CUpdateComponent::~CUpdateComponent()
{
}

//-----------------------------------------------------------------------------
//  AddComponent
//  "Adds" a component to an object
//-----------------------------------------------------------------------------
uint CUpdateComponent::AddComponent( CObject* pObject )
{
    // Get the index of the new component
    uint nIndex = CComponent::AddComponent( pObject );

    // Now initialize this component

    return nIndex;
}

//-----------------------------------------------------------------------------
//  ProcessComponent
//  Processes the component as necessary
//-----------------------------------------------------------------------------
void CUpdateComponent::ProcessComponent( void )
{
    for( uint i = 0; i < m_nNumComponents; ++i )
    {

        static float f = 0.0f;
        //CComponentMessage msg;
        //msg.m_pData = new XMVECTOR( XMVectorSet( 0.0f, f, 0.0f, 0.0f ) );
        //msg.m_nMessageType = eComponentMessagePosition;
        //msg.m_pTargetObject = m_ppObjects[ i ];

        f += 1.0f;

        CComponentManager::GetInstance()->PostMessage( eComponentMessagePosition, m_ppObjects[ i ], new XMVECTOR( XMVectorSet( 0.0f, f, 0.0f, 0.0f ) ) );
    }
}


//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CUpdateComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{
}

