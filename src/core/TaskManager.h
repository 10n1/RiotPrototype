/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/24/2011 12:52:31 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_
#include "common.h"
#include "Thread.h"

namespace Riot
{

    class CTaskManager
    {
    public:

        friend class CThread;
        friend class Engine;

        /***************************************\
        | class methods                         |
        \***************************************/
        // CTaskManagerConstructor
        CTaskManager();

        // CTaskManagerDestructor
        ~CTaskManager();
    public:

        //-----------------------------------------------------------------------------
        //  Initialize
        //-----------------------------------------------------------------------------
        void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        void Shutdown( void );
        
        //-----------------------------------------------------------------------------
        //  GetInstance
        //-----------------------------------------------------------------------------
        inline static CTaskManager* GetInstance( void );

        //-----------------------------------------------------------------------------
        //  PushTask
        //  Adds the task to the queue
        //-----------------------------------------------------------------------------
        task_handle_t PushTask( TaskFunc* pFunc, void* pData, uint nCount, uint nChunkSize );

        //-----------------------------------------------------------------------------
        //  WaitForCompletion
        //  Works until the completion is done
        //-----------------------------------------------------------------------------
        void WaitForCompletion( task_handle_t nHandle );

    private:
        //-----------------------------------------------------------------------------
        //  WakeThreads
        //  Wakes up all the threads
        //-----------------------------------------------------------------------------
        void WakeThreads( void );

        //-----------------------------------------------------------------------------
        //  WaitForThreads
        //  Waits for all threads to be idle
        //-----------------------------------------------------------------------------
        void WaitForThreads( void );
        
        //-----------------------------------------------------------------------------
        //  GetWork
        //  Retrieves work for the thread to do
        //-----------------------------------------------------------------------------
        bool GetWork( TTask** ppTask );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CTaskManager*    m_pInstance;

        CThread             m_Thread[ MAX_THREADS ];

        TTask               m_pTasks[ MAX_SUB_TASKS ];
        atomic_t            m_pCompletion[ MAX_TASKS ];

        System::semaphore_t m_pSleep;

        CMutex      m_TaskMutex;

        atomic_t    m_nStartTask;
        atomic_t    m_nEndTask;
        atomic_t    m_nCurrentHandle;
        atomic_t    m_nActiveTasks;

        uint        m_nNumThreads;
        bool        m_bShutdown;
        bool        m_bThreadsIdle;
    };


    //-----------------------------------------------------------------------------
    //  GetInstance
    //-----------------------------------------------------------------------------
    inline CTaskManager* CTaskManager::GetInstance( void ) { return m_pInstance; }

} // namespace Riot


#endif // #ifndef _TASKMANAGER_H_
