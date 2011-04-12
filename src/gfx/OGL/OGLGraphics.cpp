/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/11/2011 11:06:20 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "OGLGraphics.h"

namespace Riot
{

    using namespace SystemOpenGL;

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
    //-----------------------------------------------------------------------------
    //  Initialize
    //  Performs any API specific initialization tasks (wind order, etc)
    //-----------------------------------------------------------------------------
    void COGLDevice::Initialize( void )
    {
        glFrontFace( GL_CW );
    }

    //-----------------------------------------------------------------------------
    //  Hardware methods
    //-----------------------------------------------------------------------------    
    void COGLDevice::SetViewport( uint nWidth, uint nHeight )
    {
        glViewport( 0, 0, nWidth, nHeight );
    }

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
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
        glDrawBuffer( GL_BACK );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
    }

    void COGLDevice::Present( void )
    {
        SystemOpenGL::Present( &m_pDevice );
    }

} // namespace Riot
