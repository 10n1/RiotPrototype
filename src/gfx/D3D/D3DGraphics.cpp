/*********************************************************\
File:           D3DGraphics.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/24/2011 4:24:27 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DGraphics.h"
#include "Window.h"
#include "D3DGraphicsObjects.h"
#include <D3DX11.h>
#include <D3Dcompiler.h>

#if USE_DIRECTX

namespace Riot
{
    //-----------------------------------------------------------------------------
    //  Function declarations
    Result CompileShader( const char* szFilename, const char* szEntryPoint, const char* szProfile, ID3DBlob** ppBlob );
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    GFX_FORMAT GFX_FORMAT_FLOAT3        = DXGI_FORMAT_R32G32B32_FLOAT;
    GFX_FORMAT GFX_FORMAT_UINT16        = DXGI_FORMAT_R16_UINT;
    GFX_FORMAT GFX_FORMAT_UINT32        = DXGI_FORMAT_R32_UINT;
    GFX_FORMAT GFX_FORMAT_FLOAT2        = DXGI_FORMAT_R32G32_FLOAT;

    const uint GFX_FORMAT_FLOAT3_SIZE   = sizeof( RVector3 );
    const uint GFX_FORMAT_UINT16_SIZE   = sizeof( uint16 );
    const uint GFX_FORMAT_UINT32_SIZE   = sizeof( uint32 );
    //-----------------------------------------------------------------------------
    GFX_SEMANTIC GFX_SEMANTIC_POSITION  = "POSITION";
    GFX_SEMANTIC GFX_SEMANTIC_NORMAL    = "NORMAL";
    GFX_SEMANTIC GFX_SEMANTIC_TEXCOORD  = "TEXCOORD";
    //-----------------------------------------------------------------------------
    GFX_PRIMITIVE_TYPE GFX_PRIMITIVE_TRIANGLELIST   = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //-----------------------------------------------------------------------------
    GFX_TEXTURE_SAMPLE GFX_TEXTURE_SAMPLE_NEAREST   = D3D11_FILTER_MIN_MAG_MIP_POINT;
    GFX_TEXTURE_SAMPLE GFX_TEXTURE_SAMPLE_LINEAR    = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //-----------------------------------------------------------------------------

    // CD3DDevice constructor
    CD3DDevice::CD3DDevice()
        : m_pDevice( NULL )
        , m_pContext( NULL )
        , m_pSwapChain( NULL )
        , m_pDefaultRenderTargetView( NULL )
        , m_pDefaultDepthStencilResource( NULL )
        , m_pDefaultDepthStencilView( NULL )
    {
    }

    // CD3DDevice destructor
    CD3DDevice::~CD3DDevice()
    {
        SAFE_RELEASE( m_pDefaultRenderTargetView );
        SAFE_RELEASE( m_pDefaultDepthStencilResource );
        SAFE_RELEASE( m_pDefaultDepthStencilView );

        SAFE_RELEASE( m_pSwapChain );
        SAFE_RELEASE( m_pContext );
        SAFE_RELEASE( m_pDevice );
    }

    /***************************************\
    | class methods                         |
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //  Performs any API specific initialization tasks (wind order, etc)
    //-----------------------------------------------------------------------------
    Result CD3DDevice::Initialize( CWindow* pWindow )
    {
        HRESULT hr = S_OK;
        HWND hWnd = static_cast<HWND>( pWindow->GetWindow() );

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
        tSwapChainDesc.Flags                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;   // TODO: Fix the fullscreen 'ding'.
                                                                                        // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH isn't what causes it...


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
            ASSERT( FALSE );
            MessageBox( 0, L"Could not create DX device/swap chain", L"Error", 0 );
            return rResultFailure;
        }

        ///////////////////////////////////////////////
        // Do any feature level specific stuff
        switch( FeatureLevel )
        {
        case D3D_FEATURE_LEVEL_11_0:
            {
                strcpy( m_szVSProfile, "vs_5_0" );
                strcpy( m_szPSProfile, "ps_5_0" );
                break;
            }
        case D3D_FEATURE_LEVEL_10_1:
            {
                strcpy( m_szVSProfile, "vs_4_1" );
                strcpy( m_szPSProfile, "ps_4_1" );
                break;
            }
        case D3D_FEATURE_LEVEL_10_0:
            {
                strcpy( m_szVSProfile, "vs_4_0" );
                strcpy( m_szPSProfile, "ps_4_0" );
                break;
            }
        case D3D_FEATURE_LEVEL_9_3:
            {
                strcpy( m_szVSProfile, "vs_3_0" );
                strcpy( m_szPSProfile, "ps_3_0" );
                break;
            }
        default:
            {
                break;
            }
        };

        // Resize everything
        Resize( nWidth, nHeight );

        return rResultSuccess;
    }

    //-----------------------------------------------------------------------------
    //  Resize
    //  Resizes the graphics device (along with the back buffer )
    //-----------------------------------------------------------------------------
    void CD3DDevice::Resize( uint nWidth, uint nHeight )
    {
        /*
        We handle the resizing and back buffer 100% natively,
        just like OpenGL, rather than creating D3DRenderTargets, etc
        */

        //////////////////////////////////////////
        // First release all the current default objects
        if( m_pContext != NULL )
            m_pContext->OMSetRenderTargets( 0, 0, 0 );
        SAFE_RELEASE( m_pDefaultRenderTargetView );
        SAFE_RELEASE( m_pDefaultDepthStencilResource );
        SAFE_RELEASE( m_pDefaultDepthStencilView );

        ASSERT( m_pSwapChain );


        //////////////////////////////////////////
        // Then recreate them
        HRESULT hr = S_OK;
        // TODO: Don't use these hardcoded values
        int nAACount = 1,
            nAAQuality = 0;


        //////////////////////////////////////////
        // Resize the swap chain
        m_pSwapChain->ResizeBuffers( 2, nWidth, nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0 ); // First, null it out and resize

        //////////////////////////////////////////////
        // Create the new render target
        ID3D11Texture2D* pBackBuffer = NULL;
        hr = m_pSwapChain->GetBuffer( 0, __uuidof( pBackBuffer ), (void**)(&pBackBuffer) );
        hr = m_pDevice->CreateRenderTargetView( pBackBuffer, NULL, &m_pDefaultRenderTargetView );

        ASSERT( m_pDefaultRenderTargetView );
        SAFE_RELEASE( pBackBuffer );


        /////////////////////////////////////////////
        // Create the depth stencil texture and view
        D3D11_TEXTURE2D_DESC descDepth;
        descDepth.Width              = nWidth;
        descDepth.Height             = nHeight;
        descDepth.MipLevels          = 1;
        descDepth.ArraySize          = 1;
        descDepth.Format             = DXGI_FORMAT_D32_FLOAT;
        descDepth.SampleDesc.Count   = nAACount;
        descDepth.SampleDesc.Quality = nAAQuality;
        descDepth.Usage              = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags     = 0;
        descDepth.MiscFlags          = 0;
        hr = m_pDevice->CreateTexture2D( &descDepth, 0, &m_pDefaultDepthStencilResource );

        ASSERT( m_pDefaultDepthStencilResource);

        if(FAILED(hr))
        {
            // TODO: Handle gracefully
            MessageBox( 0, L"DepthStencil texture could not be created", L"Error", 0 );
            return;
        }

        // Create depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = nAACount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        descDSV.Flags = 0;
        hr = m_pDevice->CreateDepthStencilView( m_pDefaultDepthStencilResource, &descDSV, &m_pDefaultDepthStencilView );

        ASSERT( m_pDefaultDepthStencilView );

        if(FAILED(hr))
        {
            // TODO: Handle gracefully
            MessageBox( 0, L"DepthStencilView could not be created", L"Error", 0 );
            return;
        }

        // Set up the view port
        SetViewport( nWidth, nHeight );

        // Finally restore the default render/depth targets
        m_pContext->OMSetRenderTargets( 1, &m_pDefaultRenderTargetView, m_pDefaultDepthStencilView );
    }

    //-----------------------------------------------------------------------------
    //  Hardware methods
    //-----------------------------------------------------------------------------    
    void CD3DDevice::SetDefaultRenderDepthTarget( void )
    {
        m_pContext->OMSetRenderTargets( 1, &m_pDefaultRenderTargetView, m_pDefaultDepthStencilView );
    }

    void CD3DDevice::SetViewport( uint nWidth, uint nHeight )
    {
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

    void CD3DDevice::SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha )
    {
        m_fClearColor[0] = fRed;
        m_fClearColor[1] = fGreen;
        m_fClearColor[2] = fBlue;
        m_fClearColor[3] = fAlpha;
    }

    void CD3DDevice::SetClearDepth( float fDepth )
    {
        m_fDepthClear = fDepth;
    }

    void CD3DDevice::Clear( void )
    { 
        m_pContext->ClearRenderTargetView( m_pDefaultRenderTargetView, m_fClearColor );
        m_pContext->ClearDepthStencilView( m_pDefaultDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, m_fDepthClear, 0 ); 
    }

    void CD3DDevice::Present( void )
    {
        HRESULT hr = S_OK;    
        // TODO: Support occluded present test
        hr = m_pSwapChain->Present( 0, 0 );
    }
    //

    //
    void CD3DDevice::CreateVertexShaderAndLayout( 
            const char* szFilename, 
            const char* szEntryPoint,
            InputElementLayout Layout[],
            uint nLayoutCount, 
            IGfxVertexShader** pShader,
            IGfxVertexLayout** pLayout )
    {

        //////////////////////////////////////////
        ID3D11VertexShader* pVertexShader   = NULL;
        ID3D11InputLayout*  pInputLayout    = NULL;
        ID3DBlob*           pShaderBlob     = NULL;
        HRESULT             hr              = S_OK;
        Result              nResult         = rResultSuccess;

        // First compile the shader
        nResult = CompileShader( szFilename, szEntryPoint, m_szVSProfile, &pShaderBlob );
        ASSERT( nResult == rResultSuccess );

        void* pShaderCode = pShaderBlob->GetBufferPointer();
        uint  nShaderSize = (uint)pShaderBlob->GetBufferSize();
        
        // Then create the shader
        hr = m_pDevice->CreateVertexShader( pShaderCode, nShaderSize, NULL, &pVertexShader );
        assert( hr == S_OK );

        // and finally create the input layout
        D3D11_INPUT_ELEMENT_DESC inputLayout[] =
        {
            { Layout[0].szSemanticName, 0, (DXGI_FORMAT)Layout[0].nFormat, 0, Layout[0].nOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { Layout[1].szSemanticName, 0, (DXGI_FORMAT)Layout[1].nFormat, 0, Layout[1].nOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { Layout[2].szSemanticName, 0, (DXGI_FORMAT)Layout[2].nFormat, 0, Layout[2].nOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
        };

        hr = m_pDevice->CreateInputLayout( inputLayout, nLayoutCount, pShaderCode, nShaderSize, &pInputLayout );
        assert( hr == S_OK );

        // Now return them
        CD3DVertexShader* pNewShader = new CD3DVertexShader;
        CD3DVertexLayout* pNewLayout = new CD3DVertexLayout;
        pNewShader->m_pShader = pVertexShader;
        pNewLayout->m_pLayout = pInputLayout;

        *pShader = pNewShader;
        *pLayout = pNewLayout;
    }

    IGfxPixelShader* CD3DDevice::CreatePixelShader( const char* szFilename, const char* szEntryPoint )
    {
        CD3DPixelShader* pShader = new CD3DPixelShader;

        ID3DBlob*   pShaderBlob = NULL;
        Result      nResult = rResultSuccess;
        // First compile the shader
        nResult = CompileShader( szFilename, szEntryPoint, m_szPSProfile, &pShaderBlob );
        ASSERT( nResult == rResultSuccess );

        // Then create the shader
        void* pShaderCode = pShaderBlob->GetBufferPointer();
        uint  nShaderSize = (uint)pShaderBlob->GetBufferSize();
        HRESULT hr = m_pDevice->CreatePixelShader( pShaderCode, nShaderSize, NULL, &pShader->m_pShader );

        assert( hr == S_OK );

        return pShader;
    }
    //

    //
    IGfxTexture2D* CD3DDevice::LoadTexture( const char* szFilename )
    {
        CD3DTexture2D* pTexture = new CD3DTexture2D;

        size_t nConverted = 0;
        wchar_t szWFilename[256];
        mbstowcs_s( &nConverted, szWFilename, szFilename, strlen( szFilename ) );

        HRESULT hr = D3DX11CreateShaderResourceViewFromFile( m_pDevice, szWFilename, NULL, NULL, &pTexture->m_pResourceView, NULL );
        ASSERT( hr == S_OK );

        return pTexture;
    }
    IGfxSamplerState* CD3DDevice::CreateSamplerState( GFX_TEXTURE_SAMPLE nType )
    {
        CD3DSamplerState* pState = new CD3DSamplerState;

        D3D11_SAMPLER_DESC sampDesc;
        Memset( &sampDesc, 0, sizeof(sampDesc) );
        sampDesc.Filter = (D3D11_FILTER)nType;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        HRESULT hr = m_pDevice->CreateSamplerState( &sampDesc, &pState->m_pState );
        
        ASSERT( hr == S_OK );

        return pState;
    }
    //

    //
    IGfxBuffer* CD3DDevice::CreateConstantBuffer( uint nSize, void* pInitialData )
    {
        CD3DBuffer* pBuffer = new CD3DBuffer;

        //////////////////////////////////////////
        D3D11_BUFFER_DESC       bufferDesc  = { 0 };
        D3D11_SUBRESOURCE_DATA  initData    = { 0 };
        HRESULT                 hr          = S_OK;

        //////////////////////////////////////////
        bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth        = nSize;
        bufferDesc.BindFlags        = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags   = 0;


        D3D11_SUBRESOURCE_DATA* pData = NULL;

        if( pInitialData )
        {   
            // If we have initial data, fill the buffer with it
            initData.pSysMem = pInitialData;
            pData = &initData;
        }

        hr = m_pDevice->CreateBuffer( &bufferDesc, pData, &pBuffer->m_pBuffer );
        ASSERT( hr == S_OK );

        return pBuffer;
    }
    IGfxBuffer* CD3DDevice::CreateVertexBuffer( uint nSize, void* pInitialData )
    {
        CD3DBuffer* pBuffer = new CD3DBuffer;

        //////////////////////////////////////////
        D3D11_BUFFER_DESC       bufferDesc  = { 0 };
        D3D11_SUBRESOURCE_DATA  initData    = { 0 };
        HRESULT                 hr          = S_OK;

        //////////////////////////////////////////
        bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth        = nSize;
        bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags   = 0;


        D3D11_SUBRESOURCE_DATA* pData = NULL;

        if( pInitialData )
        {   
            // If we have initial data, fill the buffer with it
            initData.pSysMem = pInitialData;
            pData = &initData;
        }

        hr = m_pDevice->CreateBuffer( &bufferDesc, pData, &pBuffer->m_pBuffer );
        ASSERT( hr == S_OK );

        return pBuffer;
    }

    IGfxBuffer* CD3DDevice::CreateIndexBuffer( uint nSize, void* pInitialData )
    {
        CD3DBuffer* pBuffer = new CD3DBuffer;

        //////////////////////////////////////////
        D3D11_BUFFER_DESC       bufferDesc  = { 0 };
        D3D11_SUBRESOURCE_DATA  initData    = { 0 };
        HRESULT                 hr          = S_OK;

        //////////////////////////////////////////
        bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth        = nSize;
        bufferDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags   = 0;


        D3D11_SUBRESOURCE_DATA* pData = NULL;

        if( pInitialData )
        {   
            // If we have initial data, fill the buffer with it
            initData.pSysMem = pInitialData;
            pData = &initData;
        }

        hr = m_pDevice->CreateBuffer( &bufferDesc, pData, &pBuffer->m_pBuffer );
        ASSERT( hr == S_OK );

        return pBuffer;
    }
    //

    //
    void CD3DDevice::UpdateBuffer( IGfxBuffer* pBuffer, void* pData )
    {
        m_pContext->UpdateSubresource( ((CD3DBuffer*)pBuffer)->m_pBuffer, 0, NULL, pData, 0, 0 );
    }
    //

    //
    void CD3DDevice::SetVertexLayout( IGfxVertexLayout* pLayout )
    {
        m_pContext->IASetInputLayout( ((CD3DVertexLayout*)pLayout)->m_pLayout );
    }
    
    void CD3DDevice::SetVertexBuffer( IGfxBuffer* pBuffer, uint nStride )
    {
        uint nOffset = 0;
        m_pContext->IASetVertexBuffers( 0, 1, &((CD3DBuffer*)pBuffer)->m_pBuffer, &nStride, &nOffset );
    }
    
    void CD3DDevice::SetIndexBuffer( IGfxBuffer* pBuffer, uint nSize )
    {
        DXGI_FORMAT nIndexFormat = ( nSize == 2 ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // TODO: Clean this up
        m_pContext->IASetIndexBuffer( ((CD3DBuffer*)pBuffer)->m_pBuffer, nIndexFormat, 0 ); 
    }
    
    void CD3DDevice::SetPrimitiveType( GFX_PRIMITIVE_TYPE nType )
    {
        m_pContext->IASetPrimitiveTopology( (D3D11_PRIMITIVE_TOPOLOGY)nType );
    }
    
    void CD3DDevice::SetVertexShader( IGfxVertexShader* pShader )
    {
        m_pContext->VSSetShader( ((CD3DVertexShader*)pShader)->m_pShader, NULL, 0 );
    }

    void CD3DDevice::SetPixelShader( IGfxPixelShader* pShader )
    {
        m_pContext->PSSetShader( ((CD3DPixelShader*)pShader)->m_pShader, NULL, 0 );
    }
    void CD3DDevice::SetVSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
        m_pContext->VSSetConstantBuffers( nIndex, 1, &((CD3DBuffer*)pBuffer)->m_pBuffer );
    }
    void CD3DDevice::SetPSConstantBuffer( uint nIndex, IGfxBuffer* pBuffer )
    {
        m_pContext->PSSetConstantBuffers( nIndex, 1, &((CD3DBuffer*)pBuffer)->m_pBuffer );
    }
    void CD3DDevice::SetPSSamplerState( IGfxSamplerState* pState )
    {
        m_pContext->PSSetSamplers( 0, 1, &((CD3DSamplerState*)pState)->m_pState );
    }
    void CD3DDevice::SetPSTexture( uint nIndex, IGfxTexture2D* pTexture )
    {
        m_pContext->PSSetShaderResources( nIndex, 1, &((CD3DTexture2D*)pTexture)->m_pResourceView );
    }
    //

    //
    void CD3DDevice::DrawIndexedPrimitive( uint nIndexCount )
    {
        m_pContext->DrawIndexed( nIndexCount, 0, 0 );
    }
    //

    Result CompileShader( const char* szFilename, const char* szEntryPoint, const char* szProfile, ID3DBlob** ppBlob )
    {        
        size_t nConverted = 0;
        wchar_t szWFilename[256];
        mbstowcs_s( &nConverted, szWFilename, szFilename, strlen( szFilename ) );

        ID3DBlob*   pErrorBlob      = NULL;
        ID3DBlob*   pShaderBlob     = NULL;
        uint        nCompileFlags   = 0;
        HRESULT     hr              = S_OK;
        //////////////////////////////////////////
        // Load the shader
#ifdef DEBUG
        nCompileFlags = D3DCOMPILE_DEBUG;
#endif
        hr = D3DX11CompileFromFile(  szWFilename,    // Filename
            NULL,          // Array of macro definitions
            NULL,          // #include interface
            szEntryPoint,  // Function name
            szProfile,     // Shader profile
            nCompileFlags, // Compile flags
            0,             // Not used for shaders, only effects
            NULL,          // Thread pump
            &pShaderBlob,  // Compiled code
            &pErrorBlob,   // Errors
            NULL );        // HRESULT

        if( FAILED( hr ) )
        {
            // TODO: Handle error gracefully
            ASSERT( FALSE );
            const char* szWarnings = (const char*)pErrorBlob->GetBufferPointer();
            MessageBox( 0, (wchar_t*)pErrorBlob->GetBufferPointer(), L"Error", 0 );
            SAFE_RELEASE( pErrorBlob );
            return rResultFailure;
        }
        SAFE_RELEASE( pErrorBlob );

        *ppBlob = pShaderBlob;
        return rResultSuccess;
    }

} // namespace Riot

#endif // #if USE_DIRECTX
