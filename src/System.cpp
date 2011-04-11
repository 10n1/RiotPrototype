/*********************************************************\
File:           System.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 10:38:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "System.h"
#include "timer.h"
#include "Thread.h"
#include "Window.h"
#include "Engine.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#include "Win32Application.h"
#elif defined( OS_OSX )
#include <sys/types.h>
#include <sys/sysctl.h>
#include "OSXApplication.h"
#else
// Linux
#endif

namespace Riot
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
    CTimer      System::gs_GlobalTimer;
    uint        System::gs_nNumHardwareThreads = 0;
    CWindow*    System::gs_pMainWindow  = NULL;
#ifdef OS_WINDOWS
    CWin32Application*  System::gs_pApplication = NULL;
#elif defined( OS_OSX )
    // OS X has NS App automatically defined
#else
#endif

    /***************************************\
    | Public methods
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void System::Initialize( void )
    {
        // Reset the running timer
        gs_GlobalTimer.Reset();

        // Calculate the number of hardware threads
#if (MULTITHREADED == 0)
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
    void System::Shutdown( void )
    {
    }

    //-----------------------------------------------------------------------------
    //  GetRunningTime
    //  Returns the time, in seconds, since the application was started
    //-----------------------------------------------------------------------------
    double System::GetRunningTime( void )
    {
        return gs_GlobalTimer.GetRunningTime();
    }

    //-----------------------------------------------------------------------------
    //  SpawnThread
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    System::thread_handle_t System::SpawnThread( _SystemThreadProc* pFunc, void* pData )
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
    uint System::GetHardwareThreadCount( void )
    {
        return gs_nNumHardwareThreads;
    }

    //-----------------------------------------------------------------------------
    //  GetCurrentThreadHandle
    //  Returns a handle to the calling thread
    //-----------------------------------------------------------------------------
    System::thread_handle_t System::GetCurrentThreadHandle( void )
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
    //  CreateRiotSemaphore
    //  Creates a semaphore
    //-----------------------------------------------------------------------------
    System::semaphore_t System::CreateRiotSemaphore( sint nInitialValue )
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
    void System::SemaphoreDestroy( System::semaphore_t* pSem )
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
    void System::SemaphoreRelease( System::semaphore_t* pSem )
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
    void System::WaitForSemaphore( System::semaphore_t* pSem )
    {
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pSem, INFINITE );  
#else
        ::sem_wait( pSem );
#endif
    }

    //-----------------------------------------------------------------------------
    //  GetSemaphoreValue
    //  Returns the value of the semaphore
    //-----------------------------------------------------------------------------
    sint System::GetSemaphoreValue( System::semaphore_t* pSem )
    {
        sint nValue;
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pSem, INFINITE );
#else
        ::sem_getvalue( pSem, &nValue );
#endif
        return nValue;
    }

    //-----------------------------------------------------------------------------
    //  CreateRiotMutex
    //  Creates a mutex
    //-----------------------------------------------------------------------------
    System::mutex_t System::CreateRiotMutex( void )
    {
        mutex_t pMutex;
#ifdef OS_WINDOWS
        pMutex = ::CreateMutex( NULL, false, NULL ); 
#else
        ::pthread_mutex_init(&pMutex, NULL);
#endif        
        return pMutex;
    }

    //-----------------------------------------------------------------------------
    //  CreateWaitCondition
    //  Creates a condition to wait for
    //-----------------------------------------------------------------------------
    System::wait_condition_t System::CreateWaitCondition( void )
    {
        wait_condition_t pCondition;
#ifdef OS_WINDOWS 
        pCondition = ::CreateEvent( NULL, false, false, NULL );
#else
        ::pthread_cond_init( &pCondition, NULL);
#endif        
        return pCondition;
    }

    //-----------------------------------------------------------------------------
    //  WaitForCondition
    //  Waits for the specified condition
    //-----------------------------------------------------------------------------
    void System::WaitForCondition( System::wait_condition_t* pCondition, mutex_t* pMutex )
    {
#ifdef OS_WINDOWS
        ::WaitForSingleObject( *pCondition, INFINITE );
        ::ResetEvent( *pCondition ); // It was just signaled, turn it back off
#else
        ::pthread_cond_wait(pCondition, pMutex);
#endif        
    }

    //-----------------------------------------------------------------------------
    //  SignalCondition
    //  Signals a condition, waking anything waiting for it
    //-----------------------------------------------------------------------------
    void System::SignalCondition( System::wait_condition_t* pCondition )
    {
#ifdef OS_WINDOWS
        ::SetEvent( *pCondition );
#else
        ::pthread_cond_signal(pCondition);
#endif
    }

    //-----------------------------------------------------------------------------
    //  CreateMainWindow
    //  Creates the main application window
    //-----------------------------------------------------------------------------
    CWindow* System::CreateMainWindow( uint nWidth, uint nHeight )
    {
        // If we already have a window, just return it
        if( gs_pMainWindow != NULL )
        {
            gs_pMainWindow->AddRef();
            return gs_pMainWindow;
        }

        gs_pMainWindow = new CWindow();
#ifdef OS_WINDOWS
        ASSERT( gs_pApplication == NULL );
        gs_pApplication = new CWin32Application();

        gs_pApplication->CreateMainWindow( nWidth, nHeight );

        gs_pMainWindow->m_pSystemWindow = gs_pApplication;
#else
        ///////////////////////////////////
        // Create the Window
        ASSERT( NSApp == nil );
        
        NSApp = [COSXApplication sharedApplication];
        
        ASSERT( NSApp );
        
        
        ///////////////////////////////////
        // Load the Xib
        if( ![NSBundle loadNibNamed:@"Menu" owner:NSApp] )
        {
            // TODO: Handle error
        }
        
        
        ///////////////////////////////////
        // Initialize the Window
        [NSApp CreateWindowWithWidth:nWidth Height:nHeight Fullscreen:false Window:gs_pMainWindow];
        
        gs_pMainWindow->m_pSystemWindow = NSApp;
#endif
        
        gs_pMainWindow->m_nWidth = nWidth;
        gs_pMainWindow->m_nHeight = nHeight;
        
        return gs_pMainWindow;
    }

    //-----------------------------------------------------------------------------
    //  ProcessOSMessages
    //  Processes messages from the OS
    //-----------------------------------------------------------------------------
    void System::ProcessOSMessages( void )
    {
#ifdef OS_WINDOWS
        gs_pApplication->ProcessOSMessages();
#else
        [NSApp ProcessOSMessages];
#endif
    }


    //-----------------------------------------------------------------------------
    //  CGraphics
    //  Creates and returns an OpenGL interface for the specified window
    //-----------------------------------------------------------------------------
    CGraphics* System::CreateOpenGLDevice( CWindow* pWindow )
    {
        return 0;
    }

} // namespace Riot
