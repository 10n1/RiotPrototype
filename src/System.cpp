/*********************************************************\
File:           System.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 12:13:10 AM
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

    //-----------------------------------------------------------------------------
    //  Struct definitions
    //-----------------------------------------------------------------------------
    /***************************************\
    | System members
    \***************************************/
    static CTimer       gs_GlobalTimer;
    static uint         gs_nNumHardwareThreads = 0;

    
    /***************************************\
    | Public methods
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void Initialize( void )
    {
        gs_GlobalTimer.Reset();
        gs_nNumHardwareThreads = GetHardwareThreadCound();
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

    
    //-----------------------------------------------------------------------------
    //  GetHardwareThreadCount
    //  Returns the number of hardware threads in the system
    //-----------------------------------------------------------------------------
    uint GetHardwareThreadCound( void )
    {
        uint nCount = 0;
#if SINGLETHREADED
        nCount = 1;
#else

#ifdef OS_WINDOWS
        SYSTEM_INFO si;
        GetSystemInfo( &si );
        return si.dwNumberOfProcessors;
#elif defined( OS_LINUX )
        nCount = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined( OS_OSX )
        int mib[4];
        size_t len = sizeof(nCount); 

        /* set the mib for hw.ncpu */
        mib[0] = CTL_HW;
        mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

        /* get the number of CPUs from the system */
        sysctl(mib, 2, &nCount, &len, NULL, 0);

        if( nCount < 1 ) 
        {
             mib[1] = HW_NCPU;
             sysctl( mib, 2, &nCount, &len, NULL, 0 );

             if( nCount < 1 )
             {
                  nCount = 1;
             }
        }
#endif // #ifdef OS_WINDOWS

#endif // #if SINGLETHREADED

        return nCount;
    }

    
    /***************************************\
    | Private methods
    \***************************************/
} // namespace System
