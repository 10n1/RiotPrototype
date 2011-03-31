/*********************************************************\
File:           Object.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/31/2011 11:42:13 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Object.h"
#include "memory.h"
#include "Gfx\Mesh.h"
#include "Gfx\Material.h"
#include "ComponentManager.h"
#define new DEBUG_NEW

// CObject constructor
CObject::CObject()
    : m_pMesh( NULL )
    , m_pMaterial( NULL )
{

    for( uint i = 0; i < eNUMCOMPONENTS; ++i )
    {
        m_pComponentIndices[i] = -1; // -1 means the object doesn't have that component
    }
}

// CObject destructor
CObject::~CObject()
{
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pMaterial );
}


//-----------------------------------------------------------------------------
//  AddComponent
//  Adds a component of the specified type
//-----------------------------------------------------------------------------
void CObject::AddComponent( eComponentType nType )
{
    uint nIndex = CComponentManager::GetInstance()->AddComponent( nType, this );

    m_pComponentIndices[ nType ] = nIndex;
}

//-----------------------------------------------------------------------------
//  GetComponentIndex
//  Returns which slot in the component this object owns
//-----------------------------------------------------------------------------
sint CObject::GetComponentIndex( eComponentType nComponent )
{
    return m_pComponentIndices[ nComponent ];
}

//-----------------------------------------------------------------------------
//  RemoveComponent
//  Removes a component of the specified type
//-----------------------------------------------------------------------------
void CObject::RemoveComponent( eComponentType nType )
{
    // Make sure we have a component to detach
    if( m_pComponentIndices[ nType ] != -1 )
    {
        CComponentManager::GetInstance()->RemoveComponent( nType, m_pComponentIndices[ nType ] );
        m_pComponentIndices[ nType ] = -1;
    }
}

//-----------------------------------------------------------------------------
//  Reset
//  Removes all components and resets the object
//-----------------------------------------------------------------------------
void CObject::Reset( void )
{
    for( uint i = 0; i < eNUMCOMPONENTS; ++i )
    {
        RemoveComponent( (eComponentType)i );
    }
}

//-----------------------------------------------------------------------------
//  Accessors/mutators
//-----------------------------------------------------------------------------
CMesh* CObject::GetMesh( void )
{
    return m_pMesh;
}

CMaterial* CObject::GetMaterial( void )
{
    return m_pMaterial;
}

void CObject::SetMesh( CMesh* pMesh )
{
    m_pMesh = pMesh;
}

void CObject::SetMaterial( CMaterial* pMaterial )
{
    m_pMaterial = pMaterial;
}

