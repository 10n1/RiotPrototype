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
RiotObject*         g_pGroundPlane = NULL;

ID3D11SamplerState*         g_pSampler = NULL;

//-----------------------------------------------------------------------------
// Vertex format
struct Vertex
{
    float fPos[3];
    float fTexCoord[2];
};

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
void CreatePlane( float fWidth, float fHeight, float fU, float fV, RiotObject** ppObject );
void CreateBox( float fLength, float fWidth, float fHeight, RiotObject** ppObject );

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

    // Create the plane
    CreatePlane( gs_fWorldSize, gs_fWorldSize, gs_fWorldSize, gs_fWorldSize, &g_pGroundPlane );

    return hr;
}

void CreatePlane( float fWidth, float fHeight, float fU, float fV, RiotObject** ppObject )
{
    unsigned int hr = 0;
    VertexShader*               pVShader = NULL;
    PixelShader*                pPShader = NULL;
    Mesh*                       pMesh =  NULL;
    ID3D11ShaderResourceView*   pTexture = NULL;
    RiotObject*                 pObject = NULL;

    Vertex* pVertices = new Vertex[ 4 ];

    // Set up the vertices
    pVertices[ 0 ].fPos[0] = 0.0f;
    pVertices[ 0 ].fPos[1] = 0.0f;
    pVertices[ 0 ].fPos[2] = 0.0f;
    pVertices[ 0 ].fTexCoord[0] = 0.0f;
    pVertices[ 0 ].fTexCoord[1] = 0.0f;

    pVertices[ 1 ].fPos[0] = 0.0f;
    pVertices[ 1 ].fPos[1] = 0.0f;
    pVertices[ 1 ].fPos[2] = fHeight;
    pVertices[ 1 ].fTexCoord[0] = 0.0f;
    pVertices[ 1 ].fTexCoord[1] = fV;

    pVertices[ 2 ].fPos[0] = fWidth;
    pVertices[ 2 ].fPos[1] = 0.0f;
    pVertices[ 2 ].fPos[2] = 0.0f;
    pVertices[ 2 ].fTexCoord[0] = fU;
    pVertices[ 2 ].fTexCoord[1] = 0.0f;

    pVertices[ 3 ].fPos[0] = fWidth;
    pVertices[ 3 ].fPos[1] = 0.0f;
    pVertices[ 3 ].fPos[2] = fHeight;
    pVertices[ 3 ].fTexCoord[0] = fU;
    pVertices[ 3 ].fTexCoord[1] = fV;

    // Set up the indices
    unsigned short *pIndices = new unsigned short[6];
    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 1;
    pIndices[4] = 3;
    pIndices[5] = 2;

    // Create the mesh
    hr = g_pD3D->CreateMesh( pVertices, sizeof( Vertex ), 4, pIndices, 6, &pMesh );

    // Load the vertex shader
    hr = g_pD3D->CreateShader( &pVShader );
    
    // Load the pixel shader
    hr = g_pD3D->CreateShader( &pPShader );

    // Create the texture
    char szTexture[] = "default_texture.png";
    hr = D3DX11CreateShaderResourceViewFromFile( g_pD3D->GetDevice(), szTexture, NULL, NULL, &pTexture, NULL );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        char szError[256] = { 0 };
        sprintf( szError, "Could not load texture: %s", szTexture );
        MessageBox( 0, szError, "Error", 0 );
    }
    
    // Create the ground plane object
    pObject = new RiotObject;
    pObject->_Create( pMesh, pVShader, pPShader, pTexture );

    XMMATRIX mWorld = XMMatrixTranslation( -gs_fWorldSize/2, 0.0f, -gs_fWorldSize/2 );
    pObject->SetWorldMatrix( mWorld );

    // Return it
    *ppObject = pObject; 
}

void CreateBox( float fLength, float fWidth, float fHeight, RiotObject** ppObject )
{
}
