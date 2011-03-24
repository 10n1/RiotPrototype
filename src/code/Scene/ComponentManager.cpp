/*********************************************************\
File:           ComponentManager.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 10:40:32 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentManager.h"

// CComponentManager constructor
CComponentManager::CComponentManager()
{
    memset( m_ppComponents, 0, sizeof( CComponent* ) * eNUMCOMPONENTS );

    m_ppComponents[ eComponentPosition ] = new CPositionComponent;
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
