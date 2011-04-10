/*********************************************************\
File:           Thread.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 8:47:23 PM
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

    m_pThread = System::ThreadSpawn( &_ThreadProc, this );
}

//-----------------------------------------------------------------------------
//  Destroy
//  Destroys the thread
//-----------------------------------------------------------------------------
void CThread::Destroy( void )
{
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
    return true;
}

bool CThread::_PushTask(CInternalTask* pTask)
{
    return true;
}

bool CThread::PopTask(CInternalTask** ppTask)
{
    return true;
}

bool CThread::StealTasks()
{
    return true;
}

bool CThread::GiveUpSomeWork(CThread* pIdleThread)
{
    return true;
}

void CThread::WorkUntilDone(CTaskCompletion* pCard)
{
}

void CThread::DoWork(CTaskCompletion* pCard)
{
    do
    {
        CInternalTask*  pTask;

    } while ( StealTasks() );
}


} // namespace Riot
    