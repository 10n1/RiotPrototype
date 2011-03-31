/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/31/2011 10:44:30 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "ComponentManager.h"
#include "Object.h"
#include "Gfx\Mesh.h"
#include "Gfx\Material.h"
#include "Riot.h"
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
const eComponentMessageType CComponent::MessagesSent[] = 
{
    eNUMCOMPONENTMESSAGES, // can't make a zero-length array
};
const eComponentMessageType CComponent::MessagesReceived[] = 
{
    eNUMCOMPONENTMESSAGES, // can't make a zero-length array
};
const uint CComponent::NumMessagesSent       = 0;
const uint CComponent::NumMessagesReceived   = 0;
//-----------------------------------------------------------------------------

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
const eComponentMessageType CRenderComponent::MessagesReceived[] = 
{
    eComponentMessageTransform,
};
const uint CRenderComponent::NumMessagesReceived   = sizeof( MessagesReceived ) / sizeof( eComponentMessageType );
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CRenderComponent constructor
CRenderComponent::CRenderComponent()
{    
}

// CRenderComponent destructor
CRenderComponent::~CRenderComponent()
{
}

//-----------------------------------------------------------------------------
//  AddComponent
//  "Adds" a component to an object
//-----------------------------------------------------------------------------
uint CRenderComponent::AddComponent( CObject* pObject )
{
    // Get the index of the new component
    uint nIndex = CComponent::AddComponent( pObject );

    // Now initialize this component
    m_pMesh[nIndex] = pObject->GetMesh();
    m_pMaterial[nIndex] = pObject->GetMaterial();

    return nIndex;
}


//-----------------------------------------------------------------------------
//  ProcessComponent
//  Processes the component as necessary
//-----------------------------------------------------------------------------
void CRenderComponent::ProcessComponent( void )
{
    for( uint i = 0; i < m_nNumComponents; ++i )
    {
        m_pMaterial[ i ]->ApplyMaterial();
        m_pMesh[ i ]->DrawMesh();
    }
}


//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CRenderComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{
    switch( msg.m_nMessageType )
    {
    //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
    case eComponentMessageTransform:
        {
            Transform& vTransform = *((Transform*)msg.m_pData);

            m_pMesh[ nSlot ]->m_vPosition = vTransform.vPosition;
            m_pMesh[ nSlot ]->m_vOrientation = vTransform.vOrientation;
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
const eComponentMessageType CUpdateComponent::MessagesReceived[] = 
{
    eComponentMessageTransform,
};
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
    m_Transform[nIndex].vPosition = XMVectorSet( 0.0f, 0.0, 0.0f, 0.0f );
    m_Transform[nIndex].vOrientation = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );

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
        m_Transform[ i ].vPosition += XMVectorSet( 0.0f, Riot::m_fElapsedTime * 0.5f, 0.0f, 0.0f );
        m_Transform[ i ].vOrientation = XMQuaternionMultiply( m_Transform[ i ].vOrientation, XMQuaternionRotationAxis( XMVectorSet( 27.5f * rand(), -82.7f * rand(), 413.7f * rand(), 0.0f ), Riot::m_fElapsedTime ) );

        CComponentManager::GetInstance()->PostMessage( eComponentMessageTransform, m_ppObjects[ i ], &m_Transform[i], ComponentType );
    }
}


//-----------------------------------------------------------------------------
//  ReceiveMessage

//  Receives and processes a message
//-----------------------------------------------------------------------------
void CUpdateComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{
    switch( msg.m_nMessageType )
    {
    //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
    case eComponentMessageTransform:
        {
            Transform& vTransform = *((Transform*)msg.m_pData);

            m_Transform[ nSlot ].vPosition = vTransform.vPosition;
            m_Transform[ nSlot ].vOrientation = vTransform.vOrientation;
        }
        break;

    default:
        {
        }
    }
}

