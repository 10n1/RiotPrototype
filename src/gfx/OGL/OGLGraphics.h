/*********************************************************\
File:           OGLGraphics.h
Purpose:        OpenGL graphics interface
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/14/2011 10:42:02 PM
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
        void Present( void );
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
        IGfxBuffer* CreateConstantBuffer( uint nSize, void* pInitialData );
        IGfxBuffer* CreateVertexBuffer( uint nSize, void* pInitialData );
        IGfxBuffer* CreateIndexBuffer( uint nSize, void* pInitialData );
        //
        
        //
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData );
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
        void DrawIndexedPrimitive( uint nIndexCount );
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
