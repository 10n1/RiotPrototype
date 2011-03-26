/*********************************************************\
File:           ComponentManager.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 11:42:13 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentManager.h"

// CComponentManager constructor
CComponentManager::CComponentManager()
    : m_nNumMessages( 0 )
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

//-----------------------------------------------------------------------------
//  ProcessComponents
//  Updates all the components, then resolves issues
//-----------------------------------------------------------------------------
void CComponentManager::ProcessComponents( void )
{
    // First update the components...
    for( uint i = 0; i < eNUMCOMPONENTS; ++i )
    {
        m_ppComponents[i]->ProcessComponent();
    }

    // ...then resolve any discrepencies
}


//-----------------------------------------------------------------------------
//  PostMessage
//  Posts a message to be processed
//-----------------------------------------------------------------------------
void CComponentManager::PostMessage( eComponentMessageType nType, CObject* pObject, pvoid pData )
{
    PostMessage( nType, pObject, (nativeuint)pData );
}

void CComponentManager::PostMessage( eComponentMessageType nType, CObject* pObject, nativeuint nData )
{
    m_pMessages[ m_nNumMessages ].m_nData = nData;
    m_pMessages[ m_nNumMessages ].m_nMessageType = nType;
    m_pMessages[ m_nNumMessages ].m_pTargetObject = pObject;

    m_nNumMessages++;
}
