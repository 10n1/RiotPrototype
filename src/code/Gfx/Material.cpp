/*********************************************************\
File:           Material.cpp
Author:         Kyle Weicht
Created:        3/21/2011
Modified:       3/21/2011 9:47:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Material.h"
#include "PixelShader.h"
#include "memory.h"

// CMaterial constructor
CMaterial::CMaterial()
    : m_pPixelShader( NULL )
{
}

// CMaterial destructor
CMaterial::~CMaterial()
{
    SAFE_RELEASE( m_pPixelShader );
}

//-----------------------------------------------------------------------------
//  SetPixelShader
//  Sets the materials pixel shader
//  TODO: Support multiple pixel shaders
//-----------------------------------------------------------------------------
void CMaterial::SetPixelShader( CPixelShader* pShader )
{
    m_pPixelShader = pShader;
}

//-----------------------------------------------------------------------------
//  ApplyMaterial
//  Applies the material to the pipeline
//-----------------------------------------------------------------------------
void CMaterial::ApplyMaterial( void )
{
    m_pPixelShader->ApplyShader();
}
