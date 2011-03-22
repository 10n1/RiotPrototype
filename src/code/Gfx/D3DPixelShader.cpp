/*********************************************************\
File:           D3DPixelShader.cpp
Author:         Kyle Weicht
Created:        3/20/2011
Modified:       3/21/2011 9:47:52 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DPixelShader.h"
#include "memory.h"
#include <D3D11.h>

// CD3DPixelShader constructor
CD3DPixelShader::CD3DPixelShader()
    : m_pShader( NULL )
    , m_pDeviceContext( NULL )
{
}

// CD3DPixelShader destructor
CD3DPixelShader::~CD3DPixelShader()
{
    SAFE_RELEASE( m_pShader );
}

//-----------------------------------------------------------------------------
//  ApplyShader
//  Applys the shader to the pipeline
//-----------------------------------------------------------------------------
void CD3DPixelShader::ApplyShader( void )
{
    m_pDeviceContext->PSSetShader( m_pShader, NULL, 0 );
}
