/*********************************************************\
File:       Riot.cpp
Purpose:    Definition of the main engine
\*********************************************************/
#include "Common.h"
#include "Riot.h"
#include "Timer.h"
#include <stdio.h> // For printf
#include "Window.h"
#include "Gfx\Graphics.h"
#include "Scene\SceneGraph.h"
#include "Scene\Object.h"
#include "Gfx\View.h"
#include "Gfx\Material.h"
#include "Scene\ComponentManager.h"
#include "Scene\ObjectManager.h"

#include <stdlib.h>

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

uint                Riot::m_nFrameCount     = 0;
float               Riot::m_fElapsedTime    = 0.0f;
float               Riot::m_fRunningTime    = 0.0f;
RiotInput*          Riot::m_pInput          = NULL;
CWindow*            Riot::m_pMainWindow     = NULL;
CGraphics*          Riot::m_pGraphics       = NULL;
CSceneGraph*        Riot::m_pSceneGraph     = NULL;
CComponentManager*  Riot::m_pComponentManager = NULL;
CObjectManager*     Riot::m_pObjectManager  = NULL;

bool                Riot::m_bRunning        = true;

float GetRand( void )
{
    float f = rand();
    f *= 2;
    f /= RAND_MAX*2;
    f -= 1.0f;
    return f;
}

//-----------------------------------------------------------------------------
//  CreateBox
//  Creates a box
//-----------------------------------------------------------------------------
void CreateBox( void )
{
    CGraphics* pGfx = Riot::GetGraphics();
    CObjectManager* pObjMgr = CObjectManager::GetInstance()->GetInstance();

    // Get our objects
    uint nObject = pObjMgr->CreateObject();
    CObject* pBox = pObjMgr->GetObject( nObject ); //new CObject(); // Considering objects are now statically sized, maybe we shouldn't new them?
    CMesh*   pMesh = pGfx->CreateMesh( 0 );
    CMaterial* pMaterial = pGfx->CreateMaterial( L"Assets/Shaders/StandardVertexShader.hlsl", "PS" );

    // Apply them to the main object
    pBox->SetMesh( pMesh );
    pBox->SetMaterial( pMaterial );
    CSceneGraph::GetInstance()->AddObject( pBox );

    // Add some components
    pObjMgr->AddComponent( nObject, eComponentUpdate );
    pObjMgr->AddComponent( nObject, eComponentRender );
    Transform t = { XMVectorSet( 1.0f, -2.0f, 0.0f, 0.0f ), XMQuaternionRotationRollPitchYaw( 27.5f * rand(), -82.7f * rand(), 413.7f * rand() ) };
    CComponentManager::GetInstance()->SendMessage( eComponentMessageTransform, pBox, &t  );

    //////////////////////////////////////////
    // Create one light
    Transform t2 = { XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ), XMVectorSet( GetRand(), GetRand(), GetRand(), 0.0f ) };
    nObject = pObjMgr->CreateObject();
    pObjMgr->AddComponent( nObject, eComponentLight );
    CComponentManager::GetInstance()->SendMessage( eComponentMessageTransform, pObjMgr->GetObject(nObject), &t2  );
}
    
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

    // Add a box
    CreateBox();

    Timer timer; // TODO: Should the timer be a class member?
    timer.Reset();
    float fFPSTime = 0.0f; // TODO: What's the best way to calculate FPS?
    float fFPS = 0.0f;
    //-----------------------------------------------------------------------------
    while( m_bRunning )
    {
        static uint nObject = -1;
        //---------------------- Start of frame --------------------
        // pMessageSystem->ProcessMessages();
        // pSceneGraph->StartFrame();
        // pRender->StartFrame();
        m_pInput->PollInput();
        if( m_pInput->IsKeyDown( VK_ESCAPE ) )
            m_bRunning = false;

        // Add a box everytime UP arrow is pressed
        if( m_pInput->WasKeyPressed( VK_UP ) )
        {
            CreateBox();
        }
        if( m_pInput->WasKeyPressed( VK_LEFT ) )
        {
            nObject = rand() % CObjectManager::GetInstance()->GetNumObjects();
            CObjectManager::GetInstance()->RemoveComponent( nObject, eComponentUpdate );
        }
        else if( m_pInput->WasKeyPressed( VK_RIGHT ) )
        {
            if( nObject != -1 )
            {
                CObjectManager::GetInstance()->AddComponent( nObject, eComponentUpdate );
            }
        }
        //-------------------------- Frame -------------------------

        uint x = sizeof( CComponent );

        //////////////////////////////////////////
        // Update
        m_pSceneGraph->UpdateObjects( m_fElapsedTime );


        //////////////////////////////////////////
        // Render
        m_pGraphics->PrepareRender();
        m_pGraphics->Render();
        m_pGraphics->Present();

        //----------------------- End of frame ---------------------
        // Perform system messaging
        m_pMainWindow->ProcessMessages();

        // Perform timing
        ++m_nFrameCount;
        m_fElapsedTime = (float)timer.GetTime();
        if( m_fElapsedTime > 0.5f )
        {   // Protection from huge lapses when debugging
            m_fElapsedTime = 1.0f/60.0f;
        }
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
    CView* pView = new CView();
    m_pSceneGraph->AddView( pView );

    //////////////////////////////////////////
    // Load the managers
    m_pComponentManager = CComponentManager::GetInstance();
    m_pObjectManager = CObjectManager::GetInstance();
}

//-----------------------------------------------------------------------------
//  Shutdown
//  Shuts down and cleans up the engine
//-----------------------------------------------------------------------------
void Riot::Shutdown( void )
{    
    SAFE_RELEASE( m_pInput );
    SAFE_RELEASE( m_pGraphics );
    SAFE_RELEASE( m_pMainWindow );

    //////////////////////////////////////////
    // This is the last thing called
    DumpMemoryLeaks( );
}

//-----------------------------------------------------------------------------
//  GetGraphics
//  Returns the current graphics interface
//-----------------------------------------------------------------------------
CGraphics* Riot::GetGraphics( void )
{
    return m_pGraphics;
}
