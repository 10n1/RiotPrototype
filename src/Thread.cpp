/*********************************************************\
File:           Thread.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 10:35:05 PM
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
    m_pTaskManager  = pTaskManager;
    m_bFinished     = false;
    m_nNumTasks     = 0;
    m_pCurrentCompletion = NULL;

    m_pThread = System::ThreadSpawn( &_ThreadProc, this );
}

//-----------------------------------------------------------------------------
//  Destroy
//  Destroys the thread
//-----------------------------------------------------------------------------
void CThread::Destroy( void )
{
    System::DestroyThread( m_pThread );
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
        // Work if theres work to be done
        while( m_pTaskManager->m_pMainTaskCompletion )
        {
            DoWork( NULL );

            // If the task manager is shutting down, exit
            if( m_pTaskManager->m_bShutdown )
                break;
        }
        
        // Idle the thread
        Idle();

    } while( !m_pTaskManager->m_bShutdown );

    m_bFinished = true;
}

//-----------------------------------------------------------------------------
//  Idle
//  Idles the thread until it gets work
//-----------------------------------------------------------------------------
void CThread::Idle( void )
{
    System::SemaphoreRelease( &m_pTaskManager->m_pSleep );

    System::WaitForSemaphore( &m_pTaskManager->m_pWake );
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


bool CThread::PushTask(CInternalTask* pTask)
{
    if( _PushTask( pTask ) )
    {
        return true;
    }

    pTask->Run( this );

    return false;
}

bool CThread::_PushTask(CInternalTask* pTask)
{
    // Singlethreaded, don't bother
    if( m_pTaskManager->m_nNumThreads < 2 )
        return false;

    if( m_pTaskManager->m_pMainTaskCompletion == NULL )
    {
        // If theres no main completion, this must be the
        //  main thread
        ASSERT( m_nThreadId == 0 );

        // Prepare the threads to be ready
        //  (they should already be)
        m_pTaskManager->WaitForThreads();
        
        if( pTask->Spread( m_pTaskManager ) )
        {
            // Make this the main task and get to work
            m_pTaskManager->m_pMainTaskCompletion = pTask->m_pCompletion;
            m_pTaskManager->WakeThreads();

            return true;
        }
    }

    {
        // Lock our list to make sure it doesn't change
        CScopedMutex lock( &m_TaskMutex );
        // make sure we aren't full already
        if( m_nNumTasks >= MAX_TASKS_PER_THREAD )
        {
            return false;
        }
        
        // Add the task to our list
        pTask->m_pCompletion->MarkBusy();
        m_pTasks[ m_nNumTasks++ ] = pTask;
    }
    
    if( m_pTaskManager->m_pMainTaskCompletion == NULL )
    {
        m_pTaskManager->m_pMainTaskCompletion = pTask->m_pCompletion;
        m_pTaskManager->WakeThreads();
    }

    return true;
}

bool CThread::PopTask(CInternalTask** ppTask)
{
    CScopedMutex mutex( &m_TaskMutex );
    
    if( m_nNumTasks == 0 )
    {
        // We don't have any tasks to pop
        return false;
    }

    CInternalTask* pTask = m_pTasks[m_nNumTasks-1];

    // See if we can give up part of the task
    if( pTask->PartialPop( this, ppTask ) )
    {
        pTask->m_pCompletion->MarkBusy();
        return true;
    }

    // Give up the task
    *ppTask = pTask;
    --m_nNumTasks;

    return true;
}

bool CThread::StealTasks()
{
    // Try to steal from a random thread 
    uint nNumThreads = m_pTaskManager->m_nNumThreads;
    uint nThreadOffset = (m_nThreadId + 0) % nNumThreads;

    for( uint nThread = 0; nThread < nNumThreads; ++nThread )
    {
        CThread* pThread = &m_pTaskManager->m_Thread[ (nThread + nThreadOffset) % nNumThreads ];

        if( pThread == this ) // Don't steal work from self
            continue;

        if( pThread->GiveUpSomeWork( this ) )
        {
            // We just got some work!
            return true;
        }

        if( m_nNumTasks )
        {
            //  We've been given some work!
            return true;
        }
    }

    return false;
}

bool CThread::GiveUpSomeWork(CThread* pIdleThread)
{
    // If our task list is locked just return
    CScopedMutex scopedMutex;
    if( !scopedMutex.TryLock( &m_TaskMutex ) )
    {
        return false;
    }
    //  Do we have tasks to give up?
    if( m_nNumTasks == 0 )
    {
        return false;
    }

    uint            nGrabCount = 0;
    uint            nTask = 0;
    CInternalTask** ppNewTasks = NULL;

    // Lock the other threads mutex
    CScopedMutex idleMutex( &pIdleThread->m_TaskMutex );

    if( pIdleThread->m_nNumTasks )
    {
        // It's already been given tasks
        return false;
    }

    // We only have one task, split it
    if( m_nNumTasks == 1 )
    {
        CInternalTask*  pTask = NULL;

        if( m_pTasks[0]->Split( pIdleThread, &pTask ) )
        {
            pTask->m_pCompletion->MarkBusy();

            pIdleThread->m_pTasks[0]    = pTask;
            pIdleThread->m_nNumTasks    = 1;
            return true;
        }

        // Couldn't be split
        return false;
    }

    // Give the idle thread half of the tasks
    nGrabCount = (m_nNumTasks+1) / 2;

    ppNewTasks = pIdleThread->m_pTasks;
    for( nTask = 0; nTask < nGrabCount; ++nTask )
    {
        // Give the task to the other thread
        *ppNewTasks = m_pTasks[nTask];
        ++ppNewTasks;

        // Remove our reference to it
        m_pTasks[nTask] = NULL;
    }
    pIdleThread->m_nNumTasks = nGrabCount;

    // Unlock the idle thread so it can start working
    //idleMutex.Unlock();

    // Move our tasks down
    ppNewTasks = m_pTasks;
    for( ; nTask < m_nNumTasks; ++nTask )
    {
        *ppNewTasks = m_pTasks[nTask];
        ++ppNewTasks;
    }
    m_nNumTasks -= nGrabCount;

    return true;
}

void CThread::WorkUntilDone(CTaskCompletion* pCard)
{
    while( pCard->IsBusy() )
    {
        DoWork( pCard );
    }

    if( m_pTaskManager->m_pMainTaskCompletion == pCard )
    {
        // This is the root task, once it's done, theres nothing more
        m_pTaskManager->m_pMainTaskCompletion = NULL;
    }
}

void CThread::DoWork(CTaskCompletion* pCard)
{
    do
    {
        CInternalTask*  pTask            = NULL;
        CTaskCompletion* pLastCompletion = NULL;

        while( PopTask( &pTask ) )
        {
            // Save our current completion
            pLastCompletion = m_pCurrentCompletion;

            // We're now working on the new one
            m_pCurrentCompletion = pTask->m_pCompletion;

            pTask->Run( this ); // Run the task

            // Let the task know we're done working on it
            m_pCurrentCompletion->MarkComplete();

            // Restore our last completion
            m_pCurrentCompletion = pLastCompletion;

            // If we're explicly working and have finished, return
            if( pCard && !pCard->IsBusy() )
                return;
        }

        // Theres no more work to be done
        if( m_pTaskManager->m_pMainTaskCompletion == NULL )
            return;

    } while ( StealTasks() );
}


} // namespace Riot
    