/*********************************************************\
File:           TaskManager.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/23/2011 1:41:19 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "TaskManager.h"
#include "System.h"

namespace Riot
{
    /***************************************\
    | class members                         |
    \***************************************/
    CTaskManager* CTaskManager::m_pInstance = NULL;

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
        m_bShutdown     = false;
        m_bThreadsIdle  = false;
        m_nCurrentHandle= 0;
        m_nActiveTasks  = 0;

        m_nStartTask = 0;
        m_nEndTask = 0;

        Memset( m_pTasks, 0, sizeof( m_pTasks ) );

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
        CScopedMutex lock( &m_PushMutex );

        ASSERT( m_nStartTask < MAX_TASKS );
        ASSERT( m_nEndTask < MAX_TASKS );

        if( nChunkSize == 0 )
            nChunkSize = 1;

        // First we need to find a new handle to use
        task_handle_t nHandle = AtomicIncrement( &m_nEndTask ) - 1;
        AtomicCompareAndSwap( &m_nEndTask, 0, MAX_TASKS );

        while( m_pTasks[nHandle].nStart < (sint)m_pTasks[nHandle].nCount )
        {
            ; // Our task buffer is somehow filled, spin while waiting
            //  TODO: Use this time to do some work, but this is very unlikely to ever be hit
        }
        
        ASSERT( m_nStartTask < MAX_TASKS );
        ASSERT( m_nEndTask < MAX_TASKS );

        // Create our new task
        TTask   newTask = 
        { 
            pFunc,      // Function
            pData,      // Void* data
            0,          // start index
            nCount,     // the count
            nChunkSize, // The chunk size
            1,          // The current completion count
                        //  its initialized to one and becomes zero once
                        //  all the work has been completed
        };

        AtomicIncrement( &m_nActiveTasks );

        // Distribute the tasks to the threads
        m_pTasks[nHandle] = newTask;

        WakeThreads();
        ASSERT( m_nStartTask < MAX_TASKS );
        ASSERT( m_nEndTask < MAX_TASKS );

        return nHandle;
    }

    //-----------------------------------------------------------------------------
    //  WaitForCompletion
    //  Works until the completion is done
    //-----------------------------------------------------------------------------
    void CTaskManager::WaitForCompletion( task_handle_t nHandle )
    {
        if( nHandle == TASK_INVALID_HANDLE )
        {
            // Not a valid task, just return
            return;
        }

        // While we're waiting, work
        //while( m_pTasks[nHandle].nCompletion > 0 )
        {
            m_Thread[0].DoWork();
        }

        int x = 0;
    }


    //-----------------------------------------------------------------------------
    //  GetWork
    //  Retrieves work for the thread to do
    //-----------------------------------------------------------------------------
    bool CTaskManager::GetWork( TTask** ppTask, sint* pStart, sint* pCount )
    {
        CScopedMutex lock( &m_PopMutex );

        ASSERT( m_nStartTask < MAX_TASKS );
        ASSERT( m_nEndTask < MAX_TASKS );

        uint nStartTask = AtomicCompareAndSwap( &m_nStartTask, m_nStartTask, -1 );
        uint nEndTask = AtomicCompareAndSwap( &m_nEndTask, m_nEndTask, -1 );

        if( nStartTask == nEndTask )
        {
            return false;
        }

        // Get the current task we're looking at
        // We use CompareAndSwap with -1 because it will never pass.
        // This way we get an atomic lock-free read
        sint nTaskIndex = AtomicCompareAndSwap( &m_nStartTask, m_nStartTask, -1 );

        uint nChunkSize = m_pTasks[nTaskIndex].nChunkSize;
        uint nCount     = m_pTasks[nTaskIndex].nCount;
        uint nNewCount  = nChunkSize;

        // Calculate the start and end
        sint nEnd = AtomicAdd( &m_pTasks[nTaskIndex].nStart, nChunkSize );
        sint nStart = nEnd - nChunkSize;

        if( nStart >= nCount )
        {
            // we're currently starting past the end,
            //  this task is clearly done.
            // If it hasn't been incremented, increment the start, then try again
            AtomicDecrement( &m_nActiveTasks );
            AtomicDecrement( &m_pTasks[nTaskIndex].nCompletion );
            AtomicCompareAndSwap( &m_nStartTask, nTaskIndex + 1, nTaskIndex );
            AtomicCompareAndSwap( &m_nStartTask, 0, MAX_TASKS );
        ASSERT( m_nStartTask < MAX_TASKS );
        ASSERT( m_nEndTask < MAX_TASKS );
        lock.Unlock();
            return GetWork( ppTask, pStart, pCount );
        }

        // Let the task know its being worked on
        AtomicIncrement( &m_pTasks[nTaskIndex].nCompletion );

        if( nEnd > nCount )
        {
            // This is the last task, we can't do a full chunk
            nNewCount = nCount - nStart;
        }

        *ppTask = &m_pTasks[nTaskIndex];
        *pStart = nStart;
        *pCount = nNewCount;

        return true;
    }

} // namespace Riot
