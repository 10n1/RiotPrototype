/*********************************************************\
File:           OGLGraphicsObjects.h
Purpose:        The OGL graphics objects
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 9:46:26 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OGLGRAPHICSOBJECTS_H_
#define _OGLGRAPHICSOBJECTS_H_
#include "common.h"
#include "GraphicsObjects.h"
#include "SystemOpenGL.h"


#define BEGIN_DEFINE_OGL_GRAPHICS_OBJECT( Name ) \
class COGL##Name : public IGfx##Name             \
    {                                            \
        friend class COGLDevice;                 \
    public:                                      \
        COGL##Name();                            \
        ~COGL##Name();                           \
    private:

#define END_DEFINE_OGL_GRAPHICS_OBJECT  \
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

    BEGIN_DEFINE_OGL_GRAPHICS_OBJECT( Buffer )
        GLint   m_nBuffer;
    END_DEFINE_OGL_GRAPHICS_OBJECT;
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot


#undef BEGIN_DEFINE_OGL_GRAPHICS_OBJECT
#undef END_DEFINE_OGL_GRAPHICS_OBJECT

#endif // #ifndef _OGLGRAPHICSOBJECTS_H_
