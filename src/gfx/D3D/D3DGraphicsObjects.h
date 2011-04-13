/*********************************************************\
File:           D3DGraphicsObjects.h
Purpose:        The D3D graphics objects
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 8:57:18 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DGRAPHICSOBJECTS_H_
#define _D3DGRAPHICSOBJECTS_H_
#include "common.h"
#include "GraphicsObjects.h"
#include <D3D11.h>

namespace Riot
{    

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    class CD3DRenderTarget : public IRenderTarget
    {
        friend class CD3DGraphics;
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



} // namespace Riot

#endif // #ifndef _D3DGRAPHICSOBJECTS_H_
