/*********************************************************\
File:           ObjectManager.cpp
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       3/31/2011 10:59:07 AM
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
    m_pObjects[nIndex].AddComponent( nType );
}

