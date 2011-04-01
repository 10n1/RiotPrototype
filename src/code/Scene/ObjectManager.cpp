/*********************************************************\
File:           ObjectManager.cpp
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       3/31/2011 2:07:56 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ObjectManager.h"

#include <assert.h>

// CObjectManager constructor
CObjectManager::CObjectManager()
    : m_pFreeSlots( NULL )
    , m_nNumFreeSlots( 0 )
    , m_nNumObjects( 0 )
{    
    m_pFreeSlots = new uint[MAX_OBJECTS];
    memset( m_pFreeSlots, 0, sizeof(uint) * MAX_OBJECTS );
}

// CObjectManager destructor
CObjectManager::~CObjectManager()
{
    SAFE_DELETE_ARRAY( m_pFreeSlots );
}
/***************************************\
| class methods                         |
\***************************************/

//-----------------------------------------------------------------------------
//  GetInstance
//  Singleton protection
//-----------------------------------------------------------------------------
CObjectManager* CObjectManager::GetInstance( void )
{
    static CObjectManager pManager;
    return &pManager;
}

//-----------------------------------------------------------------------------
//  CreateObject
//  Creates a new object and returns its index
//-----------------------------------------------------------------------------
uint CObjectManager::CreateObject( void )
{
    assert( m_nNumObjects < MAX_OBJECTS );

    uint nIndex = m_nNumObjects;
    if( m_nNumFreeSlots != 0 )
    {
        nIndex = m_pFreeSlots[ --m_nNumFreeSlots ];
    }
    else
    {
        nIndex = m_nNumObjects++;
    }

    return nIndex;
}

//-----------------------------------------------------------------------------
//  GetObject
//  Returns the object at the specified index
//-----------------------------------------------------------------------------
CObject* CObjectManager::GetObject( uint nIndex )
{
    return &m_pObjects[nIndex];
}

//-----------------------------------------------------------------------------
//  AddComponent
//  Adds a component to the specified object
//-----------------------------------------------------------------------------
void CObjectManager::AddComponent( uint nIndex, eComponentType nType )
{
    if( m_pObjects[nIndex].m_pComponentIndices[ nType ] == -1 )
    {    
        m_pObjects[nIndex].m_pComponentIndices[ nType ] = CComponentManager::GetInstance()->AddComponent( nType, &m_pObjects[nIndex] );
    }
    // TODO: Handle case if it already exists
}

//-----------------------------------------------------------------------------
//  RemoveComponent
//  Removes a component from the specified object
//-----------------------------------------------------------------------------
void CObjectManager::RemoveComponent( uint nIndex, eComponentType nType )
{
    // Make sure we have a component to detach
    sint nComponentIndex = m_pObjects[nIndex].m_pComponentIndices[ nType ];
    if( nComponentIndex != -1 )
    {
        CComponentManager::GetInstance()->RemoveComponent( nType, nComponentIndex );
        m_pObjects[nIndex].m_pComponentIndices[ nType ] = -1;
    }

    // TODO: Handle case if it doesnt exist
}

//-----------------------------------------------------------------------------
//  DeleteObject
//  "Deletes" the object, freeing that slot
//-----------------------------------------------------------------------------
void CObjectManager::DeleteObject( uint nIndex )
{
    m_pObjects[nIndex].Reset();

    m_pFreeSlots[ m_nNumFreeSlots++ ] = nIndex;
    --m_nNumObjects;
}

//-----------------------------------------------------------------------------
//  Accessors/mutators
//-----------------------------------------------------------------------------
uint CObjectManager::GetNumObjects( void )
{
    return m_nNumObjects;
}
