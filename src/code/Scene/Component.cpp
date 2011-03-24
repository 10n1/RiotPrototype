/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 11:15:22 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
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


// CPositionComponent constructor
CPositionComponent::CPositionComponent()
    : m_vPosition( NULL )
{    
    m_vPosition = new XMVECTOR[MAX_OBJECTS];
}

// CPositionComponent destructor
CPositionComponent::~CPositionComponent()
{
    SAFE_DELETE_ARRAY( m_vPosition );
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
