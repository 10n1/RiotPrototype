/*********************************************************\
File:       Riot.cpp
Purpose:    Definition of the main engine
\*********************************************************/
#include "Riot.h"
#include "Timer.h"
#include <stdio.h> // For printf
#include "Window.h"
#include "Gfx\Graphics.h"
#include "Scene\SceneGraph.h"
#include "Scene\Object.h"
#include "Gfx\View.h"
#include "Gfx\Material.h"

#if defined( OS_WINDOWS )
#include "PlatformDependent\Win32Window.h"
#include "Gfx\D3DGraphics.h"
//#include "OpenGLDevice.h"
#elif defined( OS_OSX )
#include "PlatformDependent\OSXWindow.h"
#include "Gfx\GLGraphics.h"
#elif defined( OS_LINUX )
#include "PlatformDependent\LinuxWindow.h"
#include "Gfx\GLGraphics.h"
#endif
#include "Memory.h"

uint            Riot::m_nFrameCount     = 0;
float           Riot::m_fElapsedTime    = 0.0f;
float           Riot::m_fRunningTime    = 0.0f;
RiotInput*      Riot::m_pInput          = NULL;
CWindow*        Riot::m_pMainWindow     = NULL;
CGraphics*      Riot::m_pGraphics       = NULL;
CSceneGraph*    Riot::m_pSceneGraph     = NULL;
CView*          Riot::m_pMainView       = NULL;

bool            Riot::m_bRunning        = true;
    
//-----------------------------------------------------------------------------
//  Run
//  Starts the engine/game. All variables are set programatically
//-----------------------------------------------------------------------------
void Riot::Run( void )
{
    //-----------------------------------------------------------------------------
    // Initialization
    // TODO: Parse command line
    Initialize();

    CObject* pObject = new CObject();
    CMesh*   pMesh = m_pGraphics->CreateMesh( "lol not loading a mesh!" );
    pObject->SetMesh( pMesh );
    CMaterial* pMaterial = new CMaterial();
    CPixelShader* pShader = m_pGraphics->CreatePixelShader( "Assets/Shaders/StandardVertexShader.hlsl", "PS", "ps_5_0" );
    pMaterial->SetPixelShader( pShader );
    pObject->SetMaterial( pMaterial );
    m_pSceneGraph->AddObject( pObject );
    //-----------------------------------------------------------------------------

    Timer timer; // TODO: Should the timer be a class member?
    timer.Reset();
    float fFPSTime = 0.0f; // TODO: What's the best way to calculate FPS?
    float fFPS = 0.0f;
    //-----------------------------------------------------------------------------
    while( m_bRunning )
    {
        //---------------------- Start of frame --------------------
        // pMessageSystem->ProcessMessages();
        // pSceneGraph->StartFrame();
        // pRender->StartFrame();
        m_pInput->PollInput();
        if( m_pInput->IsKeyDown( VK_ESCAPE ) )
            m_bRunning = false;

        //-------------------------- Frame -------------------------

        //////////////////////////////////////////
        // Update
        m_pSceneGraph->UpdateObjects( m_fElapsedTime );


        //////////////////////////////////////////
        // Render

        // Update and set the view matrix
        m_pMainView->UpdateViewMatrix();
        XMMATRIX mView = m_pMainView->GetViewMatrix();
        XMMATRIX mProj = m_pMainView->GetProjMatrix();
        m_pGraphics->SetViewProj( &mView, &mProj );

        m_pGraphics->PrepareRender();
        uint nNumRenderObjects = 0;
        CObject** ppObjects = m_pSceneGraph->GetRenderObjects( &nNumRenderObjects );
        m_pGraphics->Render( ppObjects, nNumRenderObjects );
        m_pGraphics->Present();

        //----------------------- End of frame ---------------------
        // Perform system messaging
        m_pMainWindow->ProcessMessages();

        // Perform timing
        ++m_nFrameCount;
        m_fElapsedTime = (float)timer.GetTime();
        m_fRunningTime += m_fElapsedTime;
        fFPSTime += m_fElapsedTime;
        // Calculate FPS every 16 frames
        if( ( m_nFrameCount & 0xF ) == 0x0 )
        {
            fFPS = 16.0f / fFPSTime;
            fFPSTime = 0.0f;
            printf( "FPS: %f\n", fFPS );
        }
    }
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // Cleanup
    //-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
//  Initialize
//  Initializes the engine. This is called from Run
//-----------------------------------------------------------------------------
void Riot::Initialize( void )
{
    //////////////////////////////////////////
    // Create window
    uint nWindowWidth = 1024,
         nWindowHeight = 768; // TODO: Read in from file

    // Create the new window object...
#if defined( OS_WINDOWS )
    m_pMainWindow = new CWin32Window();
    m_pGraphics = new CD3DGraphics();
#elif defined( OS_OSX )
    m_pMainWindow = new COSXWindow();
    m_pGraphics = new CGLGraphics();
#elif defined( OS_LINUX )
    m_pMainWindow = new CLinuxWindow();
    m_pGraphics = new CGLGraphics();
#endif
    // ...then create the actual window
    m_pMainWindow->CreateMainWindow( nWindowWidth, nWindowHeight );
    // ...and finally the graphics device
    m_pGraphics->Initialize( m_pMainWindow );

    //////////////////////////////////////////
    // Create the input system
    m_pInput = new RiotInput();

    //////////////////////////////////////////
    //  Get the scene graph
    m_pSceneGraph = CSceneGraph::GetInstance();

    //////////////////////////////////////////
    // Create the main view
    m_pMainView = new CView();
}

//-----------------------------------------------------------------------------
//  Shutdown
//  Shuts down and cleans up the engine
//-----------------------------------------------------------------------------
void Riot::Shutdown( void )
{    
    SAFE_RELEASE( m_pMainView );
    SAFE_RELEASE( m_pGraphics );
    SAFE_RELEASE( m_pMainWindow );
    SAFE_RELEASE( m_pInput );

    //////////////////////////////////////////
    // This is the last thing called
    DumpMemoryLeaks( );
}
