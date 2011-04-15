/*********************************************************\
File:           D3DGraphicsObjects.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 8:07:06 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DGraphicsObjects.h"

#if USE_DIRECTX

namespace Riot
{    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    // CD3DRenderTarget constructor
    CD3DRenderTarget::CD3DRenderTarget()
        : m_pRenderTargetView( NULL )
    {
    }

    // CD3DRenderTarget destructor
    CD3DRenderTarget::~CD3DRenderTarget()
    {
        SAFE_RELEASE( m_pRenderTargetView );
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    CD3DBuffer::CD3DBuffer()
        : m_pBuffer( NULL )
    {
    }
    CD3DBuffer::~CD3DBuffer()
    {
        SAFE_RELEASE( m_pBuffer );
    }    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    CD3DVertexShader::CD3DVertexShader()
        : m_pShader( NULL )
    {
    }
    CD3DVertexShader::~CD3DVertexShader()
    {
        SAFE_RELEASE( m_pShader );
    }    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    CD3DPixelShader::CD3DPixelShader()
        : m_pShader( NULL )
    {
    }
    CD3DPixelShader::~CD3DPixelShader()
    {
        SAFE_RELEASE( m_pShader );
    }    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot

#endif //#if USE_DIRECTX
