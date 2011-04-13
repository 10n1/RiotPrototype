/*********************************************************\
File:           OGLGraphicsObjects.h
Purpose:        The OGL graphics objects
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 9:01:33 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OGLGRAPHICSOBJECTS_H_
#define _OGLGRAPHICSOBJECTS_H_
#include "common.h"
#include "GraphicsObjects.h"
#include "SystemOpenGL.h"

namespace Riot
{    

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    class COGLRenderTarget : public IRenderTarget
    {
        friend class COGLGraphics;
    public:
        // COGLRenderTarget constructor
        COGLRenderTarget();

        // COGLRenderTarget destructor
        ~COGLRenderTarget();

    private:
    };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



} // namespace Riot

#endif // #ifndef _OGLGRAPHICSOBJECTS_H_
