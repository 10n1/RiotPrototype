/*********************************************************\
File:           Engine.h
Purpose:        The main engine
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 12:35:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ENGINE_H_
#define _ENGINE_H_
#include "common.h"

namespace Riot
{

class CTaskManager;

class Engine
{
public:
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Run
    //  Starts the engine/game. All variables are set programatically
    //-----------------------------------------------------------------------------
    static void Run( void );

private:    
    //-----------------------------------------------------------------------------
    //  Shutdown
    //  Shuts down and cleans up the engine
    //-----------------------------------------------------------------------------
    static void Shutdown( void );

    //-----------------------------------------------------------------------------
    //  Initialize
    //  Initializes the engine. This is called from Run
    //-----------------------------------------------------------------------------
    static void Initialize( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    static CTaskManager*    m_pTaskManager;
    
    static bool             m_bRunning;
};


} // namespace Riot

#endif // #ifndef _ENGINE_H_
