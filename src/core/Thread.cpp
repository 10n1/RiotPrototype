/*********************************************************\
File:           Thread.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/23/2011 2:21:10 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Thread.h"
#include "TaskManager.h"

namespace Riot
{

    // CThread constructor
    CThread::CThread()
        : m_pThread( NULL )
        , m_nThreadId( 0 )
        , m_pTaskManager( NULL )
    {
    }

    // CThread destructor
    CThread::~CThread()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  _ThreadProc
    //  The most basic OS thread function
    //-----------------------------------------------------------------------------
    System::thread_return_t CThread::_ThreadProc( void* pData )
    {
        CThread* pThread = (CThread*)pData;

        pThread->ThreadProc();

        return 0;
    }

    //-----------------------------------------------------------------------------
    //  Start
    //  Starts the thread, running the input function, then returning
    //-----------------------------------------------------------------------------
    void CThread::Start( CTaskManager* pTaskManager )
    {
        m_pTaskManager      = pTaskManager;
        m_bFinished         = false;
        m_pSystemMutex      = System::CreateRiotMutex();
        m_pWakeCondition    = System::CreateWaitCondition();

        Memset( m_pTasks, 0, sizeof(m_pTasks) );

        m_pThread           = System::SpawnThread( &_ThreadProc, this );
    }

    //-----------------------------------------------------------------------------
    //  GetThreadId
    //  Returns the thread Id
    //-----------------------------------------------------------------------------
    uint CThread::GetThreadId( void )
    {
        return m_nThreadId;
    }

    //-----------------------------------------------------------------------------
    //  ThreadProc
    //  The thread's main loop
    //-----------------------------------------------------------------------------
    void CThread::ThreadProc( void )
    {
        for(;;)
        {
            // Idle the thread
            Idle();

            // Make sure we're not shutting down
            if( m_pTaskManager->m_bShutdown )
                break;

            // Do all the work you can
            while( m_pTaskManager->m_nActiveTasks )
            {
                DoWork( );
            }
        }

        m_bFinished = true;
    }
 
    //-----------------------------------------------------------------------------
    //  DoWork
    //  The thread starts doing work. It'll steal more if it has to
    //-----------------------------------------------------------------------------
    void CThread::DoWork( void )
    {
        TTask*  pTask = NULL;
        sint    nStart = 0;
        sint    nCount = 0;

        while( m_pTaskManager->GetWork( &pTask, &nStart, &nCount ) )
        {
            if( pTask == NULL )
            {
                continue;
            }

            TaskFunc* pFunc  = pTask->pFunc;  
            void*     pData  = pTask->pData;

            pFunc( pData, m_nThreadId, nStart, nCount );

            // Let the task know it's done being worked on
            AtomicDecrement( &pTask->nCompletion );
        }
    }

    //-----------------------------------------------------------------------------
    //  Idle
    //  Idles the thread until it gets work
    //-----------------------------------------------------------------------------
    void CThread::Idle( void )
    {
        // Zzzzzzz.....
        System::SemaphoreRelease( &m_pTaskManager->m_pSleep );

        // Wait until you're woken up
        System::WaitForCondition(&m_pWakeCondition, &m_pSystemMutex);
    }

    //-----------------------------------------------------------------------------
    //  MakeMainThread
    //  Attaches this CThread to the main program thread
    //-----------------------------------------------------------------------------
    void CThread::MakeMainThread( CTaskManager* pTaskManager )
    {
        m_pTaskManager  = pTaskManager;
        m_pThread       = System::GetCurrentThreadHandle();
        m_bFinished     = false;
    }

    //-----------------------------------------------------------------------------
    //  Wake
    //  Wakes the thread up so it can get to work
    //-----------------------------------------------------------------------------
    void CThread::Wake( void )
    {
        // Tell yourself to wake up
        System::SignalCondition( &m_pWakeCondition );
    }

} // namespace Riot
