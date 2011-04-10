/*********************************************************\
File:           System.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 6:19:06 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "System.h"
#include "timer.h"
#include "Thread.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#else
#endif

namespace Riot
{

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
        // Reset the running timer
        gs_GlobalTimer.Reset();
        
        // Calculate the number of hardware threads
#if SINGLETHREADED
        gs_nNumHardwareThreads = 1;
#else

#ifdef OS_WINDOWS
        SYSTEM_INFO si;
        GetSystemInfo( &si );
        gs_nNumHardwareThreads = si.dwNumberOfProcessors;
#elif defined( OS_LINUX )
        gs_nNumHardwareThreads = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined( OS_OSX )
        int mib[4];
        size_t len = sizeof(gs_nNumHardwareThreads); 

        /* set the mib for hw.ncpu */
        mib[0] = CTL_HW;
        mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

        /* get the number of CPUs from the system */
        sysctl(mib, 2, &gs_nNumHardwareThreads, &len, NULL, 0);

        if( gs_nNumHardwareThreads < 1 ) 
        {
             mib[1] = HW_NCPU;
             sysctl( mib, 2, &gs_nNumHardwareThreads, &len, NULL, 0 );

             if( gs_nNumHardwareThreads < 1 )
             {
                  gs_nNumHardwareThreads = 1;
             }
        }
#endif // #ifdef OS_WINDOWS

#endif // #if SINGLETHREADED
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
    //  ThreadSpawn
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    thread_handle ThreadSpawn( _SystemThreadProc* pFunc, void* pData )
    {
        thread_handle hThread = NULL;
#ifdef OS_WINDOWS
        hThread = ::CreateThread( NULL, 0, pFunc, pData, 0, 0 );        
#else
        uint nResult = 0;
        nResult = pthread_create( &hThread, NULL, pFunc, pData );
#endif
        return hThread;
    }
    
    //-----------------------------------------------------------------------------
    //  DestroyThread
    //  Destroys the thread
    //-----------------------------------------------------------------------------
    void DestroyThread( thread_handle pThread )
    {
#ifdef OS_WINDOWS
        ::CloseHandle( pThread );
#endif
    }
    
    //-----------------------------------------------------------------------------
    //  GetHardwareThreadCount
    //  Returns the number of hardware threads in the system
    //-----------------------------------------------------------------------------
    uint GetHardwareThreadCount( void )
    {
        return gs_nNumHardwareThreads;
    }

    //-----------------------------------------------------------------------------
    //  GetCurrentThreadHandle
    //  Returns a handle to the calling thread
    //-----------------------------------------------------------------------------
    thread_handle GetCurrentThreadHandle( void )
    {        
        thread_handle hThread = NULL;
#ifdef OS_WINDOWS
        hThread = ::GetCurrentThread();        
#else
        hThread = ::pthread_self();
#endif
        return hThread;
    }
    
    //-----------------------------------------------------------------------------
    //  SemaphoreCreate
    //  Creates a semaphore
    //-----------------------------------------------------------------------------
    semaphore_t SemaphoreCreate( sint nInitialValue, sint nMaximumValue )
    {
        semaphore_t pSemaphore;
#ifdef OS_WINDOWS
        pSemaphore = ::CreateSemaphore( NULL, nInitialValue, nMaximumValue, NULL );      
#else
        uint nError = ::sem_init( &pSemaphore, 0, nInitialValue );
#endif

        return pSemaphore;
    }
    
    //-----------------------------------------------------------------------------
    //  SemaphoreDestroy
    //  Destroys a semaphore
    //-----------------------------------------------------------------------------
    void SemaphoreDestroy( semaphore_t* pSem )
    {
#ifdef OS_WINDOWS
        ::CloseHandle( *pSem );  
#else
        ::sem_destroy( pSem );
#endif
    }    

    //-----------------------------------------------------------------------------
    //  SemaphoreRelease
    //  Increments semaphore value
    //-----------------------------------------------------------------------------
    void SemaphoreRelease( semaphore_t* pSem )
    {
#ifdef OS_WINDOWS
        ::ReleaseSemaphore( *pSem, 1, NULL );  
#else
        ::sem_post( pSem );
#endif
    }    
    
    //-----------------------------------------------------------------------------
    //  WaitForSemaphore
    //  Waits until the semaphore value is non-zero
    //-----------------------------------------------------------------------------
    void WaitForSemaphore( semaphore_t* pSem )
    {
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pSem, INFINITE );  
#else
        ::sem_wait( pSem );
#endif
    }
        
    /***************************************\
    | Private methods
    \***************************************/
} // namespace System

} // namespace Riot
