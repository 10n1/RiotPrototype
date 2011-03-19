/*********************************************************\
File:           Direct3DDevice.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 4:58:53 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Direct3DDevice.h"
#include "Window.h"
#include <d3d11.h>
#include "Memory.h"

// CDirect3DDevice constructor
CDirect3DDevice::CDirect3DDevice()
    : m_pDevice( NULL )
    , m_pContext( NULL )
    , m_pSwapChain( NULL )
    , m_pRenderTargetView( NULL )
    , m_pDepthStencilResource( NULL )
    , m_pDepthStencilView( NULL )
{
}

// CDirect3DDevice destructor
CDirect3DDevice::~CDirect3DDevice()
{
    SAFE_RELEASE( m_pRenderTargetView );
    SAFE_RELEASE( m_pDepthStencilResource );
    SAFE_RELEASE( m_pDepthStencilView );
    SAFE_RELEASE( m_pSwapChain );
    SAFE_RELEASE( m_pContext );
    SAFE_RELEASE( m_pDevice );
}
/***************************************\
| class methods                         |
\***************************************/
    
//-----------------------------------------------------------------------------
//  CreateDevice
//  Creates the device, reading info from the window
//-----------------------------------------------------------------------------
uint CDirect3DDevice::CreateDevice( CWindow* pWindow )
{
    HRESULT hr = S_OK;
    HWND hWnd = static_cast<HWND>( pWindow->GetSystemWindow() );

    // Get the actual client size of the window
    RECT rcClient;
    GetClientRect( hWnd, &rcClient );
    unsigned int nWidth = rcClient.right - rcClient.left;
    unsigned int nHeight = rcClient.bottom - rcClient.top;

    // TODO: Don't use these hardcoded values
    int     nAACount = 1,
            nAAQuality = 0;
    bool    bWindowed = true;

    /////////////////////////////////////////
    // Create the swap chain description
    DXGI_SWAP_CHAIN_DESC tSwapChainDesc = {0};
    tSwapChainDesc.BufferCount          = 2;         // buffer count
    tSwapChainDesc.BufferDesc.Width     = nWidth;    // width of the window
    tSwapChainDesc.BufferDesc.Height    = nHeight;   // height of the window
    tSwapChainDesc.BufferDesc.Format    = DXGI_FORMAT_R8G8B8A8_UNORM;   // Back buffer format
                                                                        // TODO: Don't hardcode this
    tSwapChainDesc.BufferDesc.RefreshRate.Numerator     = 60;   // Refresh rate
    tSwapChainDesc.BufferDesc.RefreshRate.Denominator   = 1;    // Refresh rate
    tSwapChainDesc.BufferUsage          = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Its a render target!
    tSwapChainDesc.OutputWindow         = hWnd;       // Window to attach to
    tSwapChainDesc.SampleDesc.Count     = nAACount;   // AA count
    tSwapChainDesc.SampleDesc.Quality   = nAAQuality; // AA quality
    tSwapChainDesc.Windowed             = bWindowed;  // Windowed!! Yay!
    tSwapChainDesc.Flags                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // TODO: Is the automatic switching what causes the 'ding'?

    
    /////////////////////////////////////////
    // Create the DX device
    D3D_FEATURE_LEVEL FeatureLevel;
    unsigned int nFlags = 0;
#ifdef DEBUG
    nFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    hr = D3D11CreateDeviceAndSwapChain(    
        0,                        // physical adapter to try
        D3D_DRIVER_TYPE_HARDWARE, // TODO: Add support for software/warp
        0,                        // TODO: Add support for software/warp
        nFlags,                   // Creation flags
        NULL,                     // Feature levels to try
        0,                        // Number of feature levels
        D3D11_SDK_VERSION,        // The SDK version
        &tSwapChainDesc,          // The Swap chain description
        &m_pSwapChain,            // The swap chain
        &m_pDevice,               // the device
        &FeatureLevel,            // The feature level being used
        &m_pContext );            // The context!

    if( FAILED( hr ) )
    {
        // TODO: Handle gracefully
        MessageBox( 0, "Could not create DX device/swap chain", "Error", 0 );
        return 1;
    }

    ///////////////////////////////////////////////
    // Do any feature level specific stuff
    switch( FeatureLevel )
    {
    case D3D_FEATURE_LEVEL_11_0:
        {
            break;
        }
    case D3D_FEATURE_LEVEL_10_1:
        {
            break;
        }
    case D3D_FEATURE_LEVEL_10_0:
        {
            break;
        }
    case D3D_FEATURE_LEVEL_9_3:
        {
            break;
        }
    default:
        {
            break;
        }
    };

    m_pWindow = pWindow;

    //////////////////////////////////////////
    //  Create the buffers
    Resize( nWidth, nHeight );

    return hr;
}

//-----------------------------------------------------------------------------
//  ReleaseBuffers
//  Releases all buffers to prepare for a resize
//-----------------------------------------------------------------------------
void CDirect3DDevice::ReleaseBuffers( void )
{
    if( m_pContext != NULL )
        m_pContext->OMSetRenderTargets( 0, 0, 0 );
    SAFE_RELEASE( m_pRenderTargetView );
    SAFE_RELEASE( m_pDepthStencilResource );
    SAFE_RELEASE( m_pDepthStencilView );
}

//-----------------------------------------------------------------------------
//  CreateBuffers
//  Creates all buffers required for rendering
//-----------------------------------------------------------------------------
void CDirect3DDevice::CreateBuffers( uint nWidth, uint nHeight )
{
    HRESULT hr = S_OK;
    // TODO: Don't use these hardcoded values
    int     nAACount = 1,
            nAAQuality = 0;


    //////////////////////////////////////////
    // Resize the swap chain
    m_pSwapChain->ResizeBuffers( 2, nWidth, nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0 ); // First, null it out and resize

    //////////////////////////////////////////////
    // Create the new render target
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = m_pSwapChain->GetBuffer( 0, __uuidof( pBackBuffer ), (void**)(&pBackBuffer) );
    hr = m_pDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pRenderTargetView );
    SAFE_RELEASE( pBackBuffer );

    
    /////////////////////////////////////////////
    // Create the depth stencil texture and view
    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width              = nWidth;
    descDepth.Height             = nHeight;
    descDepth.MipLevels          = 1;
    descDepth.ArraySize          = 1;
    descDepth.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count   = nAACount;
    descDepth.SampleDesc.Quality = nAAQuality;
    descDepth.Usage              = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags     = 0;
    descDepth.MiscFlags          = 0;
    hr = m_pDevice->CreateTexture2D( &descDepth, 0, &m_pDepthStencilResource );
    if(FAILED(hr))
    {
        // TODO: Handle gracefully
        MessageBox( 0, "DepthStencil texture could not be created", "Error", 0 );
        return;
    }

    // Create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = nAACount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    descDSV.Flags = 0;
    hr = m_pDevice->CreateDepthStencilView( m_pDepthStencilResource, &descDSV, &m_pDepthStencilView );
    if(FAILED(hr))
    {
        // TODO: Handle gracefully
        MessageBox( 0, "DepthStencilView could not be created", "Error", 0 );
        return;
    }

    //////////////////////////////////////////////
    // Set the new targets
    m_pContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );

    //////////////////////////////////////////////
    // Set up the view port
    D3D11_VIEWPORT vp = {0};
    vp.Width    = (float)nWidth;
    vp.Height   = (float)nHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &vp);
}



//-----------------------------------------------------------------------------
//  PrepareRender
//  Clears the screen to prepare for rendering
//-----------------------------------------------------------------------------
void CDirect3DDevice::PrepareRender( void )
{
    float fClearColor[4] = { 0.25f, 0.25f, 0.75f, 1.0f };
    m_pContext->ClearRenderTargetView( m_pRenderTargetView, fClearColor );
    m_pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 ); 
}

//-----------------------------------------------------------------------------
//  Render
//  Renders everything
//-----------------------------------------------------------------------------
void CDirect3DDevice::Render( void )
{
    //////////////////////////////////////////////
    // Perform rendering
}

//-----------------------------------------------------------------------------
//  Present
//  Presents the frame
//-----------------------------------------------------------------------------
void CDirect3DDevice::Present( void )
{
    HRESULT hr = S_OK;    
    // TODO: Support occluded present test
    hr = m_pSwapChain->Present( 0, 0 );
}
