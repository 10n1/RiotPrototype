/*********************************************************\
File:           TaskManager.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 11:51:47 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "TaskManager.h"
#include "System.h"

namespace Riot
{

/***************************************\
| class methods                         |
\***************************************/
// CTaskManagerConstructor
CTaskManager::CTaskManager()
{
}

// CTaskManagerDestructor
CTaskManager::~CTaskManager()
{
}

//-----------------------------------------------------------------------------
//  Initialize
//-----------------------------------------------------------------------------
void CTaskManager::Initialize( void )
{
    m_bShutdown             = false;
    m_bThreadsIdle          = false;

    //  Get the number of hardware threads
    m_nNumThreads = System::GetHardwareThreadCount();

    // Create semaphores
    m_pSleep = System::SemaphoreCreate( 0 );
    m_pWake = System::SemaphoreCreate( 0 );

    // Attach thread 0 to the main thread
    m_Thread[0].MakeMainThread( this );

    // Start up all other threads
    for( uint i = 1; i < m_nNumThreads; ++i )
    {
        m_Thread[i].m_nThreadId = i;
        m_Thread[i].Start( this );
    }
}

//-----------------------------------------------------------------------------
//  Shutdown
//-----------------------------------------------------------------------------
void CTaskManager::Shutdown( void )
{
    m_bShutdown = true;

    // Wait for the threads, then wake them to shutdown
    WaitForThreads();
    WakeThreads();

    for( uint i = 1; i < m_nNumThreads; ++i )
    {
        while( m_Thread[i].m_bFinished == false )
        {
            ;
        }
    }

    System::SemaphoreDestroy( &m_pSleep );
    System::SemaphoreDestroy( &m_pWake );
}

//-----------------------------------------------------------------------------
//  WakeThreads
//  Wakes up all the threads
//-----------------------------------------------------------------------------
void CTaskManager::WakeThreads( void )
{
    //ASSERT( m_bThreadsIdle );

    m_bThreadsIdle = false;
    for( uint i = 1; i < m_nNumThreads; ++i )
    {
        System::SemaphoreRelease( &m_pWake );
    }
}

//-----------------------------------------------------------------------------
//  WaitForThreads
//  Waits for all threads to be idle
//-----------------------------------------------------------------------------
void CTaskManager::WaitForThreads( void )
{
    //ASSERT( !m_bThreadsIdle );

    for( uint i = 1; i < m_nNumThreads; ++i )
    {
        System::WaitForSemaphore( &m_pSleep );
    }

    m_bThreadsIdle = true;
}

//-----------------------------------------------------------------------------
//  PushTask
//  Adds the task to the queue
//-----------------------------------------------------------------------------
void CTaskManager::PushTask( void )
{
}

//-----------------------------------------------------------------------------
//  WaitForCompletion
//  Works until the completion is done
//-----------------------------------------------------------------------------
void CTaskManager::WaitForCompletion( void )
{
}

} // namespace Riot
