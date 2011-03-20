/*********************************************************\
File:           Object.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 7:56:45 PM
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
