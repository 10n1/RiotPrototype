/*********************************************************\
File:           D3DMaterial.cpp
Author:         Kyle Weicht
Created:        3/22/2011
Modified:       3/22/2011 6:50:26 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DMaterial.h"
#include "memory.h"
#include <D3D11.h>

// CD3DMaterial constructor
CD3DMaterial::CD3DMaterial()
    : m_pDeviceContext( NULL )
    , m_pPixelShader( NULL )
{
}


// CD3DMaterial destructor
CD3DMaterial::~CD3DMaterial()
{
    SAFE_RELEASE( m_pPixelShader );
}

//-----------------------------------------------------------------------------
//  ApplyMaterial
//  Applies the material to the pipeline
//-----------------------------------------------------------------------------
void CD3DMaterial::ApplyMaterial( void )
{
    m_pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
}
