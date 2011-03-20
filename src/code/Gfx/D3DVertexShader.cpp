/*********************************************************\
File:           D3DVertexShader.cpp
Author:         Kyle Weicht
Created:        3/20/2011
Modified:       3/20/2011 12:37:21 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DVertexShader.h"
#include "memory.h"
#include <D3D11.h>

// CD3DVertexShader constructor
CD3DVertexShader::CD3DVertexShader()
    : m_pShader( NULL )
    , m_pDeviceContext( NULL )
{
}

// CD3DVertexShader destructor
CD3DVertexShader::~CD3DVertexShader()
{
    SAFE_RELEASE( m_pShader );
}

//-----------------------------------------------------------------------------
//  SetShader
//  Applys the shader to the pipeline
//-----------------------------------------------------------------------------
void CD3DVertexShader::SetShader( void )
{
    m_pDeviceContext->VSSetShader( m_pShader, NULL, 0 );
}
