/*********************************************************\
File:           TaskManager.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/23/2011 8:46:48 PM
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
        Memset( (void*)m_pCompletion, 0, sizeof( m_pCompletion ) );

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

    static atomic_t nCompletion[ MAX_TASKS ] = { 0 };

    //-----------------------------------------------------------------------------
    //  PushTask
    //  Adds the task to the queue
    //-----------------------------------------------------------------------------
    task_handle_t CTaskManager::PushTask( TaskFunc* pFunc, void* pData, uint nCount, uint nChunkSize )
    {
        CScopedMutex lock( &m_TaskMutex );

        nChunkSize = nCount;

        if( nChunkSize == 0 )
            nChunkSize = 1;

        // First we need to find a new handle to use
        task_handle_t   nHandle = (AtomicIncrement( &m_nCurrentHandle ) - 1) % MAX_TASKS;

        // Grab a pointer to its completion
        atomic_t* pCompletion = &m_pCompletion[nHandle];
        *pCompletion = 0;
        
        // Let the threads know theres another active task
        AtomicIncrement( &m_nActiveTasks );

        uint nTaskIndex = -1;
        // Distribute the tasks to the threads
        uint nStart = 0;
        uint nNewTaskCount = nChunkSize;

        while( nStart < nCount )
        {
            nTaskIndex = (AtomicIncrement( &m_nEndTask ) - 1) % MAX_SUB_TASKS;

            if( nCount - nStart < nChunkSize )
            {   // We're at the end, don't do a full chunk
                nNewTaskCount = nCount - nStart;
            }

            m_pTasks[ nTaskIndex ].pFunc        = pFunc;
            m_pTasks[ nTaskIndex ].pData        = pData;
            m_pTasks[ nTaskIndex ].nStart       = nStart;
            m_pTasks[ nTaskIndex ].nCount       = nNewTaskCount;
            m_pTasks[ nTaskIndex ].pCompletion  = pCompletion;

            AtomicIncrement( &m_nActiveTasks );

            nStart += nChunkSize;

            AtomicIncrement( pCompletion );
        }
        
        AtomicDecrement( &m_nActiveTasks );

        // Make sure the threads are awake
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
        sint nCompletion = AtomicCompareAndSwap( m_pTasks[nHandle].pCompletion, -1024, -1024 );
        while( nCompletion > 0 )
        {
            m_Thread[0].DoWork();

            nCompletion = AtomicCompareAndSwap( m_pTasks[nHandle].pCompletion, -1024, -1024 );
        }

        int x = 0;
    }

    //-----------------------------------------------------------------------------
    //  GetWork
    //  Retrieves work for the thread to do
    //-----------------------------------------------------------------------------
    bool CTaskManager::GetWork( TTask** ppTask )
    {
        CScopedMutex lock( &m_TaskMutex );

        // Grab work
        sint nActiveTasks = AtomicDecrement( &m_nActiveTasks );
        if( nActiveTasks < 0 )
        {
            // You grabbed work when there was none to grab
            AtomicIncrement( &m_nActiveTasks );
            return false;
        }

        // Grab the back of the list
        uint nTaskIndex = (AtomicIncrement( &m_nStartTask ) - 1) % MAX_SUB_TASKS;

        //ASSERT( nTaskIndex < m_nEndTask );
        
        // return it
        *ppTask = &m_pTasks[nTaskIndex];

        return true;
    }

} // namespace Riot
