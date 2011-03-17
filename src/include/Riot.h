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

#if defined( OS_WINDOWS )
#include <Windows.h>
#endif // #if defined( WIN32 ) || defined( WIN64 )

class CWindow;

class Riot
{
//---------------------------------------------------------------------------------
//  Methods
public:
    Riot( void );
    ~Riot( void );
    
    //-----------------------------------------------------------------------------
    //  Run
    //  Starts the engine/game. All variables are set programatically
    //-----------------------------------------------------------------------------
    void Run( void );
private:
    //-----------------------------------------------------------------------------
    //  _CreateWindow
    //  Creates the main window
    //  TODO: Support multiple windows/views. Eg, the editor
    //-----------------------------------------------------------------------------
    uint _CreateWindow( uint nWidth, uint nHeight );

    //-----------------------------------------------------------------------------
    //  _WndProc
    //  Windows message handler
    //-----------------------------------------------------------------------------
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------------------------
//  Members
private:
    uint        m_nNumFrames;
    float       m_fElapsedTime;
    float       m_fRunningTime;
    HWND        m_hWnd;
    RiotInput*  m_pInput;
    CWindow*    m_pMainWindow;

    bool        m_bRunning;
};

#endif // #ifndef _RIOT_H_

