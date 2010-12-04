/*********************************************************\
File:           main.cpp
\*********************************************************/
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include "Timer.h"
#include "Direct3DDevice.h"
#include "Camera.h"
#include <xnamath.h>
#include "RiotObject.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
// Defines
static const char         gs_szName[] = "3D World";
static const unsigned int gs_nWidth = 1024;
static const unsigned int gs_nHeight = 768;
static const bool         gs_bFullscreen = false;
static const float        gs_fWorldSize = 1000.0f;


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p) { p->Release(); p = NULL; } 
#endif

//-----------------------------------------------------------------------------
// Variables
CTimer              g_Timer;
HWND                g_hWnd = NULL;
Direct3DDevice*     g_pD3D = NULL;
Camera              g_Camera;
bool                g_bRunning = true;

// Ground plane
VertexShader*       g_pVertexShader = NULL;
PixelShader*        g_pPixelShader = NULL;
Mesh*               g_pMesh = NULL;

RiotObject*         g_pGroundPlane = NULL;

ID3D11ShaderResourceView*   g_pTexture = NULL;
ID3D11SamplerState*         g_pSampler = NULL;

//-----------------------------------------------------------------------------
// Methods
int CALLBACK WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow );
LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
int InitializeWindow( HWND* phWnd, 
                      HINSTANCE hInst, 
                      WNDPROC pWndProc,
                      const char* szName,
                      unsigned int nWidth, 
                      unsigned int nHeight, 
                      int nCmdShow );
int InitializeGame( void );

//-----------------------------------------------------------------------------
//  WinMain
//  Entrypoint for the program
//-----------------------------------------------------------------------------
int CALLBACK WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow )
{
    int hr = 0;
    /////////////////////////////////////
    // Initialize the window
    hr = InitializeWindow( &g_hWnd,      // The window
                           hInst,        // The instance
                           WndProc,      // The window's message handler
                           gs_szName,    // The name of the class and window
                           gs_nWidth, gs_nHeight, // Size
                           nCmdShow );   // Show parameters

    if( hr != 0 )
    {
        MessageBox( 0, "Could not create window!", "Error", 0 );
        // TODO: Handle error more gracefully
        g_bRunning = false;
    }
    
    /////////////////////////////////////
    // Initialize Direct3D
    g_pD3D = new Direct3DDevice();
    hr = g_pD3D->Initialize( g_hWnd );
    
    if( hr != 0 )
    {
        MessageBox( 0, "DX Initialization failed!", "Error", 0 );
        // TODO: Handle error more gracefully
        g_bRunning = false;
    }
    
    // Send a resize message to the window to ensure
    // the DX buffers are resized "correctly"
    LPARAM lParam = (LPARAM)(gs_nWidth + (gs_nHeight << 16));
    SendMessage( g_hWnd, WM_SIZE, 0, lParam );

    hr = InitializeGame();
    if( hr != 0 )
    {
        MessageBox( 0, "Could not initialize game!", "Error", 0 );
        // TODO: Handle error more gracefully
        g_bRunning = false;
    }
        
    /////////////////////////////////////
    // Main program loop
    g_Timer.Reset();
    float fElapsedTime = (float)g_Timer.GetTime();
    int nFrameCount = 0;
    MSG msg = { 0 };
    while( g_bRunning == true )
    {
        // Perform windows messaging
        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
                g_bRunning = false;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        
        //-----------------------------------------------------------------------------
        // Perform Game update
        // Set the constant buffer
        XMMATRIX mViewProj = XMMatrixTranspose( g_Camera.GetViewProj() );
        g_pD3D->SetViewProjMatrix( mViewProj );
        g_pD3D->GetContext()->PSSetSamplers( 0, 1, &g_pSampler );
        g_pGroundPlane->Render();

        g_pD3D->Render();
        //-----------------------------------------------------------------------------
        fElapsedTime = (float)g_Timer.GetTime();
        nFrameCount++;
        if( (nFrameCount & 10000) == 10000 )
        {
            char szTitle[256];
            sprintf_s( szTitle, 256, "FrameTime = %f ms", fElapsedTime * 1000.0f );
            // TODO: Add real text rendering, SetWindowText isn't a good idea...
            //SetWindowText( g_hWnd, szTitle );
        }
    }

    
    /////////////////////////////////////
    // Perform clean up
    SAFE_DELETE( g_pGroundPlane );
    SAFE_RELEASE( g_pSampler );
    SAFE_DELETE( g_pD3D );

    return 0;
}

//-----------------------------------------------------------------------------
//  InitializeWindow
//  Create and show a window
//-----------------------------------------------------------------------------
int InitializeWindow( HWND* phWnd, 
                      HINSTANCE hInst, 
                      WNDPROC pWndProc,
                      const char* szName,
                      unsigned int nWidth, 
                      unsigned int nHeight, 
                      int nCmdShow )
{
    /////////////////////////////////////
    // Create and register the class
    WNDCLASS wndClass;
    memset(&wndClass, 0, sizeof(wndClass));
    wndClass.lpfnWndProc    = pWndProc;
    wndClass.hInstance      = hInst;
    wndClass.hCursor        = LoadCursor(0, IDC_ARROW);
    wndClass.lpszClassName  = szName;
    wndClass.style          = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wndClass);

    /////////////////////////////////////
    // Create and show the window
    *phWnd = CreateWindow(
        szName, szName,      // Class and window name
        WS_OVERLAPPEDWINDOW, // Window style
        0, 0,                // X, Y position
        nWidth, nHeight,     // Width and height (border width 16 and 38)
        0, 0,                // Parent window, Menu handle
        hInst,               // Instance, void misc
        0 );

    ShowWindow( *phWnd, nCmdShow );

    // TODO: Handle any errors
    return 0;
}

//-----------------------------------------------------------------------------
//  WndProc
//  Handles Windows messages
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch(nMsg)
    {
    case WM_CREATE:
        {
            return 0;
        }
    case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    case WM_SIZE:
        {

            RECT rcClient;
            GetClientRect( hWnd, &rcClient );
            unsigned int nWidth = rcClient.right - rcClient.left;
            unsigned int nHeight = rcClient.bottom - rcClient.top;

            if( g_pD3D )
            {
                g_pD3D->Resize( nWidth, nHeight );
            }

            return 0;
        }
    case WM_KEYDOWN: 
    case WM_SYSKEYDOWN:
        {
            switch( wParam )
            {
            // Camera Keys
            case MOVE_FORWARD:
                {
                    break;
                }
            case MOVE_BACKWARD:
                {
                    break;
                }
            case MOVE_RIGHT:
                {
                    break;
                }
            case MOVE_LEFT:
                {
                    break;
                }
            // Quit
            case VK_ESCAPE:
                {
                    PostQuitMessage(0);
                }
            }
            return 0;
        }

    case WM_KEYUP:
    case WM_SYSKEYUP:
        {
            break;
        }
    default:
        return DefWindowProc(hWnd, nMsg, wParam, lParam);
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

//-----------------------------------------------------------------------------
//  InitializeGame
//  Initializes all the game specific stuff
//-----------------------------------------------------------------------------
int InitializeGame( void )
{
    // Init Camera
    XMVECTOR vEye = XMVectorSet( 0.0f, 1.75f, -15.0f, 0.0f );
    XMVECTOR vLookAt = XMVectorSet( 0.0f, 1.75f, 0.0f, 0.0f );
    g_Camera.Init( vEye, vLookAt, XM_PIDIV4, gs_nWidth / gs_nHeight, 0.01f, 100000.0f );

    unsigned int hr = 0;
    // Load the vertex shader
    hr = g_pD3D->CreateShader( "DefaultShader.hlsl", "DefaultVertexShader", "vs_4_0", &g_pVertexShader );
    if( hr != 0 )
    {
        // TODO: Handle error more gracefully
        return 1;
    }
    
    // Load the pixel shader
    hr = g_pD3D->CreateShader( "DefaultShader.hlsl", "DefaultPixelShader", "ps_4_0", &g_pPixelShader );
    if( hr != 0 )
    {
        // TODO: Handle error more gracefully
        return 1;
    }
    
    // Create the vertex buffer
    struct Vertex
    {
        float fPos[3];
        float fTexCoord[2];
    };

    Vertex pVertices[] =
    {
        { { 0.0f,          0.0f, gs_fWorldSize }, { 0.0f,          0.0f }, }, 
        { { gs_fWorldSize, 0.0f, gs_fWorldSize }, { gs_fWorldSize, 0.0f }, }, 
        { { gs_fWorldSize, 0.0f, 0.0f },          { gs_fWorldSize, gs_fWorldSize }, }, 
        { { 0.0f,          0.0f, 0.0f },          { 0.0f,          gs_fWorldSize }, }, 
    };
    int nNumVertices = sizeof( pVertices ) / sizeof( Vertex );

    unsigned short pIndices[] = 
    { 
        0, 2, 3,
        0, 1, 2
    };
    int nNumIndices = sizeof( pIndices ) / sizeof( unsigned short );

    hr = g_pD3D->CreateMesh( pVertices, sizeof( Vertex ), nNumVertices, pIndices, nNumIndices, &g_pMesh );

    // Create the texture
    char szTexture[] = "default_texture.png";
    hr = D3DX11CreateShaderResourceViewFromFile( g_pD3D->GetDevice(), szTexture, NULL, NULL, &g_pTexture, NULL );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        char szError[256] = { 0 };
        sprintf( szError, "Could not load texture: %s", szTexture );
        MessageBox( 0, szError, "Error", 0 );
        return hr;
    }

    // Create the sampler
    D3D11_SAMPLER_DESC samplerDesc;
    memset( &samplerDesc, 0, sizeof(samplerDesc) );
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = 16;
    hr = g_pD3D->GetDevice()->CreateSamplerState( &samplerDesc, &g_pSampler );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        MessageBox( 0, "Could not create sampler", "Error", 0 );
        return hr;
    }

    // Create the ground plane object
    g_pGroundPlane = new RiotObject;
    g_pGroundPlane->_Create( g_pMesh, g_pVertexShader, g_pPixelShader, g_pTexture );
    XMMATRIX mWorld = XMMatrixTranslation( -gs_fWorldSize/2, 0.0f, -gs_fWorldSize/2 );
    g_pGroundPlane->SetWorldMatrix( mWorld );

    return hr;
}
