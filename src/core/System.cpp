/*********************************************************\
File:           System.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/27/2011 6:21:59 PM
Modified by:    Kyle Weicht
 \*********************************************************/
#include "OGLGraphics.h"
#include "System.h"
#include "timer.h"
#include "Thread.h"
#include "Window.h"
#include "Engine.h"
#include "Graphics.h"
#include "SystemOpenGL.h"
#include "atomic.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#include "Win32Application.h"
#include <intrin.h>
#elif defined( OS_OSX )
#include <sys/types.h>
#include <sys/sysctl.h>
#include "OSXApplication.h"
#else
// Linux
#endif

#ifndef OS_WINDOWS
void __cpuid( int* a, int b )
{
#ifdef _64BIT
	__asm __volatile(
		"	push	%%rbx\n"
		"	push	%%rcx\n"
		"	push	%%rdx\n"
		"	push	%%rdi\n"
		
		"	mov	%0,	%%rdi\n"
		
		"	mov	(%%rdi),	%%eax\n"
		"	mov	4(%%rdi),	%%ebx\n"
		"	mov	8(%%rdi),	%%ecx\n"
		"	mov	12(%%rdi),	%%edx\n"

        "   mov eax, b\n"
		
		"	cpuid\n"
		
		"	movl	%%eax,	(%%rdi)\n"
		"	movl	%%ebx,	4(%%rdi)\n"
		"	movl	%%ecx,	8(%%rdi)\n"
		"	movl	%%edx,	12(%%rdi)\n"
		"	pop	%%rdi\n"
		"	pop	%%rdx\n"
		"	pop	%%rcx\n"
		"	pop	%%rbx\n"
		:
		:"rdi"(regs)
		:"memory", "eax"
	);
#else
	__asm __volatile(
		"	push	%%ebx\n"
		"	push	%%ecx\n"
		"	push	%%edx\n"
		"	push	%%edi\n"
		"	mov	%0,	%%edi\n"
		
		"	mov	(%%edi),	%%eax\n"
		"	mov	4(%%edi),	%%ebx\n"
		"	mov	8(%%edi),	%%ecx\n"
		"	mov	12(%%edi),	%%edx\n"
		
        "   mov eax, b\n"

		"	cpuid\n"
		
		"	mov	%%eax,	(%%edi)\n"
		"	mov	%%ebx,	4(%%edi)\n"
		"	mov	%%ecx,	8(%%edi)\n"
		"	mov	%%edx,	12(%%edi)\n"
		"	pop	%%edi\n"
		"	pop	%%edx\n"
		"	pop	%%ecx\n"
		"	pop	%%ebx\n"
		:
		:"m"(regs)
		:"memory", "eax"
	);
#endif // _64BIT
}
#endif



//-----------------------------------------------------------------------------
//  Upon first running, determine the CPU capabilities
//-----------------------------------------------------------------------------
static uint8 _nNumHardwareThreads = 0;
static uint8 _nSSESupported    = 0;
static uint8 _nSSE2Supported   = 0;
static uint8 _nSSE3Supported   = 0;
static uint8 _nSSSE3Supported  = 0;
static uint8 _nSSE41Supported  = 0;
static uint8 _nSSE42Supported  = 0;
static uint8 _nAVXSupported    = 0;

static const uint GetCPUCapabilities( void )
{
    //////////////////////////////////////////
    // First determine the number of threads
#if (MULTITHREADED == 0)
    gs_nNumHardwareThreads = 1;
#else

#ifdef OS_WINDOWS
    SYSTEM_INFO si;
    GetSystemInfo( &si );
    _nNumHardwareThreads = (uint8)si.dwNumberOfProcessors;
#elif defined( OS_LINUX )
    m_nNumHardwareThreads = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined( OS_OSX )
    int     mib[] = { CTL_HW, HW_AVAILCPU }; // Interested in availible CPUs
    size_t  nLen = sizeof( _nNumHardwareThreads );

    // Reads system info
    sysctl(mib, ARRAY_LENGTH(mib), &_nNumHardwareThreads, &nLen, NULL, 0);

    if( _nNumHardwareThreads < 1 ) 
    {   // HW_AVAILCPU might have been the problem, try HW_NCPU
        mib[1] = HW_NCPU;
        sysctl(mib, ARRAY_LENGTH(mib), &_nNumHardwareThreads, &nLen, NULL, 0);

        if( _nNumHardwareThreads < 1 )
        {
            _nNumHardwareThreads = 1;
        }
    }
#endif // #ifdef OS_WINDOWS
    if( _nNumHardwareThreads > MAX_THREADS )
        _nNumHardwareThreads = MAX_THREADS;

#endif // #if SINGLETHREADED

    //////////////////////////////////////////
    // Then get the capabilities
#ifdef OS_WINDOWS
    sint nFeatures[4] = { 0 };

    __cpuid( nFeatures, 1 );

    _nSSESupported   = (nFeatures[3] & BIT_25) ? 1 : 0;
    _nSSE2Supported  = (nFeatures[3] & BIT_26) ? 1 : 0;
    _nSSE3Supported  = (nFeatures[2] & BIT_0 ) ? 1 : 0;
    _nSSSE3Supported = (nFeatures[2] & BIT_9 ) ? 1 : 0;
    _nSSE41Supported = (nFeatures[2] & BIT_19) ? 1 : 0;
    _nSSE42Supported = (nFeatures[2] & BIT_20) ? 1 : 0;

    // For AVX, we need to make sure OSXSAVE is supported too
    uint8 nOSXSAVESupported  = (nFeatures[2] & BIT_27) ? 1 : 0;
    uint8 nAVXSupported      = (nFeatures[2] & BIT_28) ? 1 : 0;

    _nAVXSupported    = nOSXSAVESupported & nAVXSupported;
#endif

    return 1;
}

static const uint gs_nCPUPolled = GetCPUCapabilities();

//-----------------------------------------------------------------------------
//  The location of the supported bits
const uint8 gs_nNumHardwareThreads = _nNumHardwareThreads;
const uint8 gs_nSSESupported       = _nSSESupported;
const uint8 gs_nSSE2Supported      = _nSSE2Supported;
const uint8 gs_nSSE3Supported      = _nSSE3Supported;
const uint8 gs_nSSSE3Supported     = _nSSSE3Supported;
const uint8 gs_nSSE41Supported     = _nSSE41Supported;
const uint8 gs_nSSE42Supported     = _nSSE42Supported;
const uint8 gs_nAVXSupported       = _nAVXSupported;
//-----------------------------------------------------------------------------


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
    CTimer      System::m_GlobalTimer;
    CWindow*    System::m_pMainWindow           = NULL;
    IGraphicsDevice*  System::m_pGraphics       = NULL;
    handle      System::m_pApplication = NULL;


    /***************************************\
    | Public methods
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void System::Initialize( void )
    {
        // Reset the running timer
        m_GlobalTimer.Reset();
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void System::Shutdown( void )
    {
#ifdef OS_WINDOWS
        SAFE_DELETE( m_pApplication );
#endif // #ifdef OS_WINDOWS
    }

    //-----------------------------------------------------------------------------
    //  GetRunningTime
    //  Returns the time, in seconds, since the application was started
    //-----------------------------------------------------------------------------
    double System::GetRunningTime( void )
    {
        return m_GlobalTimer.GetRunningTime();
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
        nValue = 0xFFFFFFFF;
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
        if( m_pMainWindow != NULL )
        {
            m_pMainWindow->AddRef();
            return m_pMainWindow;
        }

        m_pMainWindow = new CWindow();
#ifdef OS_WINDOWS
        ASSERT( m_pApplication == NULL );
        m_pApplication = new CWin32Application();

        m_pMainWindow->m_pWindow = ((CWin32Application*)m_pApplication)->CreateMainWindow( nWidth, nHeight );
#else
        DECLAREPOOL;
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
            ASSERT( 0 );
        }

        
        ///////////////////////////////////
        // Initialize the Window
        m_pMainWindow->m_pWindow = [NSApp CreateWindowWithWidth:nWidth Height:nHeight Fullscreen:false Window:m_pMainWindow];
          
        
        ///////////////////////////////////
        //  Make sure OpenGL knows about the view
        SystemOpenGL::SetView( [NSApp m_pView] );
        
        m_pApplication = NSApp;    
        RELEASEPOOL;
#endif        
        m_pMainWindow->m_nWidth = nWidth;
        m_pMainWindow->m_nHeight = nHeight;
        
        return m_pMainWindow;
    }

    //-----------------------------------------------------------------------------
    //  ProcessOSMessages
    //  Processes messages from the OS
    //-----------------------------------------------------------------------------
    void System::ProcessOSMessages( void )
    {
#ifdef OS_WINDOWS
        ((CWin32Application*)m_pApplication)->ProcessOSMessages();
#else
        [NSApp ProcessOSMessages];
#endif
    }


    //-----------------------------------------------------------------------------
    //  IGraphicsDevice
    //  Creates and returns an OpenGL interface for the specified window
    //-----------------------------------------------------------------------------
    //IGraphicsDevice* System::CreateOpenGLDevice( CWindow* pWindow )
    void System::CreateOpenGLDevice( IGraphicsDevice* pDevice, CWindow* pWindow )
    {
        ASSERT( m_pGraphics == NULL );
        
        // Grab a reference to it
        m_pGraphics = pDevice;
    }

} // namespace Riot
