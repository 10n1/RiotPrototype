/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/12/2011 8:49:17 PM
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
    Result COGLDevice::Initialize( CWindow* pWindow )
    {
        // Create the system specific OpenGL device
        SystemOpenGL::CreateOpenGLDevice( &m_pDevice, pWindow );
        
        // Switch the cull mode
        glFrontFace( GL_CW );
        // Make sure we're drawing to the back buffer
        glDrawBuffer( GL_BACK );

        glDepthRange( 0.0f, 1.0f );

        return rResultSuccess;
    }

    //-----------------------------------------------------------------------------
    //  Resize
    //  Resizes the graphics device (along with the back buffer )
    //-----------------------------------------------------------------------------
    void COGLDevice::Resize( uint nWidth, uint nHeight )
    {
        SetViewport( nWidth, nHeight );
    }

    //-----------------------------------------------------------------------------
    //  Hardware methods
    //----------------------------------------------------------------------------- 
    void COGLDevice::SetDefaultRenderDepthTarget( void )
    {
        glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
    }
   
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
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
    }

    void COGLDevice::Present( void )
    {
        SystemOpenGL::Present( &m_pDevice );
    }

} // namespace Riot
