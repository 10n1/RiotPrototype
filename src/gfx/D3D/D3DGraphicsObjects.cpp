/*********************************************************\
File:           D3DGraphicsObjects.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 8:54:13 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DGraphicsObjects.h"

#if USE_DIRECTX

#define DECLARE_SIMPLE_GRAPHICS_OBJECT( Name, Object )   \
    CD3D##Name::CD3D##Name()                    \
        : Object( NULL )                        \
    {                                           \
    }                                           \
    CD3D##Name::~CD3D##Name()                   \
    {                                           \
        SAFE_RELEASE( Object );                 \
    }    


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
    DECLARE_SIMPLE_GRAPHICS_OBJECT( Buffer, m_pBuffer );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    DECLARE_SIMPLE_GRAPHICS_OBJECT( VertexShader, m_pShader );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    DECLARE_SIMPLE_GRAPHICS_OBJECT( PixelShader, m_pShader );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    DECLARE_SIMPLE_GRAPHICS_OBJECT( VertexLayout, m_pLayout );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot

#endif //#if USE_DIRECTX
