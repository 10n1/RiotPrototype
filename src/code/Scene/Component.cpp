/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/30/2011 9:54:34 PM
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
            XMVECTOR* vNewData = ((XMVECTOR*)msg.m_pData);

            m_pMesh[ nSlot ]->m_vPosition = vNewData[0];
            m_pMesh[ nSlot ]->m_vOrientation = vNewData[1];

            delete [] vNewData;
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
        f += Riot::m_fElapsedTime;
        XMVECTOR* pNewPos = new XMVECTOR[2];
        pNewPos[0] = XMVectorSet( 0.0f, f, 0.0f, 0.0f );
        pNewPos[1] = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );

        CComponentManager::GetInstance()->PostMessage( eComponentMessageTransform, m_ppObjects[ i ], pNewPos );
    }
}


//-----------------------------------------------------------------------------
//  ReceiveMessage
//  Receives and processes a message
//-----------------------------------------------------------------------------
void CUpdateComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
{
}

