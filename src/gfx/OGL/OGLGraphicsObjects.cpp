/*********************************************************\
File:           OGLGraphicsObjects.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/17/2011 10:40:25 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "OGLGraphicsObjects.h"
#if USE_OPENGL

namespace Riot
{    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    // COGLRenderTarget constructor
    COGLRenderTarget::COGLRenderTarget()
    {
    }

    // COGLRenderTarget destructor
    COGLRenderTarget::~COGLRenderTarget()
    {
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    COGLBuffer::COGLBuffer()
        : m_nBuffer( -1 )
    {
    }

    COGLBuffer::~COGLBuffer()
    {
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    COGLVertexShader::COGLVertexShader()
        : m_nShader( -1 )
    {
    }
    COGLVertexShader::~COGLVertexShader()
    {
    }    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    COGLPixelShader::COGLPixelShader()
        : m_nShader( -1 )
    {
    }
    COGLPixelShader::~COGLPixelShader()
    {
    }    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    COGLVertexLayout::COGLVertexLayout()
        : m_nLayout( -1 )
    {
    }
    COGLVertexLayout::~COGLVertexLayout()
    {
    }    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot

#endif // #if USE_OPENGL