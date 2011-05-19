/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/19/2011 4:31:55 PM
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

            //////////////////////////////////////////
            // Render
            m_pRenderer->Render( m_pTerrain );

            //////////////////////////////////////////
            // Make sure terrain is the last thing drawn
            //m_pTerrain->Render();

#if PIPELINED_RENDER
            m_pTaskManager->WaitForCompletion( nObjectUpdateHandle );
#endif // #if PIPELINED_RENDER

            //m_pRenderer->DrawDebugRay( RVector3( 0.0f, 0.0f, 0.0f ), RVector3( 0.0f, 10.0f, 0.0f ) );

            //////////////////////////////////////////
            // Update terrain

            if( bUpdateTerrain )
            {
                m_pTerrain->CenterTerrain( m_pCamera->GetPosition(), 250.0f );
            }

            m_pRenderer->DrawDebugBox( RAABB( RVector3( -10.0f, 0.0f, 0.0f ), RVector3( 10.0f, 10.0f, 0.0f ) ), RVector3( 0.4f, 1.0f, 0.7f ) );

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
                if( nFPSFrames == 32 )
                {
                    float fFPS = (1.0f/fFPSTime) * 32.0f;
                    sprintf( szFPS, "FPS: %f", fFPS );
                    printf( "FPS: %f\n", fFPS );

                    fFPSTime = 0.0f;
                    nFPSFrames = 0;
                }
                m_pRenderer->DrawString( 0, 0, szFPS );
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
                        static bool b = false;
                        static uint nObject = 0;

                        if( b == false )
                        {
                            m_pObjectManager->RemoveComponent( nObject, eComponentRender );
                            b = !b;
                        }
                        else
                        {
                            m_pObjectManager->AddComponent( nObject, eComponentRender );
                            b = !b;
                            nObject = Rand() % 10;
                        }
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
                    static uint nCount = 1;
                    static sint nBox = m_pRenderer->CreateMesh();
                    RTransform t = RTransform();
                    t.position = RVector3( RandFloat(16.0f), 150.0f , RandFloat(16.0f) );
                    //t.position = RVector3( 0.0f, i * 30.0f + 20.0f, 0.0f );
                    //t.position = RVector3( RandFloat( 2.0f ), 150.0f, RandFloat( 2.0f ) );
                    uint nObject = m_pObjectManager->CreateObject();

                    m_pObjectManager->AddComponent( nObject, eComponentRigidBody );
                    m_pObjectManager->AddComponent( nObject, eComponentRender );
                    m_pObjectManager->AddComponent( nObject, eComponentCollidable );
                    uint nCollidableIndex = m_pObjectManager->GetComponentIndex( nObject, eComponentCollidable );
                    CComponentCollidable::CalculateBoundingSphere( m_pRenderer->GetDefaultMeshData(), 24, nCollidableIndex );

                    m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t );
                    m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, nBox );
                    nCount++;
                    printf( "Objects: %d\n", nCount );
                    break;
                }
            }
        case mKeyDown:
            switch( msg.nMessage )
            {
            case KEY_G:
                {
                    static uint nCount = 1;
                    static sint nBox = m_pRenderer->CreateMesh();
                    RTransform t = RTransform();

                    for( uint i = 0; i < 24; ++i )
                    {
                        t.position = RVector3( RandFloat(128.0f) - 64.0f, RandFloat( 1024.0f ) + 65.0f, RandFloat(128.0f) - 64.0f );
                        //t.position = RVector3( 0.0f, i * 30.0f + 20.0f, 0.0f );
                        uint nObject = m_pObjectManager->CreateObject();

                        if( nObject == -1 )
                        {
                            break;
                        }

                        m_pObjectManager->AddComponent( nObject, eComponentRigidBody );
                        m_pObjectManager->AddComponent( nObject, eComponentRender );
                        m_pObjectManager->AddComponent( nObject, eComponentCollidable );
                        uint nCollidableIndex = m_pObjectManager->GetComponentIndex( nObject, eComponentCollidable );
                        CComponentCollidable::CalculateBoundingSphere( m_pRenderer->GetDefaultMeshData(), 24, nCollidableIndex );

                        m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t );
                        m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, nBox );
                    }
                    nCount++;
                    printf( "Objects: %d\n", m_pObjectManager->GetNumObjects() );
                }
                break;

            case KEY_H:
                {
                    for( uint i = 0; i < 20; ++i )
                    {
                        uint nObject = m_pObjectManager->GetNumObjects();
                        if( nObject > 0 )
                            m_pObjectManager->DeleteObject( nObject-1 );
                    }

                    printf( "Objects: %d\n", m_pObjectManager->GetNumObjects() );
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
        CComponentCollidable::SetTerrain( m_pTerrain );

        //////////////////////////////////////////
        // Create an object
        sint nBox = m_pRenderer->CreateMesh();

        static RQuaternion orientation = RQuatFromAxisAngle( RVector3( RandFloat(1.0f), RandFloat(1.0f), RandFloat(1.0f) ), RandFloat( gs_2PI ) );
        orientation = RQuaternionZero();
        RTransform t = RTransform( orientation, RVector3( 0.0f, 50.0f, 0.0f ) );

        //////////////////////////////////////////
        // Add a light
        t = RTransform( orientation, RVector3( 0.0f, 200.0f, 0.0f ), 0.1f );

        uint nObject = m_pObjectManager->CreateObject();
        m_pObjectManager->AddComponent( nObject, eComponentLight );
        m_pObjectManager->AddComponent( nObject, eComponentRender );
        m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, nBox );
        m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t  );

        
        //////////////////////////////////////////
        // Add a light
        //t = RTransform( orientation, RVector3( 10.0f, 5.0f, 0.0f ), 0.1f );
        //
        //nObject = m_pObjectManager->CreateObject();
        //m_pObjectManager->AddComponent( nObject, eComponentLight );
        //m_pObjectManager->AddComponent( nObject, eComponentRender );
        //m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, pBox );
        //m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t  );

        //SAFE_RELEASE( pBox );

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


} // namespace Riot
