/*********************************************************\
File:           OGLGraphicsObjects.h
Purpose:        The OGL graphics objects
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 8:10:02 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OGLGRAPHICSOBJECTS_H_
#define _OGLGRAPHICSOBJECTS_H_
#include "common.h"
#include "GraphicsObjects.h"
#include "SystemOpenGL.h"

#if USE_OPENGL


#define BEGIN_DEFINE_GRAPHICS_OBJECT( Name ) \
class COGL##Name : public IGfx##Name             \
    {                                            \
        friend class COGLDevice;                 \
    public:                                      \
        COGL##Name();                            \
        ~COGL##Name();                           \
    private:

#define END_DEFINE_GRAPHICS_OBJECT  \
    }

namespace Riot
{    

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    class COGLRenderTarget : public IGfxRenderTarget
    {
        friend class COGLDevice;
    public:
        // COGLRenderTarget constructor
        COGLRenderTarget();

        // COGLRenderTarget destructor
        ~COGLRenderTarget();

    private:
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( Buffer )
        GLint   m_nBuffer;
    END_DEFINE_GRAPHICS_OBJECT;    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------    
    BEGIN_DEFINE_GRAPHICS_OBJECT( VertexShader )
        GLint   m_nShader;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( PixelShader )
        GLint   m_nShader;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot


#undef BEGIN_DEFINE_OGL_GRAPHICS_OBJECT
#undef END_DEFINE_OGL_GRAPHICS_OBJECT

#endif // #if USE_OPENGL


#endif // #ifndef _OGLGRAPHICSOBJECTS_H_
