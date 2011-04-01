/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/1/2011 12:29:18 AM
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
    : m_ppObjects( NULL )
    , m_nNumComponents( 0 )
{
    m_ppObjects = new CObject*[MAX_OBJECTS];
    memset( m_ppObjects, 0, sizeof(CObject*) * MAX_OBJECTS );
}

// CComponent destructor
CComponent::~CComponent()
{
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
    uint nIndex = m_nNumComponents++;
    m_ppObjects[ nIndex ] = pObject;

    // ...then attach to it
    Attach( nIndex );

    return nIndex;
}

//-----------------------------------------------------------------------------
//  RemoveComponent
//  "Removes" a component to an object
//-----------------------------------------------------------------------------
void CComponent::RemoveComponent( uint nIndex )
{
    // First detach...
    Detach( nIndex );

    // ...then clean up the list
    m_ppObjects[ nIndex ] = m_ppObjects[ --m_nNumComponents ];
}

//-----------------------------------------------------------------------------
//  Attach
//  Attaches a component to an object
//-----------------------------------------------------------------------------
void CComponent::Attach( uint nIndex )
{
}
    
//-----------------------------------------------------------------------------
//  Detach
//  Detaches a component to an object
//-----------------------------------------------------------------------------
void CComponent::Detach( uint nIndex )
{
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
//  Attach
//  Attaches a component to an object
//-----------------------------------------------------------------------------
void CRenderComponent::Attach( uint nIndex )
{
    // Now initialize this component
    CObject* pObject = m_ppObjects[ nIndex ];
    m_pMesh[nIndex] = pObject->GetMesh();
    m_pMaterial[nIndex] = pObject->GetMaterial();
}

//-----------------------------------------------------------------------------
//  Detach
//  Detaches a component to an object
//-----------------------------------------------------------------------------
void CRenderComponent::Detach( uint nIndex )
{
    // Now initialize this component
    m_pMesh[nIndex] = m_pMesh[ m_nNumComponents - 1  ];
    m_pMaterial[nIndex] = m_pMaterial[ m_nNumComponents - 1  ];
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
    //: m_nComponentType( ComponentType )
{
}

// CUpdateComponent destructor
CUpdateComponent::~CUpdateComponent()
{
}

//-----------------------------------------------------------------------------
//  Attach
//  Attaches a component to an object
//-----------------------------------------------------------------------------
void CUpdateComponent::Attach( uint nIndex )
{
    // Now initialize this component
    m_Transform[nIndex].vPosition = XMVectorSet( 0.0f, 0.0, 0.0f, 0.0f );
    m_Transform[nIndex].vOrientation = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
}


//-----------------------------------------------------------------------------
//  Detach
//  Detaches a component to an object
//-----------------------------------------------------------------------------
void CUpdateComponent::Detach( uint nIndex )
{
    // Now initialize this component
    m_Transform[nIndex] = m_Transform[ m_nNumComponents - 1 ];
}

//-----------------------------------------------------------------------------
//  ProcessComponent
//  Processes the component as necessary
//-----------------------------------------------------------------------------
void CUpdateComponent::ProcessComponent( void )
{
    srand( *((uint*)(&Riot::m_fElapsedTime)) );
    for( uint i = 0; i < m_nNumComponents; ++i )
    {
        m_Transform[ i ].vPosition += XMVectorSet( 0.0f, Riot::m_fElapsedTime * 0.05f, 0.0f, 0.0f );
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

