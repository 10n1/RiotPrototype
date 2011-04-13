/*********************************************************\
File:           SystemOpenGL.h
Purpose:        An abstraction layer for the more low-level
                platform-specific OpenGL details
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/12/2011 7:48:38 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SYSTEMOPENGL_H_
#define _SYSTEMOPENGL_H_
#include "common.h"

#ifdef OS_WINDOWS

//#define GL3_PROTOTYPES
#include <gl3/gl3.h>
#include <gl/wglext.h>

#elif defined( OS_OSX )
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

#if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_7)
#include <OpenGL/gl3.h>
#else
#include <OpenGL/GL.h>
#endif

#else
// linux
#endif


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
            handle  m_pGLContext;       // Type CGLContextObj  
            handle  m_pGLPixelFormat;   // Type CGLPixelFormatObj
#endif // #ifdef OS_WINDOWS
        };

#ifdef OS_WINDOWS
        // OpenGl functions
        extern PFNGLCLEARPROC           glClear;
        extern PFNGLCLEARCOLORPROC      glClearColor;
        extern PFNGLCLEARDEPTHPROC      glClearDepth;
        extern PFNGLDRAWBUFFERPROC      glDrawBuffer;
        extern PFNGLVIEWPORTPROC        glViewport;
        extern PFNGLFRONTFACEPROC       glFrontFace;

        // OpenGL extensions (OpenGL 1.2+)
        extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
#endif

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
        
#ifdef OS_OSX
        //-----------------------------------------------------------------------------
        //  SetView
        //  Sets the current view
        //-----------------------------------------------------------------------------
        void SetView( handle pView );
#endif

    } // namespace SystemOpenGL


} // namespace Riot


#endif // #ifndef _SYSTEMOPENGL_H_
