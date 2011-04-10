/*********************************************************\
File:           TaskManager.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 6:29:34 PM
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
    m_bShutdown = false;
    m_bThreadsIdle = false;

    //  Get the number of hardware threads
    m_nNumActiveThreads = System::GetHardwareThreadCount();

    // Create semaphores
    m_pSleep = System::SemaphoreCreate( 0 );
    m_pWake = System::SemaphoreCreate( 0 );

    // Attach thread 0 to the main thread
    m_Thread[0].MakeMainThread( this );

    // Start up all other threads
    for( uint i = 1; i < m_nNumActiveThreads; ++i )
    {
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

    for( uint i = 1; i < m_nNumActiveThreads; ++i )
    {
        while( m_Thread[i].m_bFinished == false )
        {
        }
        m_Thread[i].Destroy();
    }
}

//-----------------------------------------------------------------------------
//  WakeThreads
//  Wakes up all the threads
//-----------------------------------------------------------------------------
void CTaskManager::WakeThreads( void )
{
    m_bThreadsIdle = false;
    for( uint i = 0; i < m_nNumActiveThreads-1; ++i )
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
    for( uint i = 0; i < m_nNumActiveThreads-1; ++i )
    {
        System::WaitForSemaphore( &m_pSleep );
    }

    m_bThreadsIdle = true;
}

} // namespace Riot
