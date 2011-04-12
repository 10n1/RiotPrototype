/*********************************************************\
File:           Win32OpenGL.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/11/2011 11:18:03 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "platform/SystemOpenGL.h"
#include "Window.h"
#include <gl3/gl3.h>
#include <gl/wglext.h>

namespace Riot
{
    namespace SystemOpenGL
    {
        //-----------------------------------------------------------------------------
        //  WGL extensions
        //-----------------------------------------------------------------------------
#define GET_OPENGL_EXTENSION( type, ext ) ext = (type)wglGetProcAddress( #ext )
        PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer           = NULL;
        PFNWGLCREATECONTEXTATTRIBSARBPROC    wglCreateContextAttribsARB  = NULL;
        PFNWGLCHOOSEPIXELFORMATARBPROC       wglChoosePixelFormatARB     = NULL;

        //-----------------------------------------------------------------------------
        //  LoadOpenGLExtensions
        //  Loads the platform specific OpenGL extentions (wgl, etc)
        //-----------------------------------------------------------------------------
        void LoadOpenGLExtensions( void )
        {
            //GET_OPENGL_EXTENSION( PFNGLCLEARPROC                   , glClear                    );
            //GET_OPENGL_EXTENSION( PFNGLCLEARCOLORPROC              , glClearColor               );
            //GET_OPENGL_EXTENSION( PFNGLCLEARDEPTHPROC              , glClearDepth               );
            //GET_OPENGL_EXTENSION( PFNGLDRAWBUFFERPROC              , glDrawBuffer               );
            //GET_OPENGL_EXTENSION( PFNGLVIEWPORTPROC                , glViewport                 );
            GET_OPENGL_EXTENSION( PFNGLBINDFRAMEBUFFERPROC         , glBindFramebuffer          );
            GET_OPENGL_EXTENSION( PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB );
            GET_OPENGL_EXTENSION( PFNWGLCHOOSEPIXELFORMATARBPROC   , wglChoosePixelFormatARB    );
        }

        //-----------------------------------------------------------------------------
        //  CreateOpenGLDevice
        //  Creates the device
        //-----------------------------------------------------------------------------
        Result CreateOpenGLDevice( TOpenGLDevice* pDevice, CWindow* pWindow )
        {
            Result nResult = rResultSuccess;

            HWND hWnd = static_cast<HWND>( pWindow->GetWindow() );
            //////////////////////////////////////////
            // Create a rendering context
            pDevice->m_hDC = GetDC( hWnd );      
            PIXELFORMATDESCRIPTOR pfd;
            pDevice->m_nPixelFormat = 1;
            bool bSuccess = SetPixelFormat( (HDC)pDevice->m_hDC, pDevice->m_nPixelFormat, &pfd );
            pDevice->m_hGLRC = wglCreateContext( (HDC)pDevice->m_hDC );
            wglMakeCurrent( (HDC)pDevice->m_hDC, (HGLRC)pDevice->m_hGLRC );
            
            // Load the creation functions we need
            GET_OPENGL_EXTENSION( PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB );
            GET_OPENGL_EXTENSION( PFNWGLCHOOSEPIXELFORMATARBPROC   , wglChoosePixelFormatARB    );

            // Release and start over
            wglMakeCurrent( NULL, NULL );
            wglDeleteContext( (HGLRC)pDevice->m_hGLRC );

            //////////////////////////////////////////
            // Define attributes required
            uint nPixCount = 0;
            int pPixAttribs[] = 
            {
                WGL_SUPPORT_OPENGL_ARB, 1,
                WGL_DRAW_TO_WINDOW_ARB, 1,
                WGL_ACCELERATION_ARB,   1,
                WGL_COLOR_BITS_ARB,     24,
                WGL_DEPTH_BITS_ARB,     16,
                WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
                WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                WGL_SAMPLES_ARB,        8,
                WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
                0
            };

            // Chose most appropriate format
            wglChoosePixelFormatARB( (HDC)pDevice->m_hDC, &pPixAttribs[0], NULL, 1, &pDevice->m_nPixelFormat, &nPixCount );

            if( pDevice->m_nPixelFormat == -1 )
            {
                // TODO: Handle error
                ASSERT( 0 );
            }

            // Set pixel format
            SetPixelFormat( (HDC)pDevice->m_hDC, pDevice->m_nPixelFormat, &pfd );

            //////////////////////////////////////////
            // Create OpenGL context
            GLint pCreationAttributes[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                0,
            };
            pDevice->m_hGLRC = wglCreateContextAttribsARB( (HDC)pDevice->m_hDC, 0, pCreationAttributes );

            if( pDevice->m_hGLRC == NULL )
            {
                // TODO: handle error
                ASSERT( 0 );
            }

            wglMakeCurrent( (HDC)pDevice->m_hDC, (HGLRC)pDevice->m_hGLRC );

            
            // Load functions
            LoadOpenGLExtensions();

            return nResult;
        }
        
        //-----------------------------------------------------------------------------
        //  Present
        //  Flips the buffers
        //-----------------------------------------------------------------------------
        void Present( TOpenGLDevice* pDevice )
        {
            SwapBuffers( (HDC)pDevice->m_hDC );
        }

    } // namespace SystemOpenGL


} // namespace Riot
