/*********************************************************\
File:           TaskManager.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/23/2011 6:28:18 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "TaskManager.h"
#include "System.h"

#include <intrin.h>

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

    static atomic_t nTasks = 0;

    //-----------------------------------------------------------------------------
    //  PushTask
    //  Adds the task to the queue
    //-----------------------------------------------------------------------------
    task_handle_t CTaskManager::PushTask( TaskFunc* pFunc, void* pData, uint nCount, uint nChunkSize )
    {
        CScopedMutex lock( &m_PushMutex );

        if( nChunkSize == 0 )
            nChunkSize = 1;

        //sint n = AtomicIncrement( &nTasks );
        
        // Increment the task counter so threads know work is coming
        AtomicIncrement( &m_nActiveTasks );

        // First we need to find a new handle to use
        task_handle_t nHandle = (AtomicIncrement( &m_nEndTask ) - 1) % MAX_TASKS;

        //while( m_pTasks[nHandle].nStart < (sint)m_pTasks[nHandle].nCount )
        //{
        //    ; // Our task buffer is somehow filled, spin while waiting
        //    //  TODO: Use this time to do some work, but this is very unlikely to ever be hit
        //}

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

            nHandle,
        };


        // Distribute the tasks to the threads
        m_pTasks[nHandle] = newTask;

        //printf( "Task %d push. Completion: %d\n", nHandle, m_pTasks[nHandle].nCompletion );

        WakeThreads();

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
        while( m_pTasks[nHandle].nCompletion > 0 )
        {
            m_Thread[0].DoWork();
        }

        int x = 0;
    }

//#define AtomicCompareAndSwapMacro( pValue, nNewValue, nComparison ) _InterlockedCompareExchange( (volatile long*)pValue, nNewValue, nComparison )
//#define AtomicAddMacro( pValue, nValue ) _InterlockedExchangeAdd( (volatile long*)pValue, nValue ) + nValue
//#define AtomicIncrementMacro( pValue ) _InterlockedIncrement( (volatile long*)pValue )
//#define AtomicDecrementMacro( pValue ) _InterlockedDecrement( (volatile long*)pValue )

    //-----------------------------------------------------------------------------
    //  GetWork
    //  Retrieves work for the thread to do
    //-----------------------------------------------------------------------------
    bool CTaskManager::GetWork( TTask* pTask )
    {
        CScopedMutex lock( &m_PushMutex );

        uint nStartTask = AtomicCompareAndSwap( &m_nStartTask, m_nStartTask, -1 );
        uint nEndTask = AtomicCompareAndSwap( &m_nEndTask, m_nEndTask, -1 );

        if( nStartTask == nEndTask )
        {
            return false;
        }

        TTask newTask;

        // Get the current task we're looking at
        // We use CompareAndSwap with -1 because it will never pass.
        // This way we get an atomic lock-free read
        sint nTaskIndex = nStartTask % MAX_TASKS;

        uint nEnd = AtomicAdd( &m_pTasks[nTaskIndex].nStart, m_pTasks[nTaskIndex].nChunkSize );
        //uint nStart = m_pTasks[nTaskIndex].nStart;

        //AtomicAdd( &m_pTasks[nTaskIndex].nStart, m_pTasks[nTaskIndex].nChunkSize );
        //AtomicAdd( &m_pTasks[nTaskIndex].nStart, m_pTasks[nTaskIndex].nChunkSize );

        uint nStart = nEnd - m_pTasks[nTaskIndex].nChunkSize;

        if( nEnd > m_pTasks[nTaskIndex].nCount )
        {
            sint nOldStart = AtomicCompareAndSwap( &m_nStartTask, nStartTask + 1, nStartTask );
            if( nOldStart == nStartTask )
            {
                // This thread is the one that changed the start
                AtomicDecrement( &m_pTasks[nTaskIndex].nCompletion );
                AtomicDecrement( &m_nActiveTasks );
            }
            //AtomicIncrement( &m_nStartTask );
            newTask.pFunc = NULL;
            *pTask = newTask;
            return true;
        }

        newTask = m_pTasks[nTaskIndex];
        newTask.nStart = nStart;

        uint nChunkSize = newTask.nChunkSize;
        uint nCount     = newTask.nCount;
        newTask.nCount  = nChunkSize;

        // Calculate the start and end
        //sint nEnd = newTask.nStart + nChunkSize ;
        //sint nStart = nEnd - nChunkSize;

        //printf( "Task %d start. Completion: %d\n", nTaskIndex, m_pTasks[nTaskIndex].nCompletion );

        if( nEnd > nCount )
        {
            // This is the last task, we can't do a full chunk
            newTask.nCount = nCount - nStart;
        }
        

        *pTask = newTask;

        return true;
    }

} // namespace Riot
