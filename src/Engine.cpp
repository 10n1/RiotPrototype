/*********************************************************\
File:           Engine.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 12:28:59 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Engine.h"
#include "TaskManager.h"

#define SHUTDOWN_AND_DELETE( Module ) if( Module ) { Module->Shutdown(); delete Module; Module = NULL; }
#define NEW_AND_INITIALIZE( Module, Type ) Module = new Type; Module->Initialize();

namespace Riot
{

/***************************************\
| class members                         |
\***************************************/
CTaskManager*   Engine::m_pTaskManager  = NULL;
bool            Engine::m_bRunning      = true;

/***************************************\
| class methods                         |
\***************************************/

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
    }

    //////////////////////////////////////////
    // Shutdown
    Shutdown();
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
    SHUTDOWN_AND_DELETE( m_pTaskManager );
    System::Shutdown();
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
    // New Modules here
}

} // namespace Riot
