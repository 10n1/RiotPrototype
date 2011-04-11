/*********************************************************\
File:           OGLGraphics.h
Purpose:        OpenGL graphics interface
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:42:44 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OGLGRAPHICS_H_
#define _OGLGRAPHICS_H_
#include "common.h"
#include "Graphics.h"
#include "SystemOpenGL.h"

namespace Riot
{

    class COGLDevice : public CGraphicsDevice
    {
        friend class System;
    public:
        // COGLDevice constructor
        COGLDevice();

        // COGLDevice destructor
        ~COGLDevice();
        /***************************************\
        | class methods                         |
        \***************************************/
        
        void SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha );
        void SetClearDepth( float fDepth );
        void Clear( void );
        void Present( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        SystemOpenGL::TOpenGLDevice m_pDevice;
    };

} // namespace Riot

#endif // #ifndef _OGLGRAPHICS_H_
