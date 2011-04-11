/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 5:45:54 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Engine.h"
#include "TaskManager.h"
#include "MessageDispatcher.h"
#include "Window.h"
#include "System.h"

#define SHUTDOWN_AND_DELETE( Module ) if( Module ) { Module->Shutdown(); delete Module; Module = NULL; }
#define NEW_AND_INITIALIZE( Module, Type ) Module = new Type; Module->Initialize();

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    Engine*             Engine::m_pInstance             = NULL;
    CTaskManager*       Engine::m_pTaskManager          = NULL;
    CMessageDispatcher* Engine::m_pMessageDispatcher    = NULL;
    CWindow*            Engine::m_pMainWindow           = NULL;

    bool                Engine::m_bRunning              = true;

    const MessageType   Engine::MessagesReceived[] = 
    {
        mShutdown,
        mHardwareKeyboard,
        mFullscreen,
        mResize,
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
            
            //////////////////////////////////////////
            //  Process OS messages
            m_pMainWindow->ProcessMessages();

            //////////////////////////////////////////
            //  Process this frames messages
            m_pMessageDispatcher->ProcessMessages();
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
        case mHardwareKeyboard:
            {
                switch( msg.nMessage )
                {
                case KEY_ESCAPE:
                    {
                        SendMsg( mShutdown );
                        break;
                    }
                case KEY_ENTER:
                    {
                        break;
                    }
                    case KEY_LEFT:
                    {
                        int x = 0;
                        
                        break;
                    }
                }
                break;
            }
        case mFullscreen:
            {
                break;
            }
        case mResize:
            {
                uint nWidth = msg.nMessage >> 16;
                uint nHeight = msg.nMessage & 0x0000FFFF;

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
        // New Modules here

        //////////////////////////////////////////
        // Now perform any other initialization

        // Create a window
        m_pMainWindow = System::CreateMainWindow( 1024, 768 );
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

        //////////////////////////////////////////
        // ...then shutdown and delete all modules
        // New modules here
        SHUTDOWN_AND_DELETE( m_pMessageDispatcher );
        SHUTDOWN_AND_DELETE( m_pTaskManager );
        System::Shutdown();
    }


} // namespace Riot
