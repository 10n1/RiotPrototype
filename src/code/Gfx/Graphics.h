/*********************************************************\
File:           Graphics.h
Purpose:        Base interface for the graphics hardware/API
                abstraction
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 5:15:23 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include "IRefCounted.h"
#include "Types.h"

class CWindow;

class CGraphics : public IRefCounted
{
public:
    // CGraphics constructor
    CGraphics();

    // CGraphics destructor
    virtual ~CGraphics();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  CreateDevice
    //  Creates the device, reading info from the window
    //-----------------------------------------------------------------------------
    virtual uint CreateDevice( CWindow* pWindow ) = 0;
    
    //-----------------------------------------------------------------------------
    //  ReleaseBuffers
    //  Releases all buffers to prepare for a resize
    //-----------------------------------------------------------------------------
    virtual void ReleaseBuffers( void ) = 0;

    //-----------------------------------------------------------------------------
    //  CreateBuffers
    //  Creates all buffers required for rendering
    //-----------------------------------------------------------------------------
    virtual void CreateBuffers( uint nWidth, uint nHeight ) = 0;

    //-----------------------------------------------------------------------------
    //  Resize
    //  Resizes the device
    //-----------------------------------------------------------------------------
    virtual void Resize( uint nWidth, uint nHeight );
    
    
    //-----------------------------------------------------------------------------
    //  PrepareRender
    //  Clears the screen to prepare for rendering
    //-----------------------------------------------------------------------------
    virtual void PrepareRender( void ) = 0;
    
    //-----------------------------------------------------------------------------
    //  Render
    //  Renders everything
    //-----------------------------------------------------------------------------
    virtual void Render( void ) = 0;
    
    //-----------------------------------------------------------------------------
    //  Present
    //  Presents the frame
    //-----------------------------------------------------------------------------
    virtual void Present( void ) = 0;

protected:
    /***************************************\
    | class members                         |
    \***************************************/
    CWindow*    m_pWindow;
};


#endif // #ifndef _GRAPHICS_H_
