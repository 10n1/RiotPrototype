/*********************************************************\
File:       Riot.h
Purpose:    Main header for the Riot engine
\*********************************************************/
#ifndef _RIOT_H_
#define _RIOT_H_
#include "Types.h"
#include "Memory.h"
#include "IRefCounted.h"
#include "Input.h"


class CWindow;

class Riot
{
//---------------------------------------------------------------------------------
//  Methods
public:    
    //-----------------------------------------------------------------------------
    //  Run
    //  Starts the engine/game. All variables are set programatically
    //-----------------------------------------------------------------------------
    static void Run( void );

    //-----------------------------------------------------------------------------
    //  Shutdown
    //  Shuts down and cleans up the engine
    //-----------------------------------------------------------------------------
    static void Shutdown( void );
private:

//---------------------------------------------------------------------------------
//  Members
private:
    static uint        m_nFrameCount;
    static float       m_fElapsedTime;
    static float       m_fRunningTime;
    static RiotInput*  m_pInput;
    static CWindow*    m_pMainWindow;

    static bool        m_bRunning;
};

#endif // #ifndef _RIOT_H_

