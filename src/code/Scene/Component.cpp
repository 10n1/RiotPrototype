/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/4/2011 10:02:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "ComponentManager.h"
#include "Object.h"
#include "Gfx\Mesh.h"
#include "Gfx\Material.h"
#include "Riot.h"
#include "Gfx\Graphics.h"
#include "Gfx\RenderCommand.h"
#include <memory>
#include <assert.h>

/*****************************************************************************\
\*****************************************************************************/
////
//#define BEGIN_DEFINE_COMPONENT_MESSAGES( Name )                         \
//const eComponentMessageType C##Name##Component::MessagesReceived[] =    \
//{
////
//
////
//#define END_DEFINE_COMPONENT_MESSAGES( Name ) \
//};                                            \
//const uint C##Name##Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
////
//
////
//#define DEFINE_NO_COMPONENT_MESSAGES( Name )                         \
//const eComponentMessageType C##Name##Component::MessagesReceived[] = \
//{                                                                    \
//    eNULLCOMPONENTMESSAGE,                                           \
//};                                                                   \
//const uint C##Name##Component::NumMessagesReceived   = 0;
////
//
////
//#define DEFINE_COMPONENT_CONDESTRUCTOR( Name )      \
//C##Name##Component::C##Name##Component()            \
//{                                                   \
//    m_nMaxComponents = MaxComponents;               \
//    m_ppObjects = new CObject*[MaxComponents];      \
//    memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents ); \
//}                                                   \
//C##Name##Component::~C##Name##Component() { }
////

//
#define BEGIN_DEFINE_COMPONENT( Name )  \
C##Name##Component::C##Name##Component()            \
{                                                   \
    m_nMaxComponents = MaxComponents;               \
    m_ppObjects = new CObject*[MaxComponents];      \
    memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents ); \
}                                                   \
C##Name##Component::~C##Name##Component() { }       \
const eComponentMessageType C##Name##Component::MessagesReceived[] =    \
{
//

//
#define END_DEFINE_COMPONENT( Name ) \
};                                            \
const uint C##Name##Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
//

#define COMPONENT_REORDER_DATA( Data ) Data[nIndex] = Data[ m_nNumComponents - 1  ]

/*****************************************************************************\
\*****************************************************************************/

// CComponent constructor
CComponent::CComponent()
    : m_nNumComponents( 0 )
    , m_nMaxComponents( 0 )
{
    //m_nMaxComponents = MaxComponents;
    //m_ppObjects = new CObject*[MaxComponents];
    //memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents );
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
sint CComponent::AddComponent( CObject* pObject )
{
    if( m_nNumComponents >= m_nMaxComponents )
    {
        // TODO: Handle error more gracefully
        return -1;
    }

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
void CComponent::Attach( uint nIndex ) { }
    
//-----------------------------------------------------------------------------
//  Detach
//  Detaches a component to an object
//-----------------------------------------------------------------------------
void CComponent::Detach( uint nIndex ) { }

//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg ) { }

/*********************************************************************************\
|*********************************************************************************|
|*********************************************************************************|
|*********************************************************************************|
\*********************************************************************************/

//-----------------------------------------------------------------------------
BEGIN_DEFINE_COMPONENT( Render )
    eComponentMessageUpdate, 
    eComponentMessageTransform
END_DEFINE_COMPONENT( Render );
//-----------------------------------------------------------------------------


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
    COMPONENT_REORDER_DATA( m_pMesh );
    COMPONENT_REORDER_DATA( m_pMaterial );
    COMPONENT_REORDER_DATA( m_vPosition );
    COMPONENT_REORDER_DATA( m_vOrientation );
    COMPONENT_REORDER_DATA( m_mWorldMatrix );
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
        // Build the command
        CRenderCommand command = { m_pMaterial[i], m_pMesh[i] };

        // Build the world matrix
        XMMATRIX mWorld = XMMatrixRotationQuaternion( m_vOrientation[i] );
        mWorld = mWorld * XMMatrixTranslationFromVector( m_vPosition[i] );
        mWorld = XMMatrixTranspose( mWorld );
        m_mWorldMatrix[i] = mWorld;

        // Pass to the render engine
        pGfx->AddCommand( command );
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
BEGIN_DEFINE_COMPONENT( Update )
    eComponentMessageTransform,
END_DEFINE_COMPONENT( Update );
//-----------------------------------------------------------------------------


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
    COMPONENT_REORDER_DATA( m_Transform );
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
BEGIN_DEFINE_COMPONENT( Light )
    eComponentMessageTransform
END_DEFINE_COMPONENT( Light );
//-----------------------------------------------------------------------------

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
    COMPONENT_REORDER_DATA( m_vPosition );
    COMPONENT_REORDER_DATA( m_vOrientation );
    COMPONENT_REORDER_DATA( m_bUpdated );
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

            m_vPosition[nSlot]      = vTransform.vPosition;
            m_vOrientation[nSlot]   = vTransform.vOrientation;
            m_bUpdated[nSlot]       = true;
        }
        break;

    default:
        {
        }
    }
}

