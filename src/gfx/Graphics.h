/*********************************************************\
File:           Graphics.h
Purpose:        Base graphics hardware API
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/22/2011 1:26:04 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include "common.h"
#include "IRefCounted.h"
#include "GraphicsObjects.h"
#include "VertexFormats.h"

namespace Riot
{
    class CWindow;

    class IGraphicsDevice : public IRefCounted
    {
        friend class System;
        friend class Renderer;
    public:
        // IGraphicsDevice constructor
        IGraphicsDevice();

        // IGraphicsDevice destructor
        virtual ~IGraphicsDevice();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Initialize
        //  Performs any API specific initialization tasks (wind order, etc)
        //-----------------------------------------------------------------------------
        virtual Result Initialize( CWindow* pWindow ) = 0;

        
        //-----------------------------------------------------------------------------
        //  Resize
        //  Resizes the graphics device (along with the back buffer )
        //-----------------------------------------------------------------------------
        virtual void Resize( uint nWidth, uint nHeight ) = 0;

        //-----------------------------------------------------------------------------
        //  Hardware methods
        //-----------------------------------------------------------------------------
        virtual void SetDefaultRenderDepthTarget( void ) = 0;

        // 
        virtual void SetViewport( uint nWidth, uint nHeight ) = 0;
        virtual void SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha ) = 0;
        virtual void SetClearDepth( float fDepth ) = 0;
        virtual void Clear( void ) = 0;
        virtual void ClearRenderTarget( IGfxRenderTarget* pRT ) = 0;
        virtual void Present( void ) = 0;
        //

        //
        virtual void SetFillMode( GFX_FILL_MODE nFill ) = 0;
        virtual void SetDepthTest( bool bTest, bool bWrite ) = 0;
        virtual void SetColorWrite( bool bEnable ) = 0;
        //

        //
        virtual void CreateVertexShaderAndLayout( 
            const char* szFilename, 
            const char* szEntryPoint,
            InputElementLayout Layout[],
            uint nLayoutCount, 
            IGfxVertexShader** pShader,
            IGfxVertexLayout** pLayout ) = 0;
        virtual IGfxPixelShader* CreatePixelShader( const char* szFilename, const char* szEntryPoint ) = 0;
        virtual IGfxRenderTarget* CreateRenderTarget( GFX_FORMAT nFormat, uint nWidth, uint nHeight ) = 0;
        //

        //
        virtual IGfxTexture2D* LoadTexture( const char* szFilename ) = 0;
        virtual IGfxSamplerState* CreateSamplerState( GFX_TEXTURE_SAMPLE nType ) = 0;
        //virtual IGfxBlendState* CreateBlendState( GFX_TEXTURE_SAMPLE nType ) = 0;
        //

        //
        //virtual IGfxVertexLayout* CreateVertexLayout( InputElementLayout Layout[], uint nLength ) = 0;
        virtual IGfxBuffer* CreateConstantBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT ) = 0;
        virtual IGfxBuffer* CreateVertexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT ) = 0;
        virtual IGfxBuffer* CreateIndexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT ) = 0;
        //

        //
        virtual void UpdateBuffer( IGfxBuffer* pBuffer, void* pData ) = 0;
        virtual void UpdateBuffer( IGfxBuffer* pBuffer, void* pData, uint nSize ) = 0;
        //

        //
        virtual void SetVertexLayout( IGfxVertexLayout* pLayout ) = 0;
        virtual void SetVertexBuffer( uint nIndex, IGfxBuffer* pBuffer, uint nStride ) = 0;
        virtual void SetIndexBuffer( IGfxBuffer* pBuffer, uint nSize ) = 0;
        virtual void SetPrimitiveType( GFX_PRIMITIVE_TYPE nType ) = 0;
        virtual void SetVertexShader( IGfxVertexShader* pShader ) = 0;
        virtual void SetPixelShader( IGfxPixelShader* pShader ) = 0;
        virtual void SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer ) = 0;
        virtual void SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer ) = 0;
        virtual void SetPSSamplerState( IGfxSamplerState* pState ) = 0;
        virtual void SetPSTexture( uint nIndex, IGfxTexture2D* pTexture ) = 0;
        virtual void SetPSRenderTarget( uint nIndex, IGfxRenderTarget* pRenderTarget ) = 0;
        virtual void SetRenderTarget( IGfxRenderTarget* pRenderTarget ) = 0;
        virtual void SetNullRenderTarget( void ) = 0;
        //

        //
        virtual void Draw( uint nVertexCount ) = 0;
        virtual void DrawIndexedPrimitive( uint nIndexCount ) = 0;
        virtual void DrawIndexedPrimitiveInstanced( uint nIndexCount, uint nInstanceCount ) = 0;
        virtual void DrawPrimitive( uint nVertexCount ) = 0;
        //

    private:
        /***************************************\
        | class members                         |
        \***************************************/

    };


} // namespace Riot

#endif // #ifndef _GRAPHICS_H_
