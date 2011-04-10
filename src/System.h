/*********************************************************\
File:           System.h
Purpose:        Interface to the base system. All platform
                specific functions will be called here.
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 12:25:10 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "common.h"

#ifndef OS_WINDOWS
#include <pthread.h>
#include <semaphore.h>
#endif

namespace Riot
{

namespace System
{
#ifdef OS_WINDOWS
    typedef unsigned long   thread_return_t;
    typedef handle          thread_handle_t;
    typedef handle          semaphore_t;
    typedef handle          wait_condition_t;
    typedef handle          mutex_t;
#else
    typedef void*           thread_return_t;
    typedef pthread_t       thread_handle_t;
    typedef sem_t           semaphore_t;
    typedef pthread_cond_t  wait_condition_t;
    typedef pthread_mutex_t mutex_t;
#endif
    typedef thread_return_t (SYSTEM_API_CONVENTION _SystemThreadProc)( void* pData );

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
    //  SpawnThread
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    thread_handle_t SpawnThread( _SystemThreadProc* pFunc, void* pData );
    
    //-----------------------------------------------------------------------------
    //  GetHardwareThreadCount
    //  Returns the number of hardware threads in the system
    //-----------------------------------------------------------------------------
    uint GetHardwareThreadCount( void );
        
    //-----------------------------------------------------------------------------
    //  GetCurrentThreadHandle
    //  Returns a handle to the calling thread
    //-----------------------------------------------------------------------------
    thread_handle_t GetCurrentThreadHandle( void );    
    
    //-----------------------------------------------------------------------------
    //  CreateRiotSemaphore
    //  Creates a semaphore
    //-----------------------------------------------------------------------------
    semaphore_t CreateRiotSemaphore( sint nInitialValue );
    
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

    //-----------------------------------------------------------------------------
    //  GetSemaphoreValue
    //  Returns the value of the semaphore
    //-----------------------------------------------------------------------------
    sint GetSemaphoreValue( semaphore_t* pSem );
    
    //-----------------------------------------------------------------------------
    //  CreateRiotMutex
    //  Creates a mutex
    //-----------------------------------------------------------------------------
    mutex_t CreateRiotMutex( void );
    
    //-----------------------------------------------------------------------------
    //  CreateWaitCondition
    //  Creates a condition to wait for
    //-----------------------------------------------------------------------------
    wait_condition_t CreateWaitCondition( void );    
    
    //-----------------------------------------------------------------------------
    //  WaitForCondition
    //  Waits for the specified condition
    //-----------------------------------------------------------------------------
    void WaitForCondition( wait_condition_t* pCondition, mutex_t* pMutex );    
    
    //-----------------------------------------------------------------------------
    //  SignalCondition
    //  Signals a condition, waking anything waiting for it
    //-----------------------------------------------------------------------------
    void SignalCondition( wait_condition_t* pCondition );

} // namespace System

} // namespace Riot

#endif // #ifndef _SYSTEM_H_
