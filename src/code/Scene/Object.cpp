/*********************************************************\
File:           Object.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/29/2011 11:13:41 PM
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
    m_vPosition = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    m_vOrientation = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );

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
//  Update
//  Updates the object
//  TODO: Pre- and Post- updates?
//-----------------------------------------------------------------------------
void CObject::Update( float fDeltaTime )
{
    // TODO: TEMP
    m_vPosition = m_vPosition + XMVectorSet( fDeltaTime * 0.1f, fDeltaTime * 0.1f, 0.0f, 0.0f );

    m_vOrientation = XMQuaternionMultiply( m_vOrientation, XMQuaternionRotationAxis( XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ), 0.1f * fDeltaTime ) );

    if( m_pMesh )
    {
        m_pMesh->m_vPosition = m_vPosition;
        m_pMesh->m_vOrientation = m_vOrientation;
    }
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


const XMVECTOR& CObject::GetPosition( void )
{
    return m_vPosition;
}

const XMVECTOR& CObject::GetOrientation( void )
{
    return m_vOrientation;
}

void CObject::SetPosition( const XMVECTOR& vPosition )
{
    m_vPosition = vPosition;
}

void CObject::SetOrientation( const XMVECTOR& vOrientation )
{
    m_vOrientation = vOrientation;
}
