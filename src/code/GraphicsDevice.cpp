/*********************************************************\
File:           GraphicsDevice.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 4:46:19 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "GraphicsDevice.h"
#include "Window.h"
#include "Memory.h"

// CGraphicsDevice constructor
CGraphicsDevice::CGraphicsDevice()
    : m_pWindow( NULL )
{
}

// CGraphicsDevice destructor
CGraphicsDevice::~CGraphicsDevice()
{
}


//-----------------------------------------------------------------------------
//  Resize
//  Resizes the device
//-----------------------------------------------------------------------------
void CGraphicsDevice::Resize( uint nWidth, uint nHeight )
{    
    // Minimized protection
    // TODO: Better way to do this?
    if( nWidth != 0 && nHeight != 0 )
    {
        ReleaseBuffers();
        CreateBuffers( nWidth, nHeight );
    }
}
