#include "Direct3DDevice.h"

//-----------------------------------------------------------------------------
//  Default shaders
//-----------------------------------------------------------------------------
static const char s_szDefaultVertexShaderFile[] = "DefaultShader.hlsl";
static const char s_szDefaultVertexShader[] = "DefaultVertexShader";
static const char s_szDefaultVertexShaderModel[] = "vs_4_0";

static const char s_szDefaultPixelShaderFile[] = "DefaultShader.hlsl";
static const char s_szDefaultPixelShader[] = "DefaultPixelShader";
static const char s_szDefaultPixelShaderModel[] = "ps_4_0";

Direct3DDevice::Direct3DDevice(void)
    : m_pDevice(NULL)
    , m_pContext(NULL)
    , m_pSwapChain(NULL)
    , m_pRenderTargetView(NULL)
    , m_pDepthStencilResource(NULL)
    , m_pDepthStencilView(NULL)
    , m_pViewProjCB(NULL)
    , m_pWorldCB(NULL)
{
}

Direct3DDevice::~Direct3DDevice(void)
{
    SAFE_RELEASE( m_pViewProjCB );
    SAFE_RELEASE( m_pWorldCB );
    SAFE_RELEASE( m_pRenderTargetView );
    SAFE_RELEASE( m_pDepthStencilResource );
    SAFE_RELEASE( m_pDepthStencilView );
    SAFE_RELEASE( m_pSwapChain );
    SAFE_RELEASE( m_pContext );
    SAFE_RELEASE( m_pDevice );
}

//-----------------------------------------------------------------------------
//  Initialize
//  Initializes DirectX
//-----------------------------------------------------------------------------
int Direct3DDevice::Initialize( HWND hWnd )
{    
    HRESULT hr = S_OK;

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
    tSwapChainDesc.Flags                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    
    /////////////////////////////////////////
    // Create the DX device
    D3D_FEATURE_LEVEL FeatureLevel;
    unsigned int nFlags = 0;
#ifdef _DEBUG
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
        &FeatureLevel,          // The feature level being used
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


    ///////////////////////////////////////////////
    // Create global constant buffers
    D3D11_BUFFER_DESC bufferDesc = { 0 };
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof( XMMATRIX );
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pViewProjCB );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        MessageBox( 0, "Constant buffer could not be created", "Error", 0 );
        return hr;
    }

    hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pWorldCB );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        MessageBox( 0, "Constant buffer could not be created", "Error", 0 );
        return hr;
    }

    return 0;
}

//-----------------------------------------------------------------------------
//  Resize
//  Resizes the DX buffers, etc.
//-----------------------------------------------------------------------------
int Direct3DDevice::Resize( unsigned int nWidth, unsigned int nHeight )
{
    HRESULT hr = S_OK;
    // TODO: Don't use these hardcoded values
    int     nAACount = 1,
            nAAQuality = 0;
    ///////////////////////////////////////////////
    // First release existing buffers
    m_pContext->OMSetRenderTargets( 0, 0, 0 );
    SAFE_RELEASE( m_pRenderTargetView );
    SAFE_RELEASE( m_pDepthStencilResource );
    SAFE_RELEASE( m_pDepthStencilView );
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
        return 1;
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
        return 1;
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

    return 0;
}

//-----------------------------------------------------------------------------
//  Render
//  Renders the scene
//-----------------------------------------------------------------------------
void Direct3DDevice::Render( void )
{
    HRESULT hr = S_OK;
    //////////////////////////////////////////////
    // Present at the beginning of the frame to
    // ensure the last frame is done being rendering
    // TODO: Support occluded present test
    hr = m_pSwapChain->Present( 0, 0 );    
    float fClearColor[4] = { 0.25f, 0.25f, 0.75f, 1.0f };
    m_pContext->ClearRenderTargetView( m_pRenderTargetView, fClearColor );
    m_pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    
    //////////////////////////////////////////////
    // Perform rendering
}

//-----------------------------------------------------------------------------
//  CreateShader
//  Creates a shader from a file
//-----------------------------------------------------------------------------
int Direct3DDevice::CreateShader( VertexShader** pShader, const char* szFilename, const char* szEntryPoint, const char* szShaderModel )
{
    HRESULT hr = S_OK;
    ID3DBlob*   pShaderBlob = NULL;

    if( szFilename && szEntryPoint && szShaderModel )
    {
        hr = CompileShader( szFilename, szEntryPoint, szShaderModel, &pShaderBlob );
    }
    else
    {
        hr = CompileShader( s_szDefaultVertexShaderFile, s_szDefaultVertexShader, s_szDefaultVertexShaderModel, &pShaderBlob );
    }

    // Create the shader
    VertexShader* pNewShader = new VertexShader;
    hr = pNewShader->Create( m_pDevice, m_pContext, pShaderBlob );

    if( FAILED( hr ) )
    {
        MessageBox( 0, "Shader creation failed!", "Error", 0 );
        SAFE_RELEASE( pShaderBlob );
        return hr;
    }

    *pShader = pNewShader;
    SAFE_RELEASE( pShaderBlob );
    return hr;
}

int Direct3DDevice::CreateShader( PixelShader** pShader, const char* szFilename, const char* szEntryPoint, const char* szShaderModel )
{    
    HRESULT hr = S_OK;
    ID3DBlob*   pShaderBlob = NULL;

    if( szFilename && szEntryPoint && szShaderModel )
    {
        hr = CompileShader( szFilename, szEntryPoint, szShaderModel, &pShaderBlob );
    }
    else
    {
        hr = CompileShader( s_szDefaultPixelShaderFile, s_szDefaultPixelShader, s_szDefaultPixelShaderModel, &pShaderBlob );
    }

    // Create the shader
    PixelShader* pNewShader = new PixelShader;
    hr = pNewShader->Create( m_pDevice, m_pContext, pShaderBlob );

    if( FAILED( hr ) )
    {
        MessageBox( 0, "Shader creation failed!", "Error", 0 );
        SAFE_RELEASE( pShaderBlob );
        return hr;
    }

    *pShader = pNewShader;
    SAFE_RELEASE( pShaderBlob );
    return hr;
}


int Direct3DDevice::CreateShader( GeometryShader** pShader, const char* szFilename, const char* szEntryPoint, const char* szShaderModel )
{    
    HRESULT hr = S_OK;
    ID3DBlob*   pShaderBlob = NULL;

    hr = CompileShader( szFilename, szEntryPoint, szShaderModel, &pShaderBlob );

    // Create the shader
    GeometryShader* pNewShader = new GeometryShader;
    hr = pNewShader->Create( m_pDevice, m_pContext, pShaderBlob );

    if( FAILED( hr ) )
    {
        MessageBox( 0, "Shader creation failed!", "Error", 0 );
        SAFE_RELEASE( pShaderBlob );
        return hr;
    }

    *pShader = pNewShader;
    SAFE_RELEASE( pShaderBlob );
    return hr;
}

int Direct3DDevice::CreateMesh( void* pVertices, 
                                unsigned int nVertexSize, 
                                unsigned int nNumVertices,
                                void* pIndices,
                                unsigned int nNumIndices,
                                Mesh** pMesh )
{
    HRESULT hr = S_OK;

    // Create the mesh
    Mesh* pNewMesh = new Mesh;

    hr = pNewMesh->Create( m_pDevice, m_pContext, pVertices, nVertexSize, nNumVertices, pIndices, nNumIndices );
    if( hr != 0 )
    {
        MessageBox( 0, "Mesh could not be created", "Error", 0 );
        SAFE_DELETE( pNewMesh );
        return hr;
    }

    *pMesh = pNewMesh;

    return hr;
}

void Direct3DDevice::SetViewProjMatrix( XMMATRIX& mViewProj )
{
    // Update it
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_pContext->Map( m_pViewProjCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    memcpy( mappedResource.pData, &mViewProj, sizeof( XMMATRIX ) );
    m_pContext->Unmap( m_pViewProjCB, 0 );

    // Set it
    m_pContext->VSSetConstantBuffers( 0, 1, &m_pViewProjCB );
}

void Direct3DDevice::SetWorldMatrix( XMMATRIX& mWorld )
{
    // Update it
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_pContext->Map( m_pWorldCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
    memcpy( mappedResource.pData, &mWorld, sizeof( XMMATRIX ) );
    m_pContext->Unmap( m_pWorldCB, 0 );

    // Set it
    m_pContext->VSSetConstantBuffers( 1, 1, &m_pWorldCB );
}

//-----------------------------------------------------------------------------
//  CompileShader
//  Compiles a shader from a file
//-----------------------------------------------------------------------------
int Direct3DDevice::CompileShader( const char* szFilename, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlob )
{    
    HRESULT hr = S_OK;
    // Load the shader first
    // TODO: Support more flags
    unsigned int nFlags = 0;
#ifdef _DEBUG
    nFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob = NULL;
    hr = D3DX11CompileFromFile( szFilename,     // Shader file to load
                                NULL,           // Shader #defines
                                NULL,           // Shader #includes
                                szEntryPoint,   // Name of the shader entry point
                                szShaderModel,  // Shader model to compile with
                                nFlags,         // Flags to compile with
                                0,              // Not used when compiling shaders
                                NULL,           // Thread pump
                                ppBlob,         // Compiled shader code
                                &pErrorBlob,    // Any errors
                                NULL );         // return code

    if( FAILED( hr ) )
    {
        char szErrorBuffer[4096] = { 0 };
        if( pErrorBlob != NULL )
        {
            wsprintf( szErrorBuffer, "Shader compilation failed!\nShader file: %s\nEntry Point: %s\nShader Model: %s\nError: %s", 
                szFilename, 
                szEntryPoint, 
                szShaderModel, 
                pErrorBlob->GetBufferPointer() );
            MessageBox( 0, szErrorBuffer, "Error", 0 );
        }
        else
        {
            wsprintf( szErrorBuffer, "Shader compilation failed!\nShader file: %s\nEntry Point: %s\nShader Model: %s", 
                szFilename, 
                szEntryPoint, 
                szShaderModel );
            MessageBox( 0, szErrorBuffer, "Error", 0 );
        }
        SAFE_RELEASE( pErrorBlob );
        return hr;
    }

    SAFE_RELEASE( pErrorBlob );
    return hr;
}
