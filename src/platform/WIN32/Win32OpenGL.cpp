/*********************************************************\
File:           Win32OpenGL.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/14/2011 8:10:49 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "platform/SystemOpenGL.h"
#include "Window.h"
#include <gl3/gl3.h>
#include <gl/wglext.h>

#if USE_OPENGL

namespace Riot
{
    namespace SystemOpenGL
    {
        //-----------------------------------------------------------------------------
        //  The OpenGL dll

        // The standard WGL functions
        typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTPROC)( HDC hdc );
        typedef PROC (WINAPI *PFNWGLGETPROCADDRESSPROC)( LPCSTR lpszProc );
        typedef BOOL (WINAPI *PFNWGLDELETECONTEXTPROC)( HGLRC hglrc );
        typedef BOOL (WINAPI *PFNWGLMAKECURRENTPROC)( HDC hdc, HGLRC hglrc );

        // Defined in OpenGL32.dll
        static PFNWGLCREATECONTEXTPROC              wglCreateContext            = NULL;
        static PFNWGLGETPROCADDRESSPROC             wglGetProcAddress           = NULL;
        static PFNWGLDELETECONTEXTPROC              wglDeleteContext            = NULL;
        static PFNWGLMAKECURRENTPROC                wglMakeCurrent              = NULL;

        // Driver extensions
        static PFNWGLCREATECONTEXTATTRIBSARBPROC    wglCreateContextAttribsARB  = NULL;
        static PFNWGLCHOOSEPIXELFORMATARBPROC       wglChoosePixelFormatARB     = NULL;

        // OpenGL32.dll
        static HMODULE pOpenGL32 = NULL;

        //-----------------------------------------------------------------------------
        //  WGL extensions
        //-----------------------------------------------------------------------------
#define GET_OPENGL_EXTENSION( type, ext ) ext = (type)wglGetProcAddress( #ext ); ASSERT( ext )

#define LOAD_FROM_DLL( type, ext ) ext = (type)GetProcAddress( pOpenGL32, #ext ); ASSERT( ext )

        // OpenGl functions
        PFNGLCLEARPROC       glClear      = NULL;
        PFNGLCLEARCOLORPROC  glClearColor = NULL;
        PFNGLCLEARDEPTHPROC  glClearDepth = NULL;
        PFNGLDRAWBUFFERPROC  glDrawBuffer = NULL;
        PFNGLVIEWPORTPROC    glViewport   = NULL;
        PFNGLFRONTFACEPROC   glFrontFace  = NULL;
        PFNGLDEPTHRANGEPROC  glDepthRange = NULL;

        // OpenGL extensions (OpenGL 1.2+)
        PFNGLBINDFRAMEBUFFERPROC             glBindFramebuffer = NULL;

        //-----------------------------------------------------------------------------
        //  LoadOpenGLExtensions
        //  Loads the platform specific OpenGL extentions (wgl, etc)
        //-----------------------------------------------------------------------------
        void LoadOpenGLExtensions( void )
        {
            // Load the basic OpenGL calls from the DLL
            LOAD_FROM_DLL( PFNGLCLEARPROC     , glClear      );
            LOAD_FROM_DLL( PFNGLCLEARCOLORPROC, glClearColor );
            LOAD_FROM_DLL( PFNGLCLEARDEPTHPROC, glClearDepth );
            LOAD_FROM_DLL( PFNGLDRAWBUFFERPROC, glDrawBuffer );
            LOAD_FROM_DLL( PFNGLVIEWPORTPROC  , glViewport   );
            LOAD_FROM_DLL( PFNGLFRONTFACEPROC , glFrontFace  );
            LOAD_FROM_DLL( PFNGLDEPTHRANGEPROC, glDepthRange );

            // Load the rest from the driver
            GET_OPENGL_EXTENSION( PFNGLBINDFRAMEBUFFERPROC         , glBindFramebuffer );
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

            HDC     hDC = NULL;
            HGLRC   hGLRC = NULL;
            HWND    hWnd = static_cast<HWND>( pWindow->GetWindow() );
            sint    nPixelFormat = 0;

            //////////////////////////////////////////
            //  First load the OpenGL32 dll
            pOpenGL32 = LoadLibrary( L"OpenGL32.dll" );
            ASSERT( pOpenGL32 );

            // Load the standard functions
            LOAD_FROM_DLL( PFNWGLCREATECONTEXTPROC, wglCreateContext );
            LOAD_FROM_DLL( PFNWGLGETPROCADDRESSPROC, wglGetProcAddress );
            LOAD_FROM_DLL( PFNWGLDELETECONTEXTPROC, wglDeleteContext );
            LOAD_FROM_DLL( PFNWGLMAKECURRENTPROC, wglMakeCurrent );

            //////////////////////////////////////////
            // Create a rendering context
            hDC = GetDC( hWnd );      
            PIXELFORMATDESCRIPTOR pfd;
            nPixelFormat = 1;

            bool bSuccess = SetPixelFormat( hDC, nPixelFormat, &pfd );
            hGLRC = wglCreateContext( hDC );
            wglMakeCurrent( hDC, hGLRC );
            
            // Load the creation functions we need
            GET_OPENGL_EXTENSION( PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB );
            GET_OPENGL_EXTENSION( PFNWGLCHOOSEPIXELFORMATARBPROC   , wglChoosePixelFormatARB    );

            // Release and start over
            wglMakeCurrent( NULL, NULL );
            wglDeleteContext( hGLRC );

            //////////////////////////////////////////
            // Define attributes required
            uint nPixCount = 0;
            int pPixAttribs[] = 
            {
                WGL_SUPPORT_OPENGL_ARB, 1,
                WGL_DRAW_TO_WINDOW_ARB, 1,
                WGL_ACCELERATION_ARB,   1,
                WGL_COLOR_BITS_ARB,     32,
                WGL_DEPTH_BITS_ARB,     24,
                WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
                WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                WGL_SAMPLES_ARB,        8,
                WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
                0
            };

            // Chose most appropriate format
            wglChoosePixelFormatARB( hDC, &pPixAttribs[0], NULL, 1, &nPixelFormat, &nPixCount );

            if( nPixelFormat == -1 )
            {
                // TODO: Handle error
                ASSERT( 0 );
            }

            // Set pixel format
            SetPixelFormat( hDC, nPixelFormat, &pfd );

            //////////////////////////////////////////
            // Create OpenGL context
            GLint pCreationAttributes[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                0,
            };
            hGLRC = wglCreateContextAttribsARB( hDC, 0, pCreationAttributes );

            if( hGLRC == NULL )
            {
                // TODO: handle error
                ASSERT( 0 );
            }

            wglMakeCurrent( hDC, hGLRC );
            
            // Load functions
            LoadOpenGLExtensions();

            //////////////////////////////////////////
            // Pass it back to the device
            pDevice->m_hDC          = hDC;
            pDevice->m_hGLRC        = hGLRC;
            pDevice->m_nPixelFormat = nPixelFormat;

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

#endif // #if USE_OPENGL
