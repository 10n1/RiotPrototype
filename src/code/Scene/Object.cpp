/*********************************************************\
File:           Object.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/20/2011 1:26:19 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Object.h"
#include "memory.h"

// CObject constructor
CObject::CObject()
    : m_pMesh( NULL )
    , m_pMaterial( NULL )
{
}

// CObject destructor
CObject::~CObject()
{
}

//-----------------------------------------------------------------------------
//  Update
//  Updates the object
//  TODO: Pre- and Post- updates?
//-----------------------------------------------------------------------------
void CObject::Update( float fDeltaTime )
{
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

