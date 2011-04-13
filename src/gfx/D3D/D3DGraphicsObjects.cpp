/*********************************************************\
File:           D3DGraphicsObjects.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 8:57:33 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DGraphicsObjects.h"

namespace Riot
{    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

    // CD3DRenderTarget constructor
    CD3DRenderTarget::CD3DRenderTarget()
        : m_pRenderTargetView( NULL )
    {
    }

    // CD3DRenderTarget destructor
    CD3DRenderTarget::~CD3DRenderTarget()
    {
        SAFE_RELEASE( m_pRenderTargetView );
    }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot
