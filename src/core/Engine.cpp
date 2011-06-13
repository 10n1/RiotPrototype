/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/29/2011 1:23:57 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Engine.h"
#include "TaskManager.h"
#include "MessageDispatcher.h"
#include "Window.h"
#include "System.h"
#include "timer.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Mesh.h"
#include "View.h"
#include "ObjectManager.h"
#include "Terrain.h"
#include "Camera.h"
#include "Console.h"
#include "Character.h"
#include "object.h"
#include "rendersystem.h"
#include "collisionsystem.h"
#include "physicssystem.h"
#include "UI.h"
#include "Socket.h"

#include <stdio.h> // included for printf

#define SHUTDOWN_AND_DELETE( Module ) if( Module ) { Module->Shutdown(); delete Module; Module = NULL; }
#define NEW_AND_INITIALIZE( Module, Type ) Module = new Type; Module->Initialize();

#define NEW_AND_INITIALIZE_AND_REGISTER( Module, Type ) \
    Module = new Type;                              \
    ASSERT( Module );                               \
    Module->Initialize();                           \
    m_pMessageDispatcher->RegisterListener( Module, Type::MessagesReceived, Type::NumMessagesReceived )

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    CTimer              Engine::m_MainTimer;
    Engine*             Engine::m_pInstance             = NULL;
    CTaskManager*       Engine::m_pTaskManager          = NULL;
    CMessageDispatcher* Engine::m_pMessageDispatcher    = NULL;
    CWindow*            Engine::m_pMainWindow           = NULL;
    CInputManager*      Engine::m_pInputManager         = NULL;
    CRenderer*          Engine::m_pRenderer             = NULL;
    CCamera*            Engine::m_pCamera               = NULL;
    CObjectManager*     Engine::m_pObjectManager        = NULL;
    CConsole*           Engine::m_pConsole              = NULL;
    CCharacter*         Engine::m_pCharacters[MAX_CHARACTERS];

    CTerrain*           Engine::m_pTerrain              = NULL;

    float               Engine::m_fElapsedTime          = 0.0f;
    uint                Engine::m_nFrame                = 0;
    bool                Engine::m_bRunning              = true;

    const MessageType   Engine::MessagesReceived[] = 
    {
        mShutdown,
        mFullscreen,
        mResize,
        mKeyPressed,
        mKeyDown,
    };
    const uint          Engine::NumMessagesReceived = ARRAY_LENGTH( MessagesReceived );

    /***************************************\
    | class methods                         |
    \***************************************/

    static bool bUpdateTerrain = true;

    //-----------------------------------------------------------------------------
    //  GetInstance
    //  Returns the global instance
    //-----------------------------------------------------------------------------
    Engine* Engine::GetInstance( void )
    {
        if( m_pInstance == NULL )
        {
            m_pInstance = new Engine();
        }

        return m_pInstance;
    }

    //-----------------------------------------------------------------------------
    //  Run
    //  Starts the engine/game. All variables are set programatically
    //-----------------------------------------------------------------------------
    void Engine::Run( void )
    {
        //////////////////////////////////////////
        // First thing, start up the engine
        Initialize();

        //////////////////////////////////////////
        // Run the engine
        while( m_bRunning )
        {
            m_pRenderer->SwapBuffers();

            //////////////////////////////////////////
            // Update everything
            m_pCamera->Update();
#if PIPELINED_RENDER
            task_handle_t nObjectUpdateHandle =  m_pTaskManager->PushTask( CObjectManager::PipelineObjectUpdate, m_pObjectManager, 1, 1 );
#else
            m_pObjectManager->PipelineObjectUpdate( m_pObjectManager, 0, 0, 1 );
#endif // #if PIPELINED_RENDER

            m_pRenderer->AddPointLight( RVector3( 0.0f, 200.0f, 0.0f ), RVector3( 1.0f, 1.0f, 1.0f ), 2000.0f );

            CRenderSystem::ProcessObjects();
            CPhysicsSystem::ProcessObjects();
            CCollisionSystem::ProcessObjects();

            m_pConsole->Render();

            //////////////////////////////////////////
            // Render
            m_pRenderer->Render( m_pTerrain );

#if PIPELINED_RENDER
            m_pTaskManager->WaitForCompletion( nObjectUpdateHandle );
#endif // #if PIPELINED_RENDER

            //////////////////////////////////////////
            // Update terrain
            if( bUpdateTerrain )
            {
                m_pTerrain->CenterTerrain( m_pCamera->GetPosition(), 250.0f );
            }

            //////////////////////////////////////////
            //  Process OS messages
            System::ProcessOSMessages();

            //////////////////////////////////////////
            // Update input
            m_pInputManager->ProcessInput();

            //////////////////////////////////////////
            //  Process this frames messages
            m_pMessageDispatcher->ProcessMessages();
            
            
            //////////////////////////////////////////
            // Network test
            CSocket out;
            CSocket in;
            byte pData[512] = "LOL!!!";
            byte pInData[512] = "SADFACETIMELOL!!!";
            out.OpenSocket( 30000 );
            in.OpenSocket( 30000 );
            
            out.SendData( pData, sizeof( pData ), BuildAddress( 127, 0, 0, 1), 30000 );
            in.ReceiveData( pInData, sizeof( pInData ) );
            
            in.CloseSocket();
            out.CloseSocket();


            //////////////////////////////////////////
            // Perform end of frame timing, etc.
            m_fElapsedTime = (float)m_MainTimer.GetElapsedTime();
            ++m_nFrame;

            static float fFPSTime = 0.0f;
            static uint  nFPSFrames = 0;

            fFPSTime += m_fElapsedTime;
            nFPSFrames++;

            if( gnShowFPS )
            {
                static char szFPS[ 255 ] = "FPS: 0";
                if( nFPSFrames == 512 )
                {
                    float fFPS = (1.0f/fFPSTime) * 512;
                    sprintf( szFPS, "FPS: %f", fFPS );
                    //printf( "FPS: %f\n", fFPS );

                    fFPSTime = 0.0f;
                    nFPSFrames = 0;

                    System::Log( szFPS );
                }
                m_pRenderer->DrawString( 200, 0, szFPS );
            }

            if( m_fElapsedTime > 1.0f/60.0f )
            {   // Prevent huge lapses in frame rate (when debugging, etc.)
                m_fElapsedTime = 1.0f/60.0f;
            }
        }

        //////////////////////////////////////////
        // Shutdown
        Shutdown();
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void Engine::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        case mShutdown:
            {
                m_bRunning = false;
                break;
            }
        case mKeyPressed:
            {
                switch( msg.nMessage )
                {
                case KEY_TILDE:
                    gnConsoleActive = !gnConsoleActive;
                    break;
                case KEY_ESCAPE:
                    {
                        m_pMessageDispatcher->SendMsg( mShutdown );
                        break;
                    }
                case KEY_E:
                    {
                        break;
                    }
                case KEY_O:
                    gnShowBoundingVolumes = !gnShowBoundingVolumes;
                    break;
                case KEY_F:
                    gnRenderWireframe = !gnRenderWireframe;
                    break;
                case KEY_R:
                    gnRenderOn = !gnRenderOn;
                    break;     
                case KEY_T:
                    bUpdateTerrain = !bUpdateTerrain;
                    break;               
                case KEY_X:
                    break;
                }
            }
        case mKeyDown:
            switch( msg.nMessage )
            {
            case KEY_G:
                {                    
                }
                break;

            case KEY_H:
                {
                }
                break;
            }
            break;
        case mFullscreen:
            {
                break;
            }
        case mResize:
            {
                break;
            }
        default:
            {
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  PostMsg
    //  Posts a message to be processed later
    //-----------------------------------------------------------------------------
    void Engine::PostMsg( const TMessage& msg ) { m_pMessageDispatcher->PostMsg( msg ); }
    void Engine::PostMsg( MessageType nType ) { m_pMessageDispatcher->PostMsg( nType ); }

    //-----------------------------------------------------------------------------
    //  SendMsg
    //  Sends a message immediately
    //-----------------------------------------------------------------------------
    void Engine::SendMsg( const TMessage& msg ) { m_pMessageDispatcher->SendMsg( msg ); }
    void Engine::SendMsg( MessageType nType ) { m_pMessageDispatcher->SendMsg( nType ); }


    //-----------------------------------------------------------------------------
    //  RegisterListener
    //  Counts number registered for each message
    //-----------------------------------------------------------------------------
    void Engine::RegisterListener( IListener* pListener, const MessageType* pMessages, uint nCount )
    {
        m_pMessageDispatcher->RegisterListener( pListener, pMessages, nCount );
    }
    
    //-----------------------------------------------------------------------------
    //  ButtonTest
    //-----------------------------------------------------------------------------
    void Engine::ButtonTest( void )
    {
        static bool bTest = false;
        
        if( !bTest )
            m_pRenderer->GetGraphicsDevice()->SetClearColor( 1.0f, 0.0f, 0.0f, 1.0f);
        else
            m_pRenderer->GetGraphicsDevice()->SetClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
        
        bTest = !bTest;
            
    }

    //-----------------------------------------------------------------------------
    //  Initialize
    //  Initializes the engine. This is called from Run
    //-----------------------------------------------------------------------------
    void Engine::Initialize( void )
    {
        //////////////////////////////////////////
        // Initialize all modules
        System::Initialize();
        NEW_AND_INITIALIZE( m_pTaskManager, CTaskManager );
        CTaskManager::m_pInstance = m_pTaskManager;
        NEW_AND_INITIALIZE( m_pMessageDispatcher, CMessageDispatcher );
        m_pMessageDispatcher->RegisterListener( Engine::GetInstance(), Engine::MessagesReceived, Engine::NumMessagesReceived );
        NEW_AND_INITIALIZE( m_pConsole, CConsole ); // Console doesn't register for messages
        NEW_AND_INITIALIZE_AND_REGISTER( m_pInputManager, CInputManager );
        NEW_AND_INITIALIZE_AND_REGISTER( m_pRenderer, CRenderer );
        NEW_AND_INITIALIZE_AND_REGISTER( m_pObjectManager, CObjectManager );
        // New Modules here

        //////////////////////////////////////////
        // Now perform any other initialization
        // Create a window
        m_pMainWindow = System::CreateMainWindow( 1024, 768 );
        // Load the graphics device
        m_pRenderer->CreateGraphicsDevice( m_pMainWindow );
        
        m_pMessageDispatcher->RegisterListener( UI::m_pInstance, UI::MessagesReceived, UI::NumMessagesReceived);
        
        UI::AddButton(0, 50, 0, 50, "LOL Button!", ButtonTest );

        // Create the main view and camera
        CView* pView = new CView;
        m_pRenderer->SetCurrentView( pView );

        m_pCamera = new CCamera;
        m_pCamera->SetView( pView );
        m_pMessageDispatcher->RegisterListener( m_pCamera, CCamera::MessagesReceived, CCamera::NumMessagesReceived );

        SAFE_RELEASE( pView );

        //////////////////////////////////////////
        // Create the terrain
        m_pTerrain = new CTerrain();
        m_pTerrain->GenerateTerrain();

        //////////////////////////////////////////
        // Add the character
        m_pCharacters[0] = new CCharacter();
        //m_pRenderer->SetCurrentView( m_pCharacters[0]->GetView() );
        //m_pCamera->SetView( m_pCharacters[0]->GetView() );

        //////////////////////////////////////////
        // Create an object
        CCollisionSystem::Init();
        CCollisionSystem::SetTerrain( m_pTerrain );
        m_pObjectManager->LoadObjectDeclaration( "assets/scripts/renderable.rs" );
        m_pObjectManager->LoadObjectDeclaration( "assets/scripts/rigidbody.rs" );
        m_pObjectManager->LoadObjectDeclaration( "assets/scripts/baseobject.rs" );

        uint nObj = m_pObjectManager->CreateObject( 0, "baseObject" );
        RVector3* v;
        sint* nMesh;
        sint* nTexture;
        CObject o = m_pObjectManager->GetObject( nObj );
        o.GetProperty( "mesh", (void**)&nMesh );
        o.GetProperty( "diffuse", (void**)&nTexture );
        *nMesh = m_pRenderer->CreateMesh();
        *nTexture = 0;
        o.GetProperty( "position", (void**)&v );
        *v = RVector3( 0.0f, 200.0f, 0.0f );
        o.GetProperty( "acceleration", (void**)&v );
        *v = RVector3( 0.0f, -9.8f, 0.0f );

        CCollisionSystem::CalculateBoundingSphere( m_pRenderer->GetDefaultMeshData(), 24, &o );

        nObj = m_pObjectManager->CreateObject( 0, "baseObject" );
        o = m_pObjectManager->GetObject( nObj );
        o.GetProperty( "mesh", (void**)&nMesh );
        o.GetProperty( "diffuse", (void**)&nTexture );
        *nMesh = m_pRenderer->CreateMesh();
        *nTexture = 0;
        o.GetProperty( "position", (void**)&v );
        *v = RVector3( 0.0f, 250.0f, 0.0f );
        o.GetProperty( "acceleration", (void**)&v );
        *v = RVector3( 0.0f, -9.8f, 0.0f );

        CCollisionSystem::CalculateBoundingSphere( m_pRenderer->GetDefaultMeshData(), 24, &o );

        // Picking test
        CObject* pObject = CCollisionSystem::PickObject( m_pCamera->GetPosition(), RVector3( 0.0f, -4.0f, 10.0f ) );
        SAFE_DELETE( pObject );
        

        // Finally reset the timer
        m_MainTimer.Reset();
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //  Shuts down and cleans up the engine
    //-----------------------------------------------------------------------------
    void Engine::Shutdown( void )
    {        
        //////////////////////////////////////////
        // Now perform any shutdown needed
        SAFE_RELEASE( m_pMainWindow );
        SAFE_DELETE( m_pCamera );
        SAFE_DELETE( m_pTerrain );
        SAFE_DELETE( m_pCharacters[0] ); // hmmm...

        //////////////////////////////////////////
        // ...then shutdown and delete all modules
        // New modules here
        SHUTDOWN_AND_DELETE( m_pObjectManager );
        SHUTDOWN_AND_DELETE( m_pRenderer );
        SHUTDOWN_AND_DELETE( m_pInputManager );
        SHUTDOWN_AND_DELETE( m_pConsole );
        SHUTDOWN_AND_DELETE( m_pMessageDispatcher );
        SHUTDOWN_AND_DELETE( m_pTaskManager );
        System::Shutdown();

        // Finally delete the instance
        SAFE_DELETE( m_pInstance );
    }


    //////////////////////////////////////////
    void Engine::IntegrateDynamics( CObject* pObject, float dt )
    {
        RVector3* pos0;
        RVector3* vel0;
        RVector3* acc0;

        pObject->GetProperty( "position", (void**)&pos0 );
        pObject->GetProperty( "velocity", (void**)&vel0 );
        pObject->GetProperty( "acceleration", (void**)&acc0 );

        RVector3 vel05;
        RVector3 pos1;
        RVector3 vel1;
        RVector3 acc1;

        //  1. Calcualte new position
        pos1    = *pos0 + *vel0*dt + (0.5f * *acc0 * Square(dt));

        //  2. Calculate 1/2 of the new velocity
        vel05   = *vel0 + 0.5f * *acc0 * dt;

        //  3. Calculate the new acceleration
        acc1    = *acc0; // force / m;

        //  4. Calculate the other half of the new velocity
        vel1    = vel05 + 0.5 * acc1 * dt;

        *pos0 = pos1;
        *vel0 = vel1;
        *acc0 = acc1;
    }
    void Engine::NormalRender( CObject* pObject, float dt )
    {
        sint* nMesh;
        sint* nTex;
        RVector3* vPos;
        pObject->GetProperty( "mesh", (void**)&nMesh );
        pObject->GetProperty( "diffuse", (void**)&nTex );
        pObject->GetProperty( "position", (void**)&vPos );

        TRenderCommand cmd;
        RTransform t;
        cmd.m_nMesh = *nMesh;
        cmd.m_nTexture = *nTex;
        t.position = *vPos;
        
        m_pRenderer->AddCommand( cmd.Encode(), t );
    }

} // namespace Riot
