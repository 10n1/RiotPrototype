/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/27/2011 3:41:24 PM
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

#include <stdio.h>

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
    };
    const uint          Engine::NumMessagesReceived = ARRAY_LENGTH( MessagesReceived );

    /***************************************\
    | class methods                         |
    \***************************************/

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

            m_pRenderer->DrawDebugBox( RVector3( -10.0f, 3.0f, -10.0f ), RVector3( 10.0f, 25.0f, 10.0f ) );
            //////////////////////////////////////////
            // Update everything
            m_pCamera->Update();
            m_pObjectManager->ProcessComponents();

            // Make sure terrain is the last thing drawn
            m_pTerrain->Render();

            //////////////////////////////////////////
            // Render
            m_pRenderer->Render();

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

            if( fFPSTime > 1.0f )
            {
                printf( "FPS: %d\n", nFPSFrames );
                fFPSTime -= 1.0f;
                nFPSFrames = 0;
            }

            if( m_fElapsedTime > 1.0f )
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
                    bShowDebugSpheres = !bShowDebugSpheres;
                    break;
                }
            }
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

        SAFE_RELEASE( pView );

        //////////////////////////////////////////
        // Create the terrain
        m_pTerrain = new CTerrain();
        m_pTerrain->GenerateTerrain();
        m_pTerrain->CreateMesh();


        //////////////////////////////////////////
        // Create an object
        CMesh* pBox = m_pRenderer->CreateMesh();

        static RQuaternion orientation = RQuatFromAxisAngle( RVector3( RandFloat(1.0f), RandFloat(1.0f), RandFloat(1.0f) ), RandFloat( gs_2PI ) );
        orientation = RQuaternionZero();
        RTransform t = RTransform( orientation, RVector3( 0.0f, 100.0f, 0.0f ) );

        // Add box 1
        uint nObject = m_pObjectManager->CreateObject();

        m_pObjectManager->AddComponent( nObject, eComponentRigidBody );
        m_pObjectManager->AddComponent( nObject, eComponentRender );
        m_pObjectManager->AddComponent( nObject, eComponentCollidable );

        m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t );
        m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, pBox );

        CComponentCollidable::CalculateBoundingSphere( m_pRenderer->GetDefaultMeshData(), 24, nObject );

        // Add more boxes
        for( uint i = 1; i < 16; ++i )
        {
            t.position = RVector3( RandFloat(128.0f) - 64.0f, i * 20.0f + 20.0f, RandFloat(128.0f) - 64.0f );
            nObject = m_pObjectManager->CreateObject();

            m_pObjectManager->AddComponent( nObject, eComponentRigidBody );
            m_pObjectManager->AddComponent( nObject, eComponentRender );
            m_pObjectManager->AddComponent( nObject, eComponentCollidable );

            m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t );
            m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, pBox );
            CComponentCollidable::CalculateBoundingSphere( m_pRenderer->GetDefaultMeshData(), 24, nObject );
        }


        //////////////////////////////////////////
        // Add a light
        t = RTransform( orientation, RVector3( 0.0f, 15.0f, 0.0f ), 0.1f );

        nObject = m_pObjectManager->CreateObject();
        m_pObjectManager->AddComponent( nObject, eComponentLight );
        m_pObjectManager->AddComponent( nObject, eComponentRender );
        m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, pBox );
        m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t  );

        
        //////////////////////////////////////////
        // Add a light
        t = RTransform( orientation, RVector3( 10.0f, 5.0f, 0.0f ), 0.1f );

        nObject = m_pObjectManager->CreateObject();
        m_pObjectManager->AddComponent( nObject, eComponentLight );
        m_pObjectManager->AddComponent( nObject, eComponentRender );
        m_pObjectManager->SendMessage( eComponentMessageMesh, nObject, pBox );
        m_pObjectManager->SendMessage( eComponentMessageTransform, nObject, &t  );

        SAFE_RELEASE( pBox );

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
        SHUTDOWN_AND_DELETE( m_pMessageDispatcher );
        SHUTDOWN_AND_DELETE( m_pTaskManager );
        System::Shutdown();

        // Finally delete the instance
        SAFE_DELETE( m_pInstance );
    }


} // namespace Riot
