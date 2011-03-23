/*********************************************************\
File:           Graphics.h
Purpose:        Base interface for the graphics hardware/API
                abstraction
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/22/2011 10:45:57 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include "IRefCounted.h"
#include "Types.h"

class CWindow;
class CObject;
class CMesh;
class CMaterial;

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
    //  Initialize
    //  Creates the device, then creates any other needed buffers, etc.
    //-----------------------------------------------------------------------------
    virtual uint Initialize( CWindow* pWindow ) = 0;

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
    virtual void Render( CObject** ppObjects, uint nNumObjects  ) = 0;
    // TODO: Passing in the objects doesn't feel right...
    
    //-----------------------------------------------------------------------------
    //  Present
    //  Presents the frame
    //-----------------------------------------------------------------------------
    virtual void Present( void ) = 0;
    
    //-----------------------------------------------------------------------------
    //  SetViewProj
    //  Sets the view projection constant buffer
    //-----------------------------------------------------------------------------
    virtual void SetViewProj( const void* pView, const void* pProj ) = 0;
    
public:
    /***************************************\
    | object creation                       |
    \***************************************/

    // TODO: A list of all the vertex formats somewhere, matched to the appropriate
    //       vertex shader?
    
    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates a mesh from the file
    //-----------------------------------------------------------------------------
    virtual CMesh* CreateMesh( const wchar_t* szFilename ) = 0;
    
    //-----------------------------------------------------------------------------
    //  CreateMaterial
    //  Creates a material from a shader file
    //-----------------------------------------------------------------------------
    virtual CMaterial* CreateMaterial( const wchar_t* szFilename, const char* szEntryPoint, const char* szProfile ) = 0;

protected:
    /***************************************\
    | class members                         |
    \***************************************/
    CWindow*    m_pWindow;
};


#endif // #ifndef _GRAPHICS_H_
