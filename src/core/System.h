/*********************************************************\
File:           System.h
Purpose:        Interface to the base system. All platform
specific functions will be called here.
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/27/2011 10:21:17 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "common.h"
#include "timer.h"
#include "Input.h"
#include "File.h"

#ifndef OS_WINDOWS
#include <pthread.h>
#include <semaphore.h>
#endif

namespace Riot
{
    class CWindow;
    class IGraphicsDevice;
    typedef uint        task_handle_t;

    class System
    {
    public:
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
        static void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        static void Shutdown( void );

        //-----------------------------------------------------------------------------
        //  DetermineCPUFeatures
        //  Polls the CPU to determine what features it supports
        //-----------------------------------------------------------------------------
        static void DetermineCPUFeatures( void );

        //-----------------------------------------------------------------------------
        //  GetRunningTime
        //  Returns the time, in seconds, since the application was started
        //-----------------------------------------------------------------------------
        static double GetRunningTime( void );

        //-----------------------------------------------------------------------------
        //  SpawnThread
        //  Creates and starts the thread, running until the function completes
        //-----------------------------------------------------------------------------
        static thread_handle_t SpawnThread( _SystemThreadProc* pFunc, void* pData );

        //-----------------------------------------------------------------------------
        //  GetCurrentThreadHandle
        //  Returns a handle to the calling thread
        //-----------------------------------------------------------------------------
        static thread_handle_t GetCurrentThreadHandle( void );    

        //-----------------------------------------------------------------------------
        //  CreateRiotSemaphore
        //  Creates a semaphore
        //-----------------------------------------------------------------------------
        static semaphore_t CreateRiotSemaphore( sint nInitialValue );

        //-----------------------------------------------------------------------------
        //  SemaphoreDestroy
        //  Destroys a semaphore
        //-----------------------------------------------------------------------------
        static void SemaphoreDestroy( semaphore_t* pSem );

        //-----------------------------------------------------------------------------
        //  SemaphoreRelease
        //  Increments semaphore value
        //-----------------------------------------------------------------------------
        static void SemaphoreRelease( semaphore_t* pSem );

        //-----------------------------------------------------------------------------
        //  WaitForSemaphore
        //  Waits until the semaphore value is non-zero
        //-----------------------------------------------------------------------------
        static void WaitForSemaphore( semaphore_t* pSem );

        //-----------------------------------------------------------------------------
        //  GetSemaphoreValue
        //  Returns the value of the semaphore
        //-----------------------------------------------------------------------------
        static sint GetSemaphoreValue( semaphore_t* pSem );

        //-----------------------------------------------------------------------------
        //  CreateRiotMutex
        //  Creates a mutex
        //-----------------------------------------------------------------------------
        static mutex_t CreateRiotMutex( void );

        //-----------------------------------------------------------------------------
        //  CreateWaitCondition
        //  Creates a condition to wait for
        //-----------------------------------------------------------------------------
        static wait_condition_t CreateWaitCondition( void );    

        //-----------------------------------------------------------------------------
        //  WaitForCondition
        //  Waits for the specified condition
        //-----------------------------------------------------------------------------
        static void WaitForCondition( wait_condition_t* pCondition, mutex_t* pMutex );    

        //-----------------------------------------------------------------------------
        //  SignalCondition
        //  Signals a condition, waking anything waiting for it
        //-----------------------------------------------------------------------------
        static void SignalCondition( wait_condition_t* pCondition );
        
        //-----------------------------------------------------------------------------
        //  CreateMainWindow
        //  Creates the main application window
        //-----------------------------------------------------------------------------
        static CWindow* CreateMainWindow( uint nWidth, uint nHeight );
        
        //-----------------------------------------------------------------------------
        //  ProcessOSMessages
        //  Processes messages from the OS
        //-----------------------------------------------------------------------------
        static void ProcessOSMessages( void );
        
        //-----------------------------------------------------------------------------
        //  CreateOpenGLDevice
        //  Creates and returns an OpenGL interface for the specified window
        //-----------------------------------------------------------------------------
        static void CreateOpenGLDevice( IGraphicsDevice* pDevice, CWindow* pWindow );
        
        //-----------------------------------------------------------------------------
        //  Log
        //  Logs a string into the system log
        //-----------------------------------------------------------------------------
        static void Log( const char* szText );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CFile        m_Log;
        static CTimer       m_GlobalTimer;
        static CWindow*     m_pMainWindow;
        static IGraphicsDevice*   m_pGraphics;

        static handle   m_pApplication; // Type CWin32Application
                                        //      COSXApplication

        static task_handle_t    m_nLogTask;

        // These are all made constant so they can be read from anywhere without
        //  acesssors
    public:
        //static const uint   m_nNumHardwareThreads;
        //
        //static const uint   m_nSSESupported;
        //static const uint   m_nSSE2Supported;
        //static const uint   m_nSSE3Supported;
        //static const uint   m_nSSSE3Supported;
        //static const uint   m_nSSE41Supported;
        //static const uint   m_nSSE42Supported;
        //static const uint   m_nAVXSupported;
    };

} // namespace Riot

#endif // #ifndef _SYSTEM_H_
