/*********************************************************\
File:           GraphicsDevice.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 5:14:39 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Graphics.h"
#include "Window.h"
#include "Memory.h"

// CGraphics constructor
CGraphics::CGraphics()
    : m_pWindow( NULL )
{
}

// CGraphics destructor
CGraphics::~CGraphics()
{
}


//-----------------------------------------------------------------------------
//  Resize
//  Resizes the device
//-----------------------------------------------------------------------------
void CGraphics::Resize( uint nWidth, uint nHeight )
{    
    // Minimized protection
    // TODO: Better way to do this?
    if( nWidth != 0 && nHeight != 0 )
    {
        ReleaseBuffers();
        CreateBuffers( nWidth, nHeight );
    }
}
