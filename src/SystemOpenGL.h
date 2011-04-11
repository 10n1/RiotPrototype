/*********************************************************\
File:           SystemOpenGL.h
Purpose:        An abstraction layer for the more low-level
                platform-specific OpenGL details
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:36:56 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SYSTEMOPENGL_H_
#define _SYSTEMOPENGL_H_
#include "common.h"

namespace Riot
{
    class CWindow;

    namespace SystemOpenGL
    {
        struct TOpenGLDevice
        {
#ifdef OS_WINDOWS
            handle  m_hDC;   // Type HDC
            handle  m_hGLRC; // Type HGLRC
            sint    m_nPixelFormat;
#elif defined (OS_OSX)
            handle  m_pView; // Type COSXView
#endif // #ifdef OS_WINDOWS
        };

        //-----------------------------------------------------------------------------
        //  LoadOpenGLExtensions
        //  Loads the platform specific OpenGL extentions (wgl, etc)
        //-----------------------------------------------------------------------------
        void LoadOpenGLExtensions( void );
        
        //-----------------------------------------------------------------------------
        //  CreateOpenGLDevice
        //  Creates the device
        //-----------------------------------------------------------------------------
        Result CreateOpenGLDevice( TOpenGLDevice* pDevice, CWindow* pWindow );

        //-----------------------------------------------------------------------------
        //  Present
        //  Flips the buffers
        //-----------------------------------------------------------------------------
        void Present( TOpenGLDevice* pDevice );

    } // namespace SystemOpenGL


} // namespace Riot


#endif // #ifndef _SYSTEMOPENGL_H_
