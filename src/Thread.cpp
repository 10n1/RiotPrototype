/*********************************************************\
File:           Thread.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 2:23:48 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Thread.h"
#include "TaskManager.h"

#include <Windows.h>

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
        TTask task;
        while( PopTask( &task ) )
        {
            TaskFunc* pFunc = task.pFunc;  
            void* pData     = task.pData;
            uint nStart     = task.nStart;
            uint nCount     = task.nCount;

            pFunc( pData, m_nThreadId, nStart, nCount );
            AtomicDecrement( task.pCompletion );
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
bool CThread::PopTask( TTask* task )
{
    // Lock the mutex and pop the task off
    m_TaskMutex.Lock();
    if( m_nNumTasks == 0 )
    {
        m_TaskMutex.Unlock();
        return false;
    }

    uint nTask = --m_nNumTasks;

    task->pFunc     = m_pTasks[nTask].pFunc;
    task->pData     = m_pTasks[nTask].pData;
    task->nStart    = m_pTasks[nTask].nStart;
    task->nCount    = m_pTasks[nTask].nCount;
    task->pCompletion   = m_pTasks[nTask].pCompletion;
    m_TaskMutex.Unlock();

    return true;
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
    // Tell yourself to wake up
    System::SignalCondition( &m_pWakeCondition );
}

} // namespace Riot
