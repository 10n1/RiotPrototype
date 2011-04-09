/*********************************************************\
File:           System.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 11:40:07 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "System.h"
#include "timer.h"
#include "Thread.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#else
#endif

namespace System
{
    //-----------------------------------------------------------------------------
    //  Function declarations
    //-----------------------------------------------------------------------------
#ifdef OS_WINDOWS
    DWORD WINAPI _ThreadProc( void* p );
#else
#endif
    //-----------------------------------------------------------------------------
    //  Struct definitions
    //-----------------------------------------------------------------------------
    /***************************************\
    | System members
    \***************************************/
    static CTimer       gs_GlobalTimer;

    
    /***************************************\
    | Public methods
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void Initialize( void )
    {
        gs_GlobalTimer.Reset();
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void Shutdown( void )
    {
    }

    //-----------------------------------------------------------------------------
    //  GetRunningTime
    //  Returns the time, in seconds, since the application was started
    //-----------------------------------------------------------------------------
    double GetRunningTime( void )
    {
        return gs_GlobalTimer.GetRunningTime();
    }

    //-----------------------------------------------------------------------------
    //  SpawnThread
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    handle SpawnThread( _SystemThreadProc* pFunc, void* pData )
    {
        handle hThread = NULL;
#ifdef OS_WINDOWS
        hThread = ::CreateThread( NULL, 0, pFunc, pData, 0, 0 );        
#else
#endif
        return hThread;
    }

    
    /***************************************\
    | Private methods
    \***************************************/
} // namespace System
