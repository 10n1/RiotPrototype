/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 4:06:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Engine.h"
#include "TaskManager.h"
#include "MessageDispatcher.h"

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
    bool                Engine::m_bRunning              = true;

    const MessageType   Engine::MessagesReceived[] = 
    {
        mShutdown,
        mKeyboard,
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
            TMessage msg = { mShutdown, 0 };
            m_pMessageDispatcher->PostMsg( msg );

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
        case mKeyboard:
            {
                break;
            }
        case mFullscreen:
            {
                break;
            }
        case mResize:
            {
                break;
            }
        }
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
        // New Modules here
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //  Shuts down and cleans up the engine
    //-----------------------------------------------------------------------------
    void Engine::Shutdown( void )
    {
        //////////////////////////////////////////
        // Shutdown and delete all task managers

        // New modules here
        SHUTDOWN_AND_DELETE( m_pMessageDispatcher );
        SHUTDOWN_AND_DELETE( m_pTaskManager );
        System::Shutdown();
    }


} // namespace Riot
