/*********************************************************\
File:           D3DGraphics.h
Purpose:        Direct3D interface
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/27/2011 3:28:24 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DGRAPHICS_H_
#define _D3DGRAPHICS_H_
#include "common.h"
#include "Graphics.h"
#include "GraphicsObjects.h"
#include "VertexFormats.h"
#include <D3D11.h>

#if USE_DIRECTX

namespace Riot
{

    class CD3DDevice : public IGraphicsDevice
    {
        friend class System;
    public:
        // CD3DDevice constructor
        CD3DDevice();

        // CD3DDevice destructor
        ~CD3DDevice();
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
        void Present( void );
        //

        //
        void SetFillMode( GFX_FILL_MODE nFill );
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
        //

        //
        IGfxTexture2D* LoadTexture( const char* szFilename );
        IGfxSamplerState* CreateSamplerState( GFX_TEXTURE_SAMPLE nType );
        //

        //
        IGfxBuffer* CreateConstantBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage );
        IGfxBuffer* CreateVertexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage );
        IGfxBuffer* CreateIndexBuffer( uint nSize, void* pInitialData, GFX_BUFFER_USAGE nUsage );
        //
        
        //
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData );
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData, uint nSize );
        //

        //
        void SetVertexLayout( IGfxVertexLayout* pLayout );
        void SetVertexBuffer( IGfxBuffer* pBuffer, uint nStride );
        void SetIndexBuffer( IGfxBuffer* pBuffer, uint nSize );
        void SetPrimitiveType( GFX_PRIMITIVE_TYPE nType );
        void SetVertexShader( IGfxVertexShader* pShader );
        void SetPixelShader( IGfxPixelShader* pShader );
        void SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer );
        void SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer );
        void SetPSSamplerState( IGfxSamplerState* pState );
        void SetPSTexture( uint nIndex, IGfxTexture2D* pTexture );
        //

        //
        void Draw( uint nVertexCount );
        void DrawIndexedPrimitive( uint nIndexCount );
        void DrawPrimitive( uint nVertexCount );
        //

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        float   m_fClearColor[4];   // 0    (+16)
        float   m_fDepthClear;      // 16   (+4)
        char    m_szVSProfile[7];   // 20   (+7)
        char    m_szPSProfile[7];   // 27   (+7)
        char    _padding[2];        // 34   (+2)
                                    // 36

        // The device, etc
        ID3D11Device*           m_pDevice;
        ID3D11DeviceContext*    m_pContext;
        IDXGISwapChain*         m_pSwapChain;

        // The default render target
        /*
        We handle the resizing and back buffer 100% natively,
        just like OpenGL, rather than creating D3DRenderTargets, etc
        */
        ID3D11RenderTargetView* m_pDefaultRenderTargetView;
        ID3D11Texture2D*        m_pDefaultDepthStencilResource;
        ID3D11DepthStencilView* m_pDefaultDepthStencilView;

        ID3D11RasterizerState*  m_pSolidRasterizerState;
        ID3D11RasterizerState*  m_pWireframeRasterizerState;
    };

} // namespace Riot

#endif // #if USE_DIRECTX


#endif // #ifndef _D3DGRAPHICS_H_
