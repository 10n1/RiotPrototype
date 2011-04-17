/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/17/2011 3:52:41 PM
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

#define SHUTDOWN_AND_DELETE( Module ) if( Module ) { Module->Shutdown(); delete Module; Module = NULL; }
#define NEW_AND_INITIALIZE( Module, Type ) Module = new Type; Module->Initialize();

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
    CView*              Engine::m_pMainView             = NULL;

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
            // Create a mesh
            CMesh* pMesh = m_pRenderer->CreateMesh();

            static RQuaternion orientation = RQuatFromAxisAngle( RVector3( RandFloat(1.0f), RandFloat(1.0f), RandFloat(1.0f) ), RandFloat( gs_2PI ) );
            orientation = RQuaternionZero();
            RTransform t = RTransform( orientation, RVector3( 0.0f, 0.0f, 0.0f ) ); 
            m_pRenderer->AddCommand( pMesh, t );
            //////////////////////////////////////////
            // Render
            m_pRenderer->Render();

            SAFE_RELEASE( pMesh );

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
            if( m_fElapsedTime > 1.0f )
            {   // Prevent huge lapses in frame rate (when debugging, etc.)
                m_fElapsedTime = 1.0f/60.0f;
            }
            ++m_nFrame;
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
                }
            }
        case mFullscreen:
            {
                break;
            }
        case mResize:
            {
                uint nWidth = msg.nMessage >> 16;
                uint nHeight = msg.nMessage & 0x0000FFFF;

                m_pMainView->SetPerspective( 60.0f, ((float)nWidth)/nHeight, 0.1f, 10000.0f );                

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
    //  GetTaskManager
    //  Returns the task manager
    //-----------------------------------------------------------------------------
    CTaskManager* Engine::GetTaskManager( void )
    {
        return m_pTaskManager;
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
        NEW_AND_INITIALIZE( m_pMessageDispatcher, CMessageDispatcher );
        m_pMessageDispatcher->RegisterListener( Engine::GetInstance(), Engine::MessagesReceived, Engine::NumMessagesReceived );
        NEW_AND_INITIALIZE( m_pInputManager, CInputManager );
        m_pMessageDispatcher->RegisterListener( m_pInputManager, CInputManager::MessagesReceived, CInputManager::NumMessagesReceived );
        NEW_AND_INITIALIZE( m_pRenderer, CRenderer );
        m_pMessageDispatcher->RegisterListener( m_pRenderer, CRenderer::MessagesReceived, CRenderer::NumMessagesReceived );
        
        // New Modules here

        //////////////////////////////////////////
        // Now perform any other initialization
        // Create a window
        m_pMainWindow = System::CreateMainWindow( 1024, 768 );
        // Load the graphics device
        m_pRenderer->CreateGraphicsDevice( m_pMainWindow );
    
        // Create the main view
        m_pMainView = new CView;
        m_pRenderer->SetCurrentView( m_pMainView );
        m_pMainView->Update( 0.0f );

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
        SAFE_RELEASE( m_pMainView );

        //////////////////////////////////////////
        // ...then shutdown and delete all modules
        // New modules here
        SHUTDOWN_AND_DELETE( m_pRenderer );
        SHUTDOWN_AND_DELETE( m_pInputManager );
        SHUTDOWN_AND_DELETE( m_pMessageDispatcher );
        SHUTDOWN_AND_DELETE( m_pTaskManager );
        System::Shutdown();

        // Finally delete the instance
        SAFE_DELETE( m_pInstance );
    }


} // namespace Riot
