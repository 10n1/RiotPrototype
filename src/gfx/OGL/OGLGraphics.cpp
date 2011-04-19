/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/17/2011 11:40:15 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "OGLGraphics.h"
#include "OGLGraphicsObjects.h"
#include <fstream>

#if USE_OPENGL

namespace Riot
{    
    //-----------------------------------------------------------------------------
    //  Function declarations
    Result LoadShaderProgram( const wchar_t* szFilename, GLuint nShader );
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    GFX_FORMAT GFX_FORMAT_FLOAT3        = 0x1;
    GFX_FORMAT GFX_FORMAT_UINT16        = 0x2;
    GFX_FORMAT GFX_FORMAT_UINT32        = 0x3;

    const uint GFX_FORMAT_FLOAT3_SIZE   = sizeof( RVector3 );
    const uint GFX_FORMAT_UINT16_SIZE   = sizeof( uint16 );
    const uint GFX_FORMAT_UINT32_SIZE   = sizeof( uint32 );
    //-----------------------------------------------------------------------------
    GFX_SEMANTIC GFX_SEMANTIC_POSITION  = "vPosition";
    GFX_SEMANTIC GFX_SEMANTIC_NORMAL    = "vNormal";
    //-----------------------------------------------------------------------------
    GFX_PRIMITIVE_TYPE GFX_PRIMITIVE_TRIANGLELIST   = GL_TRIANGLE_STRIP;
    //-----------------------------------------------------------------------------

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
        GLenum nError = glGetError();
        
        // Switch the cull mode
        glFrontFace( GL_CW );
        nError = glGetError();
        glDepthRange( 0.0f, 1.0f );
        nError = glGetError();

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
        GLenum nError = glGetError();
    }
   
    //
    void COGLDevice::SetViewport( uint nWidth, uint nHeight )
    {
        glViewport( 0, 0, nWidth, nHeight );
        GLenum nError = glGetError();
    }

    void COGLDevice::SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha )
    {
        glClearColor( fRed, fGreen, fBlue, fAlpha );
        GLenum nError = glGetError();
    }

    void COGLDevice::SetClearDepth( float fDepth )
    {
        glClearDepth( fDepth );
        GLenum nError = glGetError();
    }

    void COGLDevice::Clear( void )
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );   
        GLenum nError = glGetError();
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
        COGLVertexShader* pNewShader = new COGLVertexShader;
        COGLVertexLayout* pNewLayout = new COGLVertexLayout;

        *pShader = pNewShader;
        *pLayout = pNewLayout;
    }
    IGfxPixelShader* COGLDevice::CreatePixelShader( const wchar_t* szFilename, const char* szEntryPoint )
    {
        COGLPixelShader* pShader = new COGLPixelShader;

        GLuint  nPixelShader;
        GLint   nTestVal;
        nPixelShader = glCreateShader( GL_FRAGMENT_SHADER );

        LoadShaderProgram( szFilename, nPixelShader );

        glCompileShader( nPixelShader );
        // See what errors there were
        glGetShaderiv( nPixelShader, GL_COMPILE_STATUS, &nTestVal );
        if( nTestVal == GL_FALSE )
        {
            char pErrorLog[1024] = { 0 };
            glGetShaderInfoLog( nPixelShader, 1024, NULL, pErrorLog );

            ASSERT( 0 );
        }

        return pShader;
    }
    //

    //
    IGfxBuffer* COGLDevice::CreateConstantBuffer( uint nSize, void* pInitialData )
    {
        COGLBuffer* pBuffer = new COGLBuffer;

        return pBuffer;
    }
    IGfxBuffer* COGLDevice::CreateVertexBuffer( uint nSize, void* pInitialData )
    {
        COGLBuffer* pBuffer = new COGLBuffer;

        return pBuffer;
    }

    IGfxBuffer* COGLDevice::CreateIndexBuffer( uint nSize, void* pInitialData )
    {
        COGLBuffer* pBuffer = new COGLBuffer;

        return pBuffer;
    }
    //


    //
    void COGLDevice::UpdateBuffer( IGfxBuffer* pBuffer, void* pData )
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

    void COGLDevice::SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
    }

    void COGLDevice::SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
    }
    //

    //
    void COGLDevice::DrawIndexedPrimitive( uint nIndexCount )
    {
    }
    //

    Result LoadShaderProgram( const wchar_t* szFilename, GLuint nShader )
    {
        FILE* pFile = _wfopen( szFilename, L"rb" );
        ASSERT( pFile );
        
        byte szBuffer[1024*16] = { 0 };

        fread( szBuffer, sizeof(byte), 1024*16, pFile );

        fclose( pFile );

        GLchar* pShaders[1];

        pShaders[0] = (GLchar*)szBuffer;
        
        glShaderSource( nShader, 1, (const GLchar**)pShaders, NULL );

        GLenum nError = glGetError();
        ASSERT( nError == GL_NO_ERROR );

        if( nError != GL_NO_ERROR )
            return rResultFailure;

        return rResultSuccess;
    }


} // namespace Riot

#endif // #if USE_OPENGL

