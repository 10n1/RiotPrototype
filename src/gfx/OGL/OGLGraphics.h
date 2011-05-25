/*********************************************************\
File:           OGLGraphics.h
Purpose:        OpenGL graphics interface
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/21/2011 2:48:12 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OGLGRAPHICS_H_
#define _OGLGRAPHICS_H_
#include "SystemOpenGL.h"
#include "common.h"
#include "Graphics.h"
#include "GraphicsObjects.h"

#if USE_OPENGL

namespace Riot
{

    class COGLDevice : public IGraphicsDevice
    {
        friend class System;
    public:
        // COGLDevice constructor
        COGLDevice();

        // COGLDevice destructor
        ~COGLDevice();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Initialize
        //  Performs any API specific initialization tasks (wind order, etc)
        //-----------------------------------------------------------------------------
        Result Initialize( CWindow* pWindow );

        //-----------------------------------------------------------------------------
        //  Resize
        //  Resizes the graphics device (along with the back buffer )
        //-----------------------------------------------------------------------------
        void Resize( uint nWidth, uint nHeight );

        //-----------------------------------------------------------------------------
        //  Hardware methods
        //----------------------------------------------------------------------------- 
        void SetDefaultRenderDepthTarget( void );
       
        //
        void SetViewport( uint nWidth, uint nHeight );
        void SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha );
        void SetClearDepth( float fDepth );
        void Clear( void );
        void ClearRenderTarget( IGfxRenderTarget* pRT );
        void Present( void );
        //
        
        //
        void SetFillMode( GFX_FILL_MODE nFill );
        void SetDepthTest( bool bTest, bool bWrite );
        void SetColorWrite( bool bEnable );
        //

        //
        void CreateVertexShaderAndLayout( 
            const char* szFilename, 
            const char* szEntryPoint, 
            InputElementLayout Layout[],
            uint nLayoutCount,
            IGfxVertexShader** pShader,
            IGfxVertexLayout** pLayout );
        IGfxPixelShader* CreatePixelShader( const char* szFilename, const char* szEntryPoint );
        IGfxRenderTarget* CreateRenderTarget( GFX_FORMAT nFormat, uint nWidth, uint nHeight );
        //
        
        //
        IGfxTexture2D* LoadTexture( const char* szFilename );
        IGfxSamplerState* CreateSamplerState( GFX_TEXTURE_SAMPLE nType );
        //

        //
        IGfxBuffer* CreateConstantBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT );
        IGfxBuffer* CreateVertexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT );
        IGfxBuffer* CreateIndexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT );
        //
        
        //
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData );
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData, uint nSize );
        //

        //
        void SetVertexLayout( IGfxVertexLayout* pLayout );
        void SetVertexBuffer( uint nIndex, IGfxBuffer* pBuffer, uint nStride );
        void SetIndexBuffer( IGfxBuffer* pBuffer, uint nSize );
        void SetPrimitiveType( GFX_PRIMITIVE_TYPE nType );
        void SetVertexShader( IGfxVertexShader* pShader );
        void SetPixelShader( IGfxPixelShader* pShader );
        void SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer );
        void SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer );
        void SetPSSamplerState( IGfxSamplerState* pState );
        void SetPSTexture( uint nIndex, IGfxTexture2D* pTexture );
        void SetPSRenderTarget( uint nIndex, IGfxRenderTarget* pRenderTarget );
        void SetRenderTarget( IGfxRenderTarget* pRenderTarget );
        void SetNullRenderTarget( void );

        //

        //
        void Draw( uint nVertexCount );
        void DrawIndexedPrimitive( uint nIndexCount );
        void DrawIndexedPrimitiveInstanced( uint nIndexCount, uint nInstanceCount );
        void DrawPrimitive( uint nVertexCount );
        //

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        SystemOpenGL::TOpenGLDevice m_pDevice;
    };

} // namespace Riot

#endif // #if USE_OPENGL


#endif // #ifndef _OGLGRAPHICS_H_
