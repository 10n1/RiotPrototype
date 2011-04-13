/*********************************************************\
File:           D3DGraphicsObjects.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 9:33:11 PM
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

    CD3DBuffer::CD3DBuffer()
        : m_pBuffer( NULL )
    {
    }
    CD3DBuffer::~CD3DBuffer()
    {
        SAFE_RELEASE( m_pBuffer );
    }
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace Riot
