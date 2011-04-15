/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/14/2011 10:29:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "OGLGraphics.h"
#include "OGLGraphicsObjects.h"

#if USE_OPENGL

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
   
    //
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
    //

    //
    void COGLDevice::CreateVertexShaderAndLayout( 
            const wchar_t* szFilename, 
            const char* szEntryPoint, 
            InputElementLayout Layout[],
            uint nLayoutCount,
            IGfxVertexShader** pShader,
            IGfxVertexLayout** pLayout )
    {
    }
    IGfxPixelShader* COGLDevice::CreatePixelShader( const wchar_t* szFilename, const char* szEntryPoint )
    {
    }
    //

    //
    IGfxBuffer* COGLDevice::CreateConstantBuffer( uint nSize, void* pInitialData )
    {
        ASSERT( 0 );
        return NULL;
    }
    IGfxBuffer* COGLDevice::CreateVertexBuffer( uint nSize, void* pInitialData )
    {
    }

    IGfxBuffer* COGLDevice::CreateIndexBuffer( uint nSize, void* pInitialData )
    {
    }
    //

    //
    void COGLDevice::SetVertexLayout( IGfxVertexLayout* pLayout )
    {
    }
    
    void COGLDevice::SetVertexBuffer( IGfxBuffer* pBuffer, uint nStride )
    {
    }
    
    void COGLDevice::SetIndexBuffer( IGfxBuffer* pBuffer, uint nSize )
    {
    }
    
    void COGLDevice::SetPrimitiveType( GFX_PRIMITIVE_TYPE nType )
    {
    }
    
    void COGLDevice::SetVertexShader( IGfxVertexShader* pShader )
    {
    }
    
    void COGLDevice::SetPixelShader( IGfxPixelShader* pShader )
    {
    }
    
    //

    //
    void COGLDevice::DrawIndexedPrimitive( uint nIndexCount )
    {
    }
    //

} // namespace Riot

#endif // #if USE_OPENGL

