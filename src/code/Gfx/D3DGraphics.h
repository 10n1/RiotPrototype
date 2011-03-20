/*********************************************************\
File:           D3DGraphics.h
Purpose:        Base interface for Direct3D
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 5:45:53 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DGRAPHICS_H_
#define _D3DGRAPHICS_H_
#include "Graphics.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

class CD3DGraphics : public CGraphics
{
public:
    // CD3DGraphics constructor
    CD3DGraphics();

    // CD3DGraphics destructor
    ~CD3DGraphics();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  CreateDevice
    //  Creates the device, reading info from the window
    //-----------------------------------------------------------------------------
    uint CreateDevice( CWindow* pWindow );
    
    //-----------------------------------------------------------------------------
    //  ReleaseBuffers
    //  Releases all buffers to prepare for a resize
    //-----------------------------------------------------------------------------
    void ReleaseBuffers( void );

    //-----------------------------------------------------------------------------
    //  CreateBuffers
    //  Creates all buffers required for rendering
    //-----------------------------------------------------------------------------
    void CreateBuffers( uint nWidth, uint nHeight );
    
    
    //-----------------------------------------------------------------------------
    //  PrepareRender
    //  Clears the screen to prepare for rendering
    //-----------------------------------------------------------------------------
    void PrepareRender( void );
    
    //-----------------------------------------------------------------------------
    //  Render
    //  Renders everything
    //-----------------------------------------------------------------------------
    void Render( CObject** ppObjects, uint nNumObjects );
    
    //-----------------------------------------------------------------------------
    //  Present
    //  Presents the frame
    //-----------------------------------------------------------------------------
    void Present( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/
        
    // Direct3D stuff
    ID3D11Device*           m_pDevice;
    ID3D11DeviceContext*    m_pContext;
    IDXGISwapChain*         m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11Texture2D*        m_pDepthStencilResource;
    ID3D11DepthStencilView* m_pDepthStencilView;
};


#endif // #ifndef _D3DGRAPHICS_H_
