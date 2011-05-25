/*********************************************************\
File:           D3DGraphicsObjects.h
Purpose:        The D3D graphics objects
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       5/21/2011 4:27:15 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DGRAPHICSOBJECTS_H_
#define _D3DGRAPHICSOBJECTS_H_
#include "common.h"

#if USE_DIRECTX

#include "GraphicsObjects.h"
#include <D3D11.h>

#define BEGIN_DEFINE_GRAPHICS_OBJECT( Name ) \
class CD3D##Name : public IGfx##Name             \
    {                                            \
        friend class CD3DDevice;                 \
    public:                                      \
        CD3D##Name();                            \
        ~CD3D##Name();                           \
    private:

#define END_DEFINE_GRAPHICS_OBJECT  \
    }

namespace Riot
{    

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    class CD3DRenderTarget : public IGfxRenderTarget
    {
        friend class CD3DDevice;
    public:
        // CD3DRenderTarget constructor
        CD3DRenderTarget();

        // CD3DRenderTarget destructor
        ~CD3DRenderTarget();

    private:
        ID3D11RenderTargetView*     m_pRT;
        ID3D11ShaderResourceView*   m_pRV;
        ID3D11Texture2D*            m_pTexture;
    };
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( Buffer )
        ID3D11Buffer*   m_pBuffer;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------    
    BEGIN_DEFINE_GRAPHICS_OBJECT( VertexShader )
        ID3D11VertexShader* m_pShader;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( PixelShader )
        ID3D11PixelShader*  m_pShader;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( VertexLayout )
        ID3D11InputLayout*  m_pLayout;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( Texture2D )
        ID3D11ShaderResourceView*  m_pResourceView;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( SamplerState )
        ID3D11SamplerState*  m_pState;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    BEGIN_DEFINE_GRAPHICS_OBJECT( BlendState )
        ID3D11BlendState*  m_pState;
    END_DEFINE_GRAPHICS_OBJECT;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


} // namespace Riot

#undef BEGIN_DEFINE_D3D_GRAPHICS_OBJECT
#undef END_DEFINE_D3D_GRAPHICS_OBJECT

#endif // #if USE_DIRECTX

#endif // #ifndef _D3DGRAPHICSOBJECTS_H_
