/*********************************************************\
File:       Riot.h
Purpose:    Main header for the Riot engine
\*********************************************************/
#ifndef _RIOT_H_
#define _RIOT_H_
#include "Common.h"
#include "Types.h"
#include "Memory.h"
#include "IRefCounted.h"
#include "Input.h"

class CSceneGraph;
class CWindow;
class CGraphics;
class CView;
class CComponentManager;

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

    //-----------------------------------------------------------------------------
    //  GetGraphics
    //  Returns the current graphics interface
    //-----------------------------------------------------------------------------
    static CGraphics* GetGraphics( void );
private:
    //-----------------------------------------------------------------------------
    //  Initialize
    //  Initializes the engine. This is called from Run
    //-----------------------------------------------------------------------------
    static void Initialize( void );

//---------------------------------------------------------------------------------
//  Members
private:
    static uint         m_nFrameCount;
    static float        m_fElapsedTime;
    static float        m_fRunningTime;
    static RiotInput*   m_pInput;
    static CWindow*     m_pMainWindow;
    static CGraphics*   m_pGraphics;
    static CSceneGraph* m_pSceneGraph;
    static CComponentManager*   m_pComponentManager;

    static bool         m_bRunning;
};

#endif // #ifndef _RIOT_H_

