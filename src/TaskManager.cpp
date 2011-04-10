/*********************************************************\
File:           TaskManager.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 2:15:02 AM
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
    m_nCurrentTask          = 0;

    Memset( (void*)&m_nTaskCompletion[0], 0, sizeof( m_nTaskCompletion ) );

    //  Get the number of hardware threads
    m_nNumThreads = System::GetHardwareThreadCount();

    // Create semaphores
    m_pSleep = System::CreateRiotSemaphore( 0 );

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
}

//-----------------------------------------------------------------------------
//  WakeThreads
//  Wakes up all the threads
//-----------------------------------------------------------------------------
void CTaskManager::WakeThreads( void )
{
    ASSERT( m_bThreadsIdle );

    m_bThreadsIdle = false;
    for( uint i = 1; i < m_nNumThreads; ++i )
    {
        m_Thread[i].Wake();
    }
    
}

//-----------------------------------------------------------------------------
//  WaitForThreads
//  Waits for all threads to be idle
//-----------------------------------------------------------------------------
void CTaskManager::WaitForThreads( void )
{
    ASSERT( !m_bThreadsIdle );

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
task_handle_t CTaskManager::PushTask( TaskFunc* pFunc, void* pData, uint nCount, uint nChunkSize )
{
    WaitForThreads();

    // If we already have too many tasks, just run this task
    if( m_nCurrentTask >= MAX_TASKS )
    {
        uint i;
        for( i = 0; i < nCount; i += nChunkSize )
        {
            pFunc( pData, 0, i, nChunkSize );
        }

        // We went past the count, finish the rest
        if( i > nCount )
        {
            pFunc( pData, 0, i-nChunkSize, nCount-(i-nChunkSize) );
        }

        return INVALID_HANDLE;
    }

    // Create our new task
    TTask   newTask = 
    { 
        pFunc,              // Function
        pData,              // Void* data
        0,                  // start index
        nChunkSize,         // the count
        &m_nTaskCompletion[ m_nCurrentTask ]  
    };

    // Distribute the tasks to the threads
    uint nThread = 0;
    uint nStart = 0;
    while( nStart < nCount )
    {
        if( nCount - nStart < nChunkSize )
        {   // We're at the end, don't do a full chunk
            newTask.nCount = nCount - nStart;
        }
        newTask.nStart = nStart;

        if( nThread % m_nNumThreads == 0 )
        {
            nThread++;
        }
        m_Thread[nThread++ % m_nNumThreads].PushTask( newTask );

        nStart += nChunkSize;
    }

    // Make sure the threads are awake
    WakeThreads();

    return m_nCurrentTask++;
}

//-----------------------------------------------------------------------------
//  WaitForCompletion
//  Works until the completion is done
//-----------------------------------------------------------------------------
void CTaskManager::WaitForCompletion( task_handle_t nHandle )
{
    if( nHandle == INVALID_HANDLE )
    {
        // Not a valid task, just return
        return;
    }

    while( m_nTaskCompletion[nHandle] )
    {
        ;
    }
}

} // namespace Riot
