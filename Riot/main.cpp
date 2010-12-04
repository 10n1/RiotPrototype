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
static const unsigned int gs_nWidth = 1024;
static const unsigned int gs_nHeight = 768;
static const float        gs_fWorldSize = 1000.0f;
static const unsigned int gs_nCitySize = 8;
static const unsigned int gs_nCitySizeSq = gs_nCitySize*gs_nCitySize;
static const char         gs_szName[] = "3D World";
static const bool         gs_bFullscreen = false;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p) { p->Release(); p = NULL; } 
#endif

//-----------------------------------------------------------------------------
// Variables

XMMATRIX            g_mWorld;
CTimer              g_Timer;
HWND                g_hWnd = NULL;
Direct3DDevice*     g_pD3D = NULL;
Camera              g_Camera;
bool                g_bRunning = true;

// Ground plane
RiotObject*         g_pGroundPlane = NULL;
RiotObject*         g_pBuildings[gs_nCitySize*gs_nCitySize] = { NULL };

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
void CreateCity( int nWidth, int nHeight );

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
        g_Camera.Update( fElapsedTime );
        XMMATRIX mViewProj = XMMatrixTranspose( g_Camera.GetViewProj() );
        g_pD3D->SetViewProjMatrix( mViewProj );
        g_pD3D->GetContext()->PSSetSamplers( 0, 1, &g_pSampler );
        g_pGroundPlane->Render();
        for( int i = 0; i < gs_nCitySizeSq; ++i )
        {
            g_pBuildings[i]->Render();
        }

        g_pD3D->Render();
        //-----------------------------------------------------------------------------
        fElapsedTime = (float)g_Timer.GetTime();
        nFrameCount++;
        if( nFrameCount == 1000 )
        {
            char szTitle[256];
            sprintf_s( szTitle, 256, "FrameTime = %f ms", fElapsedTime * 1000.0f );
            // TODO: Add real text rendering, SetWindowText isn't a good idea...
            SetWindowText( g_hWnd, szTitle );
            nFrameCount = 0;
        }
    }

    
    /////////////////////////////////////
    // Perform clean up
    SAFE_DELETE( g_pGroundPlane );
    for( int i = 0; i < gs_nCitySizeSq; ++i )
    {
        SAFE_DELETE( g_pBuildings[i] );
    }
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
                    g_Camera.KeyInput( MOVE_FORWARD );
                    break;
                }
            case MOVE_BACKWARD:
                {
                    g_Camera.KeyInput( MOVE_BACKWARD );
                    break;
                }
            case MOVE_RIGHT:
                {
                    g_Camera.KeyInput( MOVE_RIGHT );
                    break;
                }
            case MOVE_LEFT:
                {
                    g_Camera.KeyInput( MOVE_LEFT );
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
    XMVECTOR vEye = XMVectorSet( 0.0f, 1.75f, -100.0f, 0.0f );
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
    g_mWorld = XMMatrixTranslation( -gs_fWorldSize/2, 0.0f, -gs_fWorldSize/2 );
    g_pGroundPlane->SetWorldMatrix( g_mWorld );

    // Create the city scape
    CreateCity( gs_nCitySize, gs_nCitySize );

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
    pVertices[ 1 ].fTexCoord[1] = fV/2;

    pVertices[ 2 ].fPos[0] = fWidth;
    pVertices[ 2 ].fPos[1] = 0.0f;
    pVertices[ 2 ].fPos[2] = 0.0f;
    pVertices[ 2 ].fTexCoord[0] = fU/2;
    pVertices[ 2 ].fTexCoord[1] = 0.0f;

    pVertices[ 3 ].fPos[0] = fWidth;
    pVertices[ 3 ].fPos[1] = 0.0f;
    pVertices[ 3 ].fPos[2] = fHeight;
    pVertices[ 3 ].fTexCoord[0] = fU/2;
    pVertices[ 3 ].fTexCoord[1] = fV/2;

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
    
    delete [] pIndices;
    delete [] pVertices;

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
    pObject = new RiotObject();
    pObject->_Create( pMesh, pVShader, pPShader, pTexture );

    // Return it
    *ppObject = pObject; 
}

void CreateBox( float fLength, float fWidth, float fHeight, RiotObject** ppObject )
{
    
    unsigned int hr = 0;
    VertexShader*               pVShader = NULL;
    PixelShader*                pPShader = NULL;
    Mesh*                       pMesh =  NULL;
    ID3D11ShaderResourceView*   pTexture = NULL;
    RiotObject*                 pObject = NULL;

    Vertex* pVertices = new Vertex[ 8 ];

    float fXMin = -fLength/2;
    float fXMax = fLength/2;
    float fZMin = -fWidth/2;
    float fZMax = fWidth/2;

    // Set up the vertices
    // Bottom plane
    pVertices[ 0 ].fPos[0] = fXMin;
    pVertices[ 0 ].fPos[1] = 0.0f;
    pVertices[ 0 ].fPos[2] = fZMin;
    pVertices[ 0 ].fTexCoord[0] = fXMin/2;
    pVertices[ 0 ].fTexCoord[1] = fZMin/2;

    pVertices[ 1 ].fPos[0] = fXMin;
    pVertices[ 1 ].fPos[1] = 0.0f;
    pVertices[ 1 ].fPos[2] = fZMax;
    pVertices[ 1 ].fTexCoord[0] = fXMin/2;
    pVertices[ 1 ].fTexCoord[1] = fZMax/2;

    pVertices[ 2 ].fPos[0] = fXMax;
    pVertices[ 2 ].fPos[1] = 0.0f;
    pVertices[ 2 ].fPos[2] = fZMin;
    pVertices[ 2 ].fTexCoord[0] = fXMax/2;
    pVertices[ 2 ].fTexCoord[1] = fZMin/2;

    pVertices[ 3 ].fPos[0] = fXMax;
    pVertices[ 3 ].fPos[1] = 0.0f;
    pVertices[ 3 ].fPos[2] = fZMax;
    pVertices[ 3 ].fTexCoord[0] = fXMax/2;
    pVertices[ 3 ].fTexCoord[1] = fZMax/2;

    pVertices[ 4 ].fPos[0] = fXMin;
    pVertices[ 4 ].fPos[1] = fHeight;
    pVertices[ 4 ].fPos[2] = fZMin;
    pVertices[ 4 ].fTexCoord[0] = fXMin/2;
    pVertices[ 4 ].fTexCoord[1] = fZMin/2;
    
    pVertices[ 5 ].fPos[0] = fXMin;
    pVertices[ 5 ].fPos[1] = fHeight;
    pVertices[ 5 ].fPos[2] = fZMax;
    pVertices[ 5 ].fTexCoord[0] = fXMin/2;
    pVertices[ 5 ].fTexCoord[1] = fZMax/2;
    
    pVertices[ 6 ].fPos[0] = fXMax;
    pVertices[ 6 ].fPos[1] = fHeight;
    pVertices[ 6 ].fPos[2] = fZMin;
    pVertices[ 6 ].fTexCoord[0] = fXMax/2;
    pVertices[ 6 ].fTexCoord[1] = fZMin/2;
    
    pVertices[ 7 ].fPos[0] = fXMax;
    pVertices[ 7 ].fPos[1] = fHeight;
    pVertices[ 7 ].fPos[2] = fZMax;
    pVertices[ 7 ].fTexCoord[0] = fXMax/2;
    pVertices[ 7 ].fTexCoord[1] = fZMax/2;

    // Set up the indices
    unsigned short *pIndices = new unsigned short[36];
    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 1;
    pIndices[4] = 3;
    pIndices[5] = 2;

    pIndices[6] = 4;
    pIndices[7] = 5;
    pIndices[8] = 6;
              
    pIndices[9] = 5;
    pIndices[10] = 7;
    pIndices[11] = 6;

    pIndices[12] = 0;
    pIndices[13] = 4;
    pIndices[14] = 2;
    
    pIndices[15] = 4;
    pIndices[16] = 6;
    pIndices[17] = 2;
    
    pIndices[18] = 0;
    pIndices[19] = 1;
    pIndices[20] = 5;

    pIndices[21] = 5;
    pIndices[22] = 4;
    pIndices[23] = 0;
        
    pIndices[24] = 3;
    pIndices[25] = 7;
    pIndices[26] = 1;
    
    pIndices[27] = 7;
    pIndices[28] = 5;
    pIndices[29] = 1;
    
    pIndices[30] = 2;
    pIndices[31] = 6;
    pIndices[32] = 3;
    
    pIndices[33] = 6;
    pIndices[34] = 7;
    pIndices[35] = 3;
    
    // Create the mesh
    hr = g_pD3D->CreateMesh( pVertices, sizeof( Vertex ), 8, pIndices, 36, &pMesh );

    delete [] pIndices;
    delete [] pVertices;

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
    pObject = new RiotObject();
    pObject->_Create( pMesh, pVShader, pPShader, pTexture );

    // Return it
    *ppObject = pObject; 
}

void CreateCity( int nWidth, int nHeight )
{
    static const float fBlockSize = 100.0f;
    static const float fLaneWidth = 3.66f;
    static const float fSidewalkWidth = 2.0f;
    static const float fStoryHeight = 3.96f;

    int nBuilding = 0;
    for( int x = -nWidth/2; x < 0; ++x )
    {
        for( int z = -nHeight/2; z < 0; ++z )
        {
            CreateBox( fBlockSize, fStoryHeight*5.0f, fBlockSize, &g_pBuildings[nBuilding] );

            float fXPos = x * fBlockSize/2.0f + (x*fSidewalkWidth*2-1) + (x*fLaneWidth*4-2);
            float fZPos = z * fBlockSize/2.0f + (z*fSidewalkWidth*2-1) + (z*fLaneWidth*4-2);
            g_mWorld = XMMatrixTranslation( fXPos, 0.0f, fZPos );
            g_pBuildings[nBuilding++]->SetWorldMatrix( g_mWorld );
        }
    }

    for( int x = -nWidth/2; x < 0; ++x )
    {
        for( int z = 1; z <= nHeight/2; ++z )
        {
            CreateBox( fBlockSize, fStoryHeight*5.0f, fBlockSize, &g_pBuildings[nBuilding] );

            float fXPos = x * fBlockSize/2.0f + (x*fSidewalkWidth*2-1) + (x*fLaneWidth*4-2);
            float fZPos = z * fBlockSize/2.0f + (z*fSidewalkWidth*2-1) + (z*fLaneWidth*4-2);
            g_mWorld = XMMatrixTranslation( fXPos, 0.0f, fZPos );
            g_pBuildings[nBuilding++]->SetWorldMatrix( g_mWorld );
        }
    }

    for( int x = 1; x <= nWidth/2; ++x )
    {
        for( int z = -nHeight/2; z < 0; ++z )
        {
            CreateBox( fBlockSize, fStoryHeight*5.0f, fBlockSize, &g_pBuildings[nBuilding] );

            float fXPos = x * fBlockSize/2.0f + (x*fSidewalkWidth*2-1) + (x*fLaneWidth*4-2);
            float fZPos = z * fBlockSize/2.0f + (z*fSidewalkWidth*2-1) + (z*fLaneWidth*4-2);
            g_mWorld = XMMatrixTranslation( fXPos, 0.0f, fZPos );
            g_pBuildings[nBuilding++]->SetWorldMatrix( g_mWorld );
        }
    }

    for( int x = 1; x <= nWidth/2; ++x )
    {
        for( int z = 1; z <= nHeight/2; ++z )
        {
            CreateBox( fBlockSize, fStoryHeight*5.0f, fBlockSize, &g_pBuildings[nBuilding] );

            float fXPos = x * fBlockSize/2.0f + (x*fSidewalkWidth*2-1) + (x*fLaneWidth*4-2);
            float fZPos = z * fBlockSize/2.0f + (z*fSidewalkWidth*2-1) + (z*fLaneWidth*4-2);
            g_mWorld = XMMatrixTranslation( fXPos, 0.0f, fZPos );
            g_pBuildings[nBuilding++]->SetWorldMatrix( g_mWorld );
        }
    }
}
