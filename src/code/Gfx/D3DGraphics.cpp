/*********************************************************\
File:           D3DGraphics.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/6/2011 10:05:04 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DGraphics.h"
#include "Window.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include "Scene/Object.h"
#include "D3DMesh.h"
#include "D3DMaterial.h"
#include "Material.h"
#include "Gfx\View.h"
#include <fstream>
#include <xnamath.h>
#include "RiotMath.h"
#include "View.h"
#include "Main\Terrain.h"


// CD3DGraphics constructor
CD3DGraphics::CD3DGraphics()
    : m_pDevice( NULL )
    , m_pContext( NULL )
    , m_pSwapChain( NULL )
    , m_pRenderTargetView( NULL )
    , m_pDepthStencilResource( NULL )
    , m_pDepthStencilView( NULL )
    , m_pViewProjCB( NULL )
    , m_pWorldCB( NULL )
    , m_pLightCB( NULL )
{
}

// CD3DGraphics destructor
CD3DGraphics::~CD3DGraphics()
{
    SAFE_RELEASE( m_pLightCB );
    SAFE_RELEASE( m_pWorldCB );
    SAFE_RELEASE( m_pViewProjCB );
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
//  Initialize
//  Creates the device, then creates any other needed buffers, etc.
//-----------------------------------------------------------------------------
uint CD3DGraphics::Initialize( CWindow* pWindow )
{
    uint nResult = 0;

    //////////////////////////////////////////
    // First create the device
    nResult = CreateDevice( pWindow );

    //////////////////////////////////////////    
    D3D11_BUFFER_DESC       bufferDesc  = { 0 };
    D3D11_SUBRESOURCE_DATA  initData    = { 0 };
    HRESULT                 hr          = S_OK;

    //////////////////////////////////////////
    // Create the ViewProj Constant buffer
    bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth        = sizeof( XMMATRIX );
    bufferDesc.BindFlags        = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags   = 0;

    hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pViewProjCB );
    // TODO: Handle error


    //////////////////////////////////////////
    // Create the World matrix Constant buffer
    bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth        = sizeof( XMMATRIX );
    bufferDesc.BindFlags        = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags   = 0;

    hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pWorldCB );
    // TODO: Handle error


    //////////////////////////////////////////
    // Create the lighting Constant buffer
    bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth        = (sizeof( XMVECTOR ) * MAX_LIGHTS) + 16;
    bufferDesc.BindFlags        = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags   = 0;

    hr = m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pLightCB );
    // TODO: Handle error

    //////////////////////////////////////////
    // Create the materials
    m_pMaterials[eMaterialStandard] = CreateMaterial( L"Assets/Shaders/StandardVertexShader.hlsl", "PS" );
    m_pMaterials[eMaterialStandard]->ProcessObject(); // Make sure we have something bound

    //////////////////////////////////////////
    // Create the default mesh

    return nResult;
}

//-----------------------------------------------------------------------------
//  CreateDevice
//  Creates the device, reading info from the window
//-----------------------------------------------------------------------------
uint CD3DGraphics::CreateDevice( CWindow* pWindow )
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
        MessageBox( 0, L"Could not create DX device/swap chain", L"Error", 0 );
        return 1;
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
void CD3DGraphics::ReleaseBuffers( void )
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
void CD3DGraphics::CreateBuffers( uint nWidth, uint nHeight )
{
    if( m_pSwapChain == NULL )
    {
        // TODO: Handle error correctly
        return;
    }

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
        MessageBox( 0, L"DepthStencil texture could not be created", L"Error", 0 );
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
        MessageBox( 0, L"DepthStencilView could not be created", L"Error", 0 );
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
void CD3DGraphics::PrepareRender( void )
{
    float fClearColor[4] = { 0.25f, 0.25f, 0.75f, 1.0f };
    m_pContext->ClearRenderTargetView( m_pRenderTargetView, fClearColor );
    m_pContext->ClearDepthStencilView( m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 ); 
}

//-----------------------------------------------------------------------------
//  Render
//  Renders everything
//-----------------------------------------------------------------------------
void CD3DGraphics::Render( void )
{
    //////////////////////////////////////////////
    // Perform rendering

    // Update and set the view matrix
    XMMATRIX mView = m_pCurrView->GetViewMatrix();
    XMMATRIX mProj = m_pCurrView->GetProjMatrix();
    SetViewProj( &mView, &mProj );

    // Set lighting
    //if( m_bUpdateLighting == true )
    {
        m_pContext->UpdateSubresource( m_pLightCB, 0, NULL, m_vLights, 0, 0 );
        m_pContext->PSSetConstantBuffers( 0, 1, &m_pLightCB );
        m_bUpdateLighting = false;
    }

    // Now render all objects
    CGraphics::Render();
}

//-----------------------------------------------------------------------------
//  Present
//  Presents the frame
//-----------------------------------------------------------------------------
void CD3DGraphics::Present( void )
{
    HRESULT hr = S_OK;    
    // TODO: Support occluded present test
    hr = m_pSwapChain->Present( 0, 0 );
}

//-----------------------------------------------------------------------------
//  CreateMesh
//  Creates a mesh from the file
//-----------------------------------------------------------------------------
CMesh* CD3DGraphics::CreateMesh( const wchar_t* szFilename )
{

    if( szFilename != NULL )
    {
        //static unsigned int nCount = 0;
        //sprintf( szNewfile, "%d.mesh", nCount );
        //FILE* pFile = fopen( szNewfile, "wb" );
        //fwrite( &m_nVertexStride, sizeof( m_nVertexStride ), 1, pFile );
        //fwrite( &m_nVertexCount, sizeof( m_nVertexCount ), 1, pFile );
        //unsigned int IndexFormat = (m_nIndexFormat == DXGI_FORMAT_R16_UINT) ? 16 : 32;
        //fwrite( &IndexFormat, sizeof( IndexFormat ), 1, pFile );
        //fwrite( &m_nIndexCount, sizeof( m_nIndexCount ), 1, pFile );
        //fwrite( pVertices, m_nVertexStride, m_nVertexCount, pFile );
        //fwrite( pIndices, nIndexFormat/4, m_nIndexCount, pFile );
        //fclose( pFile );
    }
    else if( m_pBoxMesh == NULL )
    {
        // Create vertex buffer
        SimpleVertex vertices[] =
        {
            { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) },

            { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) },

            { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },
            { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ) },

            { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },
            { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3( 1.0f, 0.0f, 0.0f ) },

            { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },
            { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },
            { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },
            { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 0.0f, -1.0f ) },

            { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
            { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
            { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
            { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) },
        };

        WORD indices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        CMesh* pMesh = CreateMesh( sizeof( SimpleVertex ), ARRAYSIZE( vertices ), 16, ARRAYSIZE( indices ), vertices, indices );

        m_pBoxMesh = pMesh;
    }

    m_pBoxMesh->AddRef();
    return m_pBoxMesh;
}


//-----------------------------------------------------------------------------
//  CreateMesh
//  Creates a mesh from the file
//-----------------------------------------------------------------------------
CMesh* CD3DGraphics::CreateMesh( uint nVertexStride, uint nVertexCount, uint nIndexFormat, uint nIndexCount, void* pVertices, void* pIndices )
{
    D3D11_BUFFER_DESC       bufferDesc  = { 0 };
    D3D11_SUBRESOURCE_DATA  initData    = { 0 };
    HRESULT                 hr          = S_OK;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    //////////////////////////////////////////
    //  Create the new mesh
    CD3DMesh*   pMesh = new CD3DMesh();
    pMesh->m_pDeviceContext = m_pContext;

    //////////////////////////////////////////
    // Load the shader    
    ID3DBlob*   pShaderBlob = NULL;
    ID3DBlob*   pErrorBlob = NULL;
    uint nCompileFlags = 0;
#ifdef DEBUG
    nCompileFlags = D3DCOMPILE_DEBUG;
#endif
    hr = D3DX11CompileFromFile(  L"Assets/Shaders/StandardVertexShader.hlsl", // Filename
        NULL,           // Array of macro definitions
        NULL,           // #include interface
        "VS",           // Function name
        m_szVSProfile,       // Shader profile
        nCompileFlags,  // Compile flags
        0,              // Not used for shaders, only effects
        NULL,           // Thread pump
        &pShaderBlob,   // Compiled code
        &pErrorBlob,    // Errors
        NULL );         // HRESULT

    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, (wchar_t*)pErrorBlob->GetBufferPointer(), L"Error", 0 );
        SAFE_RELEASE( pErrorBlob );
    }
    SAFE_RELEASE( pErrorBlob );

    // Now create the shader
    hr = m_pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &pMesh->m_pVertexShader );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create shader", L"Error", 0 );
        SAFE_RELEASE( pShaderBlob );
    }

    //////////////////////////////////////////
    // Create input layout
    hr = m_pDevice->CreateInputLayout( layout, 
        numElements, 
        pShaderBlob->GetBufferPointer(), 
        pShaderBlob->GetBufferSize(), 
        &pMesh->m_pVertexLayout );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create input layout", L"Error", 0 );
        SAFE_RELEASE( pShaderBlob );
    } 
    SAFE_RELEASE( pShaderBlob );

    //////////////////////////////////////////
    // Create vertex buffer
    bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth        = nVertexStride * nVertexCount;
    bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags   = 0;
    initData.pSysMem            = pVertices;

    hr = m_pDevice->CreateBuffer( &bufferDesc, &initData, &pMesh->m_pVertexBuffer );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create Vertex buffer", L"Error", 0 );
    } 

    //////////////////////////////////////////
    // Create index buffer
    bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth        = nIndexCount * ( nIndexFormat == 16 ? 2 : 4 );
    bufferDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags   = 0;
    initData.pSysMem            = pIndices;
    hr = m_pDevice->CreateBuffer( &bufferDesc, &initData, &pMesh->m_pIndexBuffer );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create index buffer", L"Error", 0 );
    } 

    pMesh->m_nIndexSize     = nIndexFormat;
    pMesh->m_nIndexCount    = nIndexCount;
    pMesh->m_nVertexSize    = nVertexStride;

    return pMesh;
}

//-----------------------------------------------------------------------------
//  CreateMaterial
//  Creates a material from a shader file
//-----------------------------------------------------------------------------
CMaterial* CD3DGraphics::CreateMaterial( const wchar_t* szFilename, const char* szEntryPoint )
{    
    ID3DBlob*   pShaderBlob = NULL;
    ID3DBlob*   pErrorBlob = NULL;
    uint        nCompileFlags = 0;
    HRESULT     hr = S_OK;

    //////////////////////////////////////////
    // Create the material
    CD3DMaterial*   pMaterial = new CD3DMaterial();
    pMaterial->m_pDeviceContext = m_pContext;

    //////////////////////////////////////////
    // Load the shader

#ifdef DEBUG
    nCompileFlags = D3DCOMPILE_DEBUG;
#endif
    hr = D3DX11CompileFromFile(  szFilename,    // Filename
        NULL,          // Array of macro definitions
        NULL,          // #include interface
        szEntryPoint,  // Function name
        m_szPSProfile, // Shader profile
        nCompileFlags, // Compile flags
        0,             // Not used for shaders, only effects
        NULL,          // Thread pump
        &pShaderBlob,  // Compiled code
        &pErrorBlob,   // Errors
        NULL );        // HRESULT

    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, (wchar_t*)pErrorBlob->GetBufferPointer(), L"Error", 0 );
        SAFE_RELEASE( pErrorBlob );
    }
    SAFE_RELEASE( pErrorBlob );

    // Now create the shader
    hr = m_pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &pMaterial->m_pPixelShader );
    if( FAILED( hr ) )
    {
        // TODO: Handle error gracefully
        DebugBreak();
        MessageBox( 0, L"Couldn't create shader", L"Error", 0 );
        SAFE_RELEASE( pShaderBlob );
    }

    return pMaterial;
}

//-----------------------------------------------------------------------------
//  SetViewProj
//  Sets the view projection constant buffer
//-----------------------------------------------------------------------------
void CD3DGraphics::SetViewProj( const void* pView, const void* pProj )
{
    XMMATRIX mMatrices[1] = 
    {
        XMMatrixTranspose( *((XMMATRIX*)pView) * *((XMMATRIX*)pProj)), 

    };

    m_pContext->UpdateSubresource( m_pViewProjCB, 0, NULL, mMatrices, 0, 0 );
    m_pContext->VSSetConstantBuffers( 0, 1, &m_pViewProjCB );
}

//-----------------------------------------------------------------------------
//  SetWorldMatrix
//  Applies the world matrix to the pipeline
//-----------------------------------------------------------------------------
void CD3DGraphics::SetWorldMatrix( XMMATRIX* pMatrix )
{
    m_pContext->UpdateSubresource( m_pWorldCB, 0, NULL, pMatrix, 0, 0 );
    m_pContext->VSSetConstantBuffers( 1, 1, &m_pWorldCB );

    RVector4 a( 0, 1, 0 ), b( 0, 1, 0 );

    float f = DotProduct( a, b );
}
