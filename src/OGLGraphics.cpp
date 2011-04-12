/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:42:44 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "OGLGraphics.h"

#ifdef OS_WINDOWS
#define GL3_PROTOTYPES
#include <gl3/gl3.h>
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

    // COGLDevice constructor
    COGLDevice::COGLDevice()
    {
        Memset( &m_pDevice, 0, sizeof( m_pDevice ) );
    }

    // COGLDevice destructor
    COGLDevice::~COGLDevice()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/

    void COGLDevice::SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha )
    {
        glClearColor( fRed, fGreen, fBlue, fAlpha );
    }

    void COGLDevice::SetClearDepth( float fDepth )
    {
        glClearDepth( fDepth );
    }

    void COGLDevice::Clear( void )
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
    }

    void COGLDevice::Present( void )
    {
        SystemOpenGL::Present( &m_pDevice );
    }

} // namespace Riot
