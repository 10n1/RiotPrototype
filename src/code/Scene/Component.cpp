/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/3/2011 9:15:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "ComponentManager.h"
#include "Object.h"
#include "Gfx\Mesh.h"
#include "Gfx\Material.h"
#include "Riot.h"
#include "Gfx\Graphics.h"
#include <memory>

// CComponent constructor
CComponent::CComponent()
    : m_nNumComponents( 0 )
{
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
    m_ppObjects = new CObject*[MaxComponents];
    memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents );
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
    m_vPosition[nIndex] = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    m_vOrientation[nIndex] = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
    m_mWorldMatrix[nIndex] = XMMatrixIdentity();
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
    CGraphics* pGfx = Riot::GetGraphics();
    for( uint i = 0; i < m_nNumComponents; ++i )
    {
        pGfx->AddCommand( m_pMaterial[i] );
        pGfx->AddCommand( m_pMesh[i] );
        XMMATRIX mWorld = XMMatrixRotationQuaternion( m_vOrientation[i] );
        mWorld = mWorld * XMMatrixTranslationFromVector( m_vPosition[i] );
        mWorld = XMMatrixTranspose( mWorld );
        m_mWorldMatrix[i] = mWorld;
        pGfx->AddMatrix( &m_mWorldMatrix[i] );
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

            m_vPosition[nSlot] = vTransform.vPosition;
            m_vOrientation[nSlot] = vTransform.vOrientation;
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
    m_ppObjects = new CObject*[MaxComponents];
    memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents );
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


/*********************************************************************************\
|*********************************************************************************|
|*********************************************************************************|
|*********************************************************************************|
\*********************************************************************************/
//-----------------------------------------------------------------------------
const eComponentMessageType CLightComponent::MessagesReceived[] = 
{
    eComponentMessageTransform,
};
const uint CLightComponent::NumMessagesReceived   = sizeof( MessagesReceived ) / sizeof( eComponentMessageType );
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CLightComponent constructor
CLightComponent::CLightComponent()
{    
    m_ppObjects = new CObject*[MaxComponents];
    memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents );
}

// CLightComponent destructor
CLightComponent::~CLightComponent()
{
}


//-----------------------------------------------------------------------------
//  Attach
//  Attaches a component to an object
//-----------------------------------------------------------------------------
void CLightComponent::Attach( uint nIndex )
{
    // Now initialize this component
    CObject* pObject = m_ppObjects[ nIndex ];
    m_vPosition[nIndex] = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    m_vOrientation[nIndex] = XMVectorSet( 1.0f, 1.0f, 1.0f, 0.0f );
    m_bUpdated[nIndex] = true;
}

//-----------------------------------------------------------------------------
//  Detach
//  Detaches a component to an object
//-----------------------------------------------------------------------------
void CLightComponent::Detach( uint nIndex )
{
    // Now initialize this component
}

//-----------------------------------------------------------------------------
//  ProcessComponent
//  Processes the component as necessary
//-----------------------------------------------------------------------------
void CLightComponent::ProcessComponent( void )
{
    for( uint i = 0; i < m_nNumComponents; ++i )
    {
        if( m_bUpdated[i] == true )
        {
            Riot::GetGraphics()->SetLight( m_vOrientation[i], i );
            m_bUpdated[i] = false;
        }
    }
}


//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CLightComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{
    switch( msg.m_nMessageType )
    {
    //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
    case eComponentMessageTransform:
        {
            Transform& vTransform = *((Transform*)msg.m_pData);

            m_vPosition[nSlot] = vTransform.vPosition;
            m_vOrientation[nSlot] = vTransform.vOrientation;
            m_bUpdated[nSlot] = true;
        }
        break;

    default:
        {
        }
    }
}

