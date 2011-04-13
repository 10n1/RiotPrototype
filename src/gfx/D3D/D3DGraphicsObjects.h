/*********************************************************\
File:           D3DGraphicsObjects.h
Purpose:        The D3D graphics objects
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 10:58:50 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DGRAPHICSOBJECTS_H_
#define _D3DGRAPHICSOBJECTS_H_
#include "common.h"
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
        ID3D11RenderTargetView* m_pRenderTargetView;
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

} // namespace Riot

#undef BEGIN_DEFINE_D3D_GRAPHICS_OBJECT
#undef END_DEFINE_D3D_GRAPHICS_OBJECT

#endif // #ifndef _D3DGRAPHICSOBJECTS_H_
