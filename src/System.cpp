/*********************************************************\
File:           System.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 11:51:47 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "System.h"
#include "timer.h"
#include "Thread.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#elif defined( OS_OSX )
#include <sys/types.h>
#include <sys/sysctl.h>
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
        int     mib[] = { CTL_HW, HW_AVAILCPU }; // Interested in availible CPUs
        size_t  nLen = sizeof( gs_nNumHardwareThreads );

        // Reads system info
        sysctl(mib, ARRAY_LENGTH(mib), &gs_nNumHardwareThreads, &nLen, NULL, 0);

        if( gs_nNumHardwareThreads < 1 ) 
        {   // HW_AVAILCPU might have been the problem, try HW_NCPU
            mib[1] = HW_NCPU;
            sysctl(mib, ARRAY_LENGTH(mib), &gs_nNumHardwareThreads, &nLen, NULL, 0);

             if( gs_nNumHardwareThreads < 1 )
             {
                  gs_nNumHardwareThreads = 1;
             }
        }
#endif // #ifdef OS_WINDOWS
        if( gs_nNumHardwareThreads > MAX_THREADS )
            gs_nNumHardwareThreads = MAX_THREADS;

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
    //  SpawnThread
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    thread_handle_t SpawnThread( _SystemThreadProc* pFunc, void* pData )
    {
        thread_handle_t hThread = NULL;
#ifdef OS_WINDOWS
        hThread = ::CreateThread( NULL, 0, pFunc, pData, 0, 0 );        
#else
        uint nResult = 0;
        nResult = pthread_create( &hThread, NULL, pFunc, pData );
#endif
        return hThread;
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
    thread_handle_t GetCurrentThreadHandle( void )
    {        
        thread_handle_t hThread = NULL;
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
    semaphore_t SemaphoreCreate( sint nInitialValue )
    {
        semaphore_t pSemaphore;
#ifdef OS_WINDOWS
        pSemaphore = ::CreateSemaphore( NULL, nInitialValue, LONG_MAX, NULL );      
#else
        ::sem_init( &pSemaphore, 0, nInitialValue );
        // TODO: Handle error
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
    
    
    //-----------------------------------------------------------------------------
    //  CreateMutex
    //  Creates a mutex
    //-----------------------------------------------------------------------------
    mutex_t CreateMutex( void )
    {
        mutex_t pMutex;
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pSem, INFINITE );  
#else
        ::pthread_mutex_init(&pMutex, NULL);
#endif        
        return pMutex;
    }
    
    //-----------------------------------------------------------------------------
    //  CreateWaitCondition
    //  Creates a condition to wait for
    //-----------------------------------------------------------------------------
    wait_condition_t CreateWaitCondition( void )
    {
        wait_condition_t pCondition;
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pSem, INFINITE );  
#else
        ::pthread_cond_init( &pCondition, NULL);
#endif        
        return pCondition;
    }
    
#include <stdio.h>
    //-----------------------------------------------------------------------------
    //  WaitForCondition
    //  Waits for the specified condition
    //-----------------------------------------------------------------------------
    void WaitForCondition( wait_condition_t* pCondition, mutex_t* pMutex )
    {
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pSem, INFINITE );  
#else
        ::pthread_cond_wait(pCondition, pMutex);
#endif        
    }
    
    //-----------------------------------------------------------------------------
    //  SignalCondition
    //  Signals a condition, waking anything waiting for it
    //-----------------------------------------------------------------------------
    void SignalCondition( wait_condition_t* pCondition )
    {
#ifdef OS_WINDOWS
#else
        ::pthread_cond_signal(pCondition);
#endif
    }
        
    /***************************************\
    | Private methods
    \***************************************/
} // namespace System

} // namespace Riot
