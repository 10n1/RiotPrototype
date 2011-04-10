/*********************************************************\
File:           System.h
Purpose:        Interface to the base system. All platform
                specific functions will be called here.
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 6:29:34 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "common.h"

namespace Riot
{

namespace System
{
#ifdef OS_WINDOWS
    typedef unsigned long (_SystemThreadProc)( void* pData );
    typedef unsigned long   thread_return_t;
    typedef handle          thread_handle;
    typedef handle          semaphore_t;
#else
    typedef void* (_SystemThreadProc)( void* pData );
    typedef void*       thread_return_t;
    typedef pthread_t   thread_handle;
    typedef sem_t       semaphore_t;
#endif

    /***************************************\
    | class methods                         |
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void Initialize( void );
    
    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void Shutdown( void );

    //-----------------------------------------------------------------------------
    //  GetRunningTime
    //  Returns the time, in seconds, since the application was started
    //-----------------------------------------------------------------------------
    double GetRunningTime( void );
    
    //-----------------------------------------------------------------------------
    //  ThreadSpawn
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    thread_handle ThreadSpawn( _SystemThreadProc* pFunc, void* pData );
    
    //-----------------------------------------------------------------------------
    //  DestroyThread
    //  Destroys the thread
    //-----------------------------------------------------------------------------
    void DestroyThread( thread_handle pThread );
    
    //-----------------------------------------------------------------------------
    //  GetHardwareThreadCount
    //  Returns the number of hardware threads in the system
    //-----------------------------------------------------------------------------
    uint GetHardwareThreadCount( void );
        
    //-----------------------------------------------------------------------------
    //  GetCurrentThreadHandle
    //  Returns a handle to the calling thread
    //-----------------------------------------------------------------------------
    thread_handle GetCurrentThreadHandle( void );    
    
    //-----------------------------------------------------------------------------
    //  SemaphoreCreate
    //  Creates a semaphore
    //-----------------------------------------------------------------------------
    semaphore_t SemaphoreCreate( sint nInitialValue );
    
    //-----------------------------------------------------------------------------
    //  SemaphoreDestroy
    //  Destroys a semaphore
    //-----------------------------------------------------------------------------
    void SemaphoreDestroy( semaphore_t* pSem );

    //-----------------------------------------------------------------------------
    //  SemaphoreRelease
    //  Increments semaphore value
    //-----------------------------------------------------------------------------
    void SemaphoreRelease( semaphore_t* pSem );
    
    //-----------------------------------------------------------------------------
    //  WaitForSemaphore
    //  Waits until the semaphore value is non-zero
    //-----------------------------------------------------------------------------
    void WaitForSemaphore( semaphore_t* pSem );

} // namespace System

} // namespace Riot

#endif // #ifndef _SYSTEM_H_
