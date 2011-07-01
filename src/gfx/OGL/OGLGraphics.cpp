/*********************************************************\
File:           OGLGraphics.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/21/2011 2:48:12 PM
Modified by:    Kyle Weicht
 \*********************************************************/
#include <string.h>
#include <fstream>
#include "OGLGraphics.h"
#include "OGLGraphicsObjects.h"
#include "File.h"

#if USE_OPENGL

namespace Riot
{    
    //-----------------------------------------------------------------------------
    //  Function declarations
    Result LoadShaderProgram( const char* szFilename, GLuint nShader );
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    GFX_FORMAT GFX_FORMAT_FLOAT4        = 0x4;
    GFX_FORMAT GFX_FORMAT_FLOAT3        = 0x3;
    GFX_FORMAT GFX_FORMAT_FLOAT2        = 0x2;
    GFX_FORMAT GFX_FORMAT_UINT16        = 0x2;
    GFX_FORMAT GFX_FORMAT_UINT32        = 0x3;
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
    
    typedef struct _GfxTextureSampler
    {
    } GfxTextureSampler;
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
        , m_bCreateShaderProgram( true )
        , m_pActiveVertexShader( NULL )
        , m_pActivePixelShader( NULL )
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
        glFrontFace( GL_CCW );
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
        // TODO: clear specific render target
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
        glDepthFunc( GL_LEQUAL );
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

        // 1. Create the vertex shader
        pNewShader->m_nShader = glCreateShader( GL_VERTEX_SHADER );
        
        char szFormattedName[256] = { 0 };
        sprintf( szFormattedName, "%s.glsl", szFilename );
        Result rLoadShader = LoadShaderProgram( szFormattedName, pNewShader->m_nShader );

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
        
        // 2. Create the vertex layout...
        //    Generate an array buffer object to store the vertex attributes
        m_pInputLayout = Layout;
        m_nLayoutCount = nLayoutCount;
        glGenVertexArraysAPPLE( 1, (GLuint*)&pNewLayout->m_nLayout );
        glBindVertexArrayAPPLE( pNewLayout->m_nLayout );
        for ( uint i = 0; i < nLayoutCount; ++i ) 
        {
            glVertexAttribPointer( Layout[i].nIndex, Layout[i].nFormat, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0 );
        }
        glBindVertexArrayAPPLE( 0 );
        
        if( pLayout != NULL )
            *pLayout = pNewLayout;
    }
    IGfxPixelShader* COGLDevice::CreatePixelShader( const char* szFilename, const char* szEntryPoint )
    {
        COGLPixelShader* pShader = new COGLPixelShader;
        
        GLint   nTestVal;
        pShader->m_nShader = glCreateShader( GL_FRAGMENT_SHADER );
                
        GLenum nError = glGetError();
        
        switch( nError )
        {
            case GL_INVALID_ENUM:
                break;
            case GL_INVALID_OPERATION:
                break;
            default:
                break;
        }

        char szFormattedName[256] = { 0 };
        sprintf( szFormattedName, "%s.glsl", szFilename );
        LoadShaderProgram( szFormattedName, pShader->m_nShader );

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
        
        glGenRenderbuffers( 1, (GLuint*)&pRT->m_nRT );
        glRenderbufferStorage( GL_RENDERBUFFER, nFormat, nWidth, nHeight );
        
        return pRT;
    }
    IGfxShaderProgram* COGLDevice::CreateShaderProgram( IGfxVertexShader* pVertexShader, IGfxPixelShader* pPixelShader )
    {
        if( !m_bCreateShaderProgram )
            return NULL;  // hmmmm...
        
        COGLShaderProgram* pShaderProgram = new COGLShaderProgram;
        pShaderProgram->m_nProgram = glCreateProgram();

        glAttachShader( pShaderProgram->m_nProgram, ( (COGLVertexShader*)pVertexShader )->m_nShader );
        glAttachShader( pShaderProgram->m_nProgram, ( (COGLPixelShader*)pPixelShader )->m_nShader );
        
        // set attrib locations
        for( uint i = 0; i < m_nLayoutCount; ++i )
        {
            glBindAttribLocation( pShaderProgram->m_nProgram, m_pInputLayout[i].nIndex, m_pInputLayout[i].szSemanticName );
        }

        glLinkProgram( pShaderProgram->m_nProgram );

        GLint status;
        glGetProgramiv( pShaderProgram->m_nProgram, GL_LINK_STATUS, &status );
        if (status == GL_FALSE)
        {
            char pErrorLog[1024] = { 0 };
            glGetProgramInfoLog( pShaderProgram->m_nProgram, 1024, NULL, pErrorLog );

            ASSERT( 0 );
        }
        
        m_bCreateShaderProgram = false;

        return pShaderProgram;
    }
    //
    
    //
    IGfxTexture2D* COGLDevice::LoadTexture( const char* szFilename )
    {
        COGLTexture2D* pTexture = new COGLTexture2D;
        
        glGenTextures( 1, (GLuint*)&pTexture->m_nTexture );
        // TODO: read in the texture file contents
        CFile fTextureFile;
        fTextureFile.LoadFile( szFilename, "rb" );
        GLbyte* pTextureBits;
//        uint nTotalRead = fTextureFile.ReadBytes( pTextureBits, <#uint nNumBytes#>);

        sint nTexWidth;
        sint nTexHeight;
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, nTexWidth, nTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTextureBits );
        
        return pTexture;
    }
    IGfxSamplerState* COGLDevice::CreateSamplerState( GFX_TEXTURE_SAMPLE nType )
    {
        COGLSamplerState* pState = new COGLSamplerState;
        
        // TODO
        
        return pState;
    }
    //

    //
    IGfxBuffer* COGLDevice::CreateConstantBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage )
    {
        COGLBuffer* pBuffer = new COGLBuffer;
        
        glGenBuffers( 1, (GLuint*)&pBuffer->m_nBuffer );
        glBufferData( GL_UNIFORM_BUFFER, nSize, pInitialData, nUsage );

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
        GLvoid* pMappedData;
        // NOTE: need a way to determine the buffer binding type in order
        //       to map/unmap...also need the size of pData
//        pMappedData = glMapBuffer(<#GLenum target#>, GL_WRITE_ONLY );
//        Memcpy( pMappedData, pData, <#uint nSize#>)
//        glUnmapBuffer(<#GLenum target#>);
    }
    void COGLDevice::UpdateBuffer( IGfxBuffer* pBuffer, void* pData, uint nSize )
    {
        GLvoid* pMappedData;
        // NOTE: need a way to determine the buffer binding type in order
        //       to map/unmap
//        pMappedData = glMapBuffer(<#GLenum target#>, GL_WRITE_ONLY );
//        Memcpy( pMappedData, pData, nSize );
//        glUnmapBuffer(<#GLenum target#>);
    }
    //

    //
    void COGLDevice::SetVertexLayout( IGfxVertexLayout* pLayout )
    {
        glBindVertexArrayAPPLE( ( (COGLVertexLayout*)pLayout )->m_nLayout );
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
        m_bCreateShaderProgram = m_bCreateShaderProgram || ((COGLVertexShader*)m_pActiveVertexShader)->m_nShader != ((COGLVertexShader*)pShader)->m_nShader;
        
        if( m_bCreateShaderProgram )
            m_pActiveVertexShader = pShader;
    }

    void COGLDevice::SetPixelShader( IGfxPixelShader* pShader )
    {
        m_bCreateShaderProgram = m_bCreateShaderProgram || ((COGLPixelShader*)m_pActivePixelShader)->m_nShader != ((COGLPixelShader*)pShader)->m_nShader;
        
        if( m_bCreateShaderProgram )
            m_pActivePixelShader = pShader;
    }

    void COGLDevice::SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
        glBindBuffer( GL_UNIFORM_BUFFER, ( (COGLBuffer*)pBuffer )->m_nBuffer );
    }

    void COGLDevice::SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
        // NOTE: I think this is necessary in case the VS and PS don't share
        //       constant buffers (or in OpenGL speak: uniform blocks)
        glBindBuffer( GL_UNIFORM_BUFFER, ( (COGLBuffer*)pBuffer )->m_nBuffer );
    }
    void COGLDevice::SetPSSamplerState( IGfxSamplerState* pState )
    {
        // TODO
    }
    void COGLDevice::SetPSTexture( uint nIndex, IGfxTexture2D* pTexture )
    {
        // TODO
    }
    void COGLDevice::SetPSRenderTarget( uint nIndex, IGfxRenderTarget* pRenderTarget )
    {
        // TODO
//        glBindRenderbuffer( GL_RENDERBUFFER, ( (COGLRenderTarget*)pRenderTarget )->m_nRT );
    }
    void COGLDevice::SetRenderTarget( IGfxRenderTarget* pRenderTarget )
    {
        glBindRenderbuffer( GL_RENDERBUFFER, ( (COGLRenderTarget*)pRenderTarget )->m_nRT );
    }
    void COGLDevice::SetNullRenderTarget( void )
    {
        // "unbinding" active render target
        glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    }
    //

    //
    void COGLDevice::Draw( uint nVertexCount )
    {
        IGfxShaderProgram* pShaderProgram = CreateShaderProgram( m_pActiveVertexShader, m_pActivePixelShader );
        glUseProgram( (GLuint)( (COGLShaderProgram*)pShaderProgram )->m_nProgram );
        glDrawArrays( m_nPrimitiveType, 0, nVertexCount );
    }
    
    // Both of these functions use the appropriate glDrawElements*
    // calls because they use index buffer...otherwise they would
    // just use glDrawArrays*
    void COGLDevice::DrawIndexedPrimitive( uint nIndexCount )
    {
        // NOTE: make sure that the second parameter is correct.
        //       it seems from the definition in the back of the
        //       book that it should be stride and not the index count
        IGfxShaderProgram* pShaderProgram = CreateShaderProgram( m_pActiveVertexShader, m_pActivePixelShader );
        COGLShaderProgram* poglShaderProgram = (COGLShaderProgram*)pShaderProgram;
//        glUseProgram( (GLuint)( (COGLShaderProgram*)pShaderProgram )->m_nProgram );
        glUseProgram( (GLuint)poglShaderProgram->m_nProgram );
        glDrawElements( m_nPrimitiveType, 3, GL_UNSIGNED_INT, 0 );
    }
    void COGLDevice::DrawIndexedPrimitiveInstanced( uint nIndexCount, uint nInstanceCount )
    {
        // NOTE: ^read the note for DrawIndexedPrimitive
        IGfxShaderProgram* pShaderProgram = CreateShaderProgram( m_pActiveVertexShader, m_pActivePixelShader );
        glUseProgram( (GLuint)( (COGLShaderProgram*)pShaderProgram )->m_nProgram );
        glDrawElementsInstancedARB( m_nPrimitiveType, 3, GL_UNSIGNED_INT, 0, nInstanceCount );
    }
    
    void COGLDevice::DrawPrimitive( uint nVertexCount )
    {
        IGfxShaderProgram* pShaderProgram = CreateShaderProgram( m_pActiveVertexShader, m_pActivePixelShader );
        glUseProgram( (GLuint)( (COGLShaderProgram*)pShaderProgram )->m_nProgram );
        glDrawArrays( m_nPrimitiveType, 0, nVertexCount );
    }
    //

    Result LoadShaderProgram( const char* szFilename, GLuint nShader )
    {
        byte szBuffer[1024*16] = { 0 };
        
        FILE* pFile = fopen( szFilename, "rb" );
        ASSERT( pFile );
        
        fread( szBuffer, sizeof(byte), 1024*16, pFile );

        fclose( pFile );

        GLchar* pShaders[1];

        pShaders[0] = (GLchar*)szBuffer;
        
        glShaderSource( nShader, 1, (const GLchar**)pShaders, NULL );

        GLenum nError = glGetError();
        
        switch( nError )
        {
            case GL_INVALID_VALUE:
                break;
            case GL_INVALID_OPERATION:
                break;
            case GL_INVALID_ENUM:
                break;
            case GL_STACK_OVERFLOW:
                break;
            case GL_STACK_UNDERFLOW:
                break;
            case GL_OUT_OF_MEMORY:
                break;
            case GL_TABLE_TOO_LARGE:
                break;
            default:
                break;
        }
        
        ASSERT( nError == GL_NO_ERROR );

        if( nError != GL_NO_ERROR )
            return rResultFailure;

        return rResultSuccess;
    }


} // namespace Riot

#endif // #if USE_OPENGL

