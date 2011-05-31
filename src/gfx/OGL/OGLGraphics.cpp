/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/21/2011 2:48:12 PM
Modified by:    Kyle Weicht
 \*********************************************************/
#include <fstream>
#include "OGLGraphics.h"
#include "OGLGraphicsObjects.h"

#if USE_OPENGL

namespace Riot
{    
    //-----------------------------------------------------------------------------
    //  Function declarations
    Result LoadShaderProgram( const char* szFilename, GLuint nShader );
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    GFX_FORMAT GFX_FORMAT_FLOAT4        = 0x5;
    GFX_FORMAT GFX_FORMAT_FLOAT3        = 0x1;
    GFX_FORMAT GFX_FORMAT_UINT16        = 0x2;
    GFX_FORMAT GFX_FORMAT_UINT32        = 0x3;
    GFX_FORMAT GFX_FORMAT_FLOAT2        = 0x4;
    GFX_FORMAT GFX_FORMAT_BYTE4         = 0x5;
    //-----------------------------------------------------------------------------
    GFX_SEMANTIC GFX_SEMANTIC_POSITION  = "vPosition";
    GFX_SEMANTIC GFX_SEMANTIC_NORMAL    = "vNormal";
    GFX_SEMANTIC GFX_SEMANTIC_TEXCOORD  = "vTexCoord";
    GFX_SEMANTIC GFX_SEMANTIC_COLOR     = "vColor";
    GFX_SEMANTIC GFX_SEMANTIC_TRANSFORM = "Transform";
    //-----------------------------------------------------------------------------
    GFX_PRIMITIVE_TYPE GFX_PRIMITIVE_TRIANGLELIST   = GL_TRIANGLE_STRIP;;
    GFX_PRIMITIVE_TYPE GFX_PRIMITIVE_LINELIST       = GL_LINE_STRIP;
    //-----------------------------------------------------------------------------
    GFX_TEXTURE_SAMPLE GFX_TEXTURE_SAMPLE_NEAREST   = 0x1;
    GFX_TEXTURE_SAMPLE GFX_TEXTURE_SAMPLE_LINEAR    = 0x2;
    //-----------------------------------------------------------------------------
    GFX_FILL_MODE    GFX_FILL_SOLID     = GL_FILL;
    GFX_FILL_MODE    GFX_FILL_WIREFRAME = GL_LINE;
    //-----------------------------------------------------------------------------
    GFX_BUFFER_USAGE    GFX_BUFFER_USAGE_DEFAULT    = GL_STATIC_DRAW;
    GFX_BUFFER_USAGE    GFX_BUFFER_USAGE_DYNAMIC    = GL_DYNAMIC_DRAW;
    GFX_BUFFER_USAGE    GFX_BUFFER_USAGE_IMMUTABLE  = GL_STATIC_DRAW; // ??
    //-----------------------------------------------------------------------------
    GFX_INPUT_DATA  GFX_INPUT_DATA_PER_VERTEX   = 0x0;
    GFX_INPUT_DATA  GFX_INPUT_DATA_PER_INSTANCE = 0x1;
    //-----------------------------------------------------------------------------

    using namespace SystemOpenGL;

    // COGLDevice constructor
    COGLDevice::COGLDevice()
        : m_nPrimitiveType( GFX_PRIMITIVE_TRIANGLELIST )
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
    void COGLDevice::ClearRenderTarget( IGfxRenderTarget* pRT )
    {
        
    }

    void COGLDevice::Present( void )
    {
        SystemOpenGL::Present( &m_pDevice );
    }
    //
    
    
    //
    void COGLDevice::SetFillMode( GFX_FILL_MODE nFill )
    {
        glPolygonMode( GL_FRONT_AND_BACK, nFill );
    }
    // NOTE: These flags need review to make sure they are correct
    void COGLDevice::SetDepthTest( bool bTest, bool bWrite )
    {
        if( !bTest && !bWrite )
            glDisable( GL_DEPTH_TEST );
        else
            glEnable( GL_DEPTH_TEST );

        glDepthMask( (!bWrite) ? GL_FALSE : GL_TRUE );
        glDepthFunc( GL_LESS );
        glDepthRange( 0.0f, 1.0f );

    }
    void COGLDevice::SetColorWrite( bool bEnable )
    {
        if( bEnable )
        {
            glEnable( GL_BLEND );
            glBlendEquation( GL_FUNC_ADD );
            glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        }
        else
        {
            glDisable( GL_BLEND );
        }
    }
    //

    //
    void COGLDevice::CreateVertexShaderAndLayout( 
            const char* szFilename, 
            const char* szEntryPoint, 
            InputElementLayout Layout[],
            uint nLayoutCount,
            IGfxVertexShader** pShader,
            IGfxVertexLayout** pLayout )
    {
        COGLVertexShader* pNewShader = new COGLVertexShader;
        COGLVertexLayout* pNewLayout = new COGLVertexLayout;

        pNewShader->m_nShader = glCreateShader( GL_VERTEX_SHADER );

        LoadShaderProgram( szFilename, pNewShader->m_nShader );

        glCompileShader( pNewShader->m_nShader );

        GLint nTestVal;
        glGetShaderiv( pNewShader->m_nShader, GL_COMPILE_STATUS, &nTestVal );
        if( nTestVal == GL_FALSE )
        {
            char pErrorLog[1024] = { 0 };
            glGetShaderInfoLog( pNewShader->m_nShader, 1024, NULL, pErrorLog );

            ASSERT( 0 );
        }

        *pShader = pNewShader;
        
        if( pLayout != NULL )
            *pLayout = pNewLayout;
    }
    IGfxPixelShader* COGLDevice::CreatePixelShader( const char* szFilename, const char* szEntryPoint )
    {
        COGLPixelShader* pShader = new COGLPixelShader;
        
        GLint   nTestVal;
        pShader->m_nShader = glCreateShader( GL_FRAGMENT_SHADER );

        LoadShaderProgram( szFilename, pShader->m_nShader );

        glCompileShader( pShader->m_nShader );
        // See what errors there were
        glGetShaderiv( pShader->m_nShader, GL_COMPILE_STATUS, &nTestVal );
        if( nTestVal == GL_FALSE )
        {
            char pErrorLog[1024] = { 0 };
            glGetShaderInfoLog( pShader->m_nShader, 1024, NULL, pErrorLog );

            ASSERT( 0 );
        }

        return pShader;
    }
    IGfxRenderTarget* COGLDevice::CreateRenderTarget( GFX_FORMAT nFormat, uint nWidth, uint nHeight )
    {
        COGLRenderTarget* pRT = new COGLRenderTarget;
        
        return pRT;
    }
    IGfxShaderProgram* COGLDevice::CreateShaderProgram( IGfxVertexShader* pVertexShader, IGfxPixelShader* pPixelShader )
    {
        COGLShaderProgram* pShaderProgram = new COGLShaderProgram;
        GLuint program = glCreateProgram();

        glAttachShader( pShaderProgram->m_nProgram, ( (COGLVertexShader*)pVertexShader )->m_nShader );
        glAttachShader( pShaderProgram->m_nProgram, ( (COGLPixelShader*)pPixelShader )->m_nShader );

        glLinkProgram( pShaderProgram->m_nProgram );

        GLint status;
        glGetProgramiv( pShaderProgram->m_nProgram, GL_LINK_STATUS, &status );
        if (status == GL_FALSE)
        {
            char pErrorLog[1024] = { 0 };
            glGetProgramInfoLog( pShaderProgram->m_nProgram, 1024, NULL, pErrorLog );

            ASSERT( 0 );
        }

        return pShaderProgram;
    }
    //
    
    //
    IGfxTexture2D* COGLDevice::LoadTexture( const char* szFilename )
    {
        COGLTexture2D* pTexture = new COGLTexture2D;

        glGenTextures( 1, (GLuint*)&pTexture->m_nTexture );
        // TODO: read in the texture file contents

        GLbyte* pTextureBits;
        sint nTexWidth;
        sint nTexHeight;
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, nTexWidth, nTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTextureBits );
        
        return pTexture;
    }
    IGfxSamplerState* COGLDevice::CreateSamplerState( GFX_TEXTURE_SAMPLE nType )
    {
        COGLSamplerState* pState = new COGLSamplerState;
        
        return pState;
    }
    //

    //
    IGfxBuffer* COGLDevice::CreateConstantBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage )
    {
        COGLBuffer* pBuffer = new COGLBuffer;

        return pBuffer;
    }
    IGfxBuffer* COGLDevice::CreateVertexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage )
    {
        COGLBuffer* pBuffer = new COGLBuffer;

        glGenBuffers( 1, (GLuint*)&pBuffer->m_nBuffer );
        glBufferData( GL_ARRAY_BUFFER, nSize, pInitialData, nUsage );

        return pBuffer;
    }

    IGfxBuffer* COGLDevice::CreateIndexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage )
    {
        COGLBuffer* pBuffer = new COGLBuffer;

        glGenBuffers( 1, (GLuint*)&pBuffer->m_nBuffer );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, nSize, pInitialData, nUsage );

        return pBuffer;
    }
    //


    //
    void COGLDevice::UpdateBuffer( IGfxBuffer* pBuffer, void* pData )
    {
    }
    void COGLDevice::UpdateBuffer( IGfxBuffer* pBuffer, void* pData, uint nSize )
    {
        
    }
    //

    //
    void COGLDevice::SetVertexLayout( IGfxVertexLayout* pLayout )
    {
    }

    void COGLDevice::SetVertexBuffer( uint nIndex, IGfxBuffer* pBuffer, uint nStride )
    {
        glBindBuffer( GL_ARRAY_BUFFER, ( (COGLBuffer*)pBuffer )->m_nBuffer );
    }
    
    void COGLDevice::SetIndexBuffer( IGfxBuffer* pBuffer, uint nSize )
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ( (COGLBuffer*)pBuffer )->m_nBuffer );
    }
    
    void COGLDevice::SetPrimitiveType( GFX_PRIMITIVE_TYPE nType )
    {
        m_nPrimitiveType = nType;
    }
    
    void COGLDevice::SetVertexShader( IGfxVertexShader* pShader )
    {
        m_pActiveVertexShader = pShader;
    }

    void COGLDevice::SetPixelShader( IGfxPixelShader* pShader )
    {
        m_pActivePixelShader = pShader;
    }

    void COGLDevice::SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
    }

    void COGLDevice::SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
    }
    void COGLDevice::SetPSSamplerState( IGfxSamplerState* pState )
    {
        
    }
    void COGLDevice::SetPSTexture( uint nIndex, IGfxTexture2D* pTexture )
    {
        
    }
    void COGLDevice::SetPSRenderTarget( uint nIndex, IGfxRenderTarget* pRenderTarget )
    {
        
    }
    void COGLDevice::SetRenderTarget( IGfxRenderTarget* pRenderTarget )
    {
        
    }
    void COGLDevice::SetNullRenderTarget( void )
    {
        
    }
    //

    //
    void COGLDevice::Draw( uint nVertexCount )
    {        
        glDrawArrays( m_nPrimitiveType, 0, nVertexCount );
    }
    void COGLDevice::DrawIndexedPrimitive( uint nIndexCount )
    {
        // NOTE: probably not a good idea ...
        glDrawArraysInstanced( m_nPrimitiveType, 0, nIndexCount, 1 );
    }
    void COGLDevice::DrawIndexedPrimitiveInstanced( uint nIndexCount, uint nInstanceCount )
    {
        glDrawArraysInstanced( m_nPrimitiveType, 0, nIndexCount, nInstanceCount );
    }
    void COGLDevice::DrawPrimitive( uint nVertexCount )
    {
        glDrawArrays( m_nPrimitiveType, 0, nVertexCount );
    }
    //

    Result LoadShaderProgram( const char* szFilename, GLuint nShader )
    {
        FILE* pFile = fopen( szFilename, "rb" );
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

