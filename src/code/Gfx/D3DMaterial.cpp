/*********************************************************\
File:           D3DMaterial.cpp
Author:         Kyle Weicht
Created:        3/22/2011
Modified:       4/3/2011 8:27:05 PM
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
//  ProcessObject
//  Applies the material to the pipeline
//-----------------------------------------------------------------------------
void CD3DMaterial::ProcessObject( void )
{
    m_pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
}
