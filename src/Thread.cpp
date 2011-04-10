/*********************************************************\
File:           Thread.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 4:26:45 PM
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
        m_bAwake            = false;
        m_pSystemMutex      = System::CreateRiotMutex();
        m_pWakeCondition    = System::CreateWaitCondition();

        m_pThread = System::SpawnThread( &_ThreadProc, this );
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
        do
        {
            m_bAwake = true;

            while( m_pTaskManager->m_nActiveTasks )
            {
                // Do all your work
                DoWork( NULL );

                // Make sure we're not shutting down
                if( m_pTaskManager->m_bShutdown )
                    break;
            }

            // Idle the thread
            Idle();

        } while( !m_pTaskManager->m_bShutdown );

        m_bFinished = true;
    }

    //-----------------------------------------------------------------------------
    //  PushTask
    //  Pushes a task onto the top of the task queue
    //-----------------------------------------------------------------------------
    void CThread::PushTask( TTask& task )
    {
        // Lock the mutex and add the task
        m_TaskMutex.Lock();
        m_pTasks[m_nNumTasks++] = task;
        AtomicIncrement( task.pCompletion );
        m_TaskMutex.Unlock();
    }

    //-----------------------------------------------------------------------------
    //  PopTask
    //  Pops a task off the queue
    //-----------------------------------------------------------------------------
    bool CThread::PopTask( TTask* pTask )
    {
        // Lock the mutex and pop the task off
        CScopedMutex lock( &m_TaskMutex );

        if( m_nNumTasks == 0 )
        {
            return false;
        }

        uint nTask = --m_nNumTasks;

        *pTask = m_pTasks[nTask];
        //task->pFunc     = m_pTasks[nTask].pFunc;
        //task->pData     = m_pTasks[nTask].pData;
        //task->nStart    = m_pTasks[nTask].nStart;
        //task->nCount    = m_pTasks[nTask].nCount;
        //task->pCompletion   = m_pTasks[nTask].pCompletion;

        return true;
    }
 
    //-----------------------------------------------------------------------------
    //  DoWork
    //  The thread starts doing work. It'll steal more if it has to
    //-----------------------------------------------------------------------------
    void CThread::DoWork( task_completion_t* pCompletion )
    {
        do
        {
            TTask task;
            while( PopTask( &task ) )
            {
                // Grab some work and do it
                TaskFunc* pFunc = task.pFunc;  
                void* pData     = task.pData;
                uint nStart     = task.nStart;
                uint nCount     = task.nCount;

                pFunc( pData, m_nThreadId, nStart, nCount );
                AtomicDecrement( task.pCompletion );

                if( pCompletion && (*pCompletion == 0) )
                {
                    // What we're waiting for is done
                    return;
                }
            }

        } while( StealTasks() );
    }

    //-----------------------------------------------------------------------------
    //  GiveUpWork
    //  Gives up work to the idle thread
    //-----------------------------------------------------------------------------
    bool CThread::GiveUpWork( CThread* pIdleThread )
    {
        CScopedMutex lock;
        if( lock.TryLock( &m_TaskMutex ) == false )
        {
            // This thread is locked, leave
            return false;
        }

        if( m_nNumTasks == 0 )
        {
            // We've just finished our work, leave
            return false;
        }

        // We now own the thread and theres work. Let's take it!
        CScopedMutex idleLock( &pIdleThread->m_TaskMutex );

        if( pIdleThread->m_nNumTasks )
        {
            // In this short period, we've been given work. Leave
            return false;
        }

        uint nCount = (m_nNumTasks+1) >> 1;
        TTask* pTasks = pIdleThread->m_pTasks;
        for( uint i = m_nNumTasks-nCount; i < m_nNumTasks; ++i )
        {
            *pTasks = m_pTasks[i];
            pTasks++;
        }
        m_nNumTasks -= nCount;
        pIdleThread->m_nNumTasks = nCount;

        return true;
    }

    //-----------------------------------------------------------------------------
    //  StealTasks
    //  Steals tasks from the other threads
    //-----------------------------------------------------------------------------
    bool CThread::StealTasks( void )
    {
        // Using this thread counter so we're not always trying to grab from the same thread
        static uint nThread = 0;
        nThread++;

        for( uint i = 0; i < m_pTaskManager->m_nNumThreads; ++i )
        {
            CThread* pThread = &m_pTaskManager->m_Thread[ (i + nThread) % m_pTaskManager->m_nNumThreads];

            if( pThread == this ) continue; // Don't steal from yourself, that's silly

            if( pThread->GiveUpWork( this ) )
            {
                // We sucessfully stole work!
                return true;
            }

            if( m_nNumTasks )
            {
                // We've been given work!
                return true;
            }
        }


        return false;
    }

    //-----------------------------------------------------------------------------
    //  Idle
    //  Idles the thread until it gets work
    //-----------------------------------------------------------------------------
    void CThread::Idle( void )
    {
        // Zzzzzzz.....
        System::SemaphoreRelease( &m_pTaskManager->m_pSleep );

        m_bAwake = false;
        // Wait until you're woken up
        System::WaitForCondition(&m_pWakeCondition, &m_pSystemMutex);
    }

    //-----------------------------------------------------------------------------
    //  MakeMainThread
    //  Attaches this CThread to the main program thread
    //-----------------------------------------------------------------------------
    void CThread::MakeMainThread( CTaskManager* pTaskManager )
    {
        m_pTaskManager = pTaskManager;
        m_pThread = System::GetCurrentThreadHandle();
        m_bFinished = false;
    }

    //-----------------------------------------------------------------------------
    //  Wake
    //  Wakes the thread up so it can get to work
    //-----------------------------------------------------------------------------
    void CThread::Wake( void )
    {
        if( m_bAwake == true )
        {
            // We're already awake
            return;
        }
        // Tell yourself to wake up
        System::SignalCondition( &m_pWakeCondition );
    }

} // namespace Riot
