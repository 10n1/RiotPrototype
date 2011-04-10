/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 3:15:39 PM
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
    private:

        friend class CThread;
    public:
        /***************************************\
        | class methods                         |
        \***************************************/
        // CTaskManagerConstructor
        CTaskManager();

        // CTaskManagerDestructor
        ~CTaskManager();

        //-----------------------------------------------------------------------------
        //  Initialize
        //-----------------------------------------------------------------------------
        void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        void Shutdown( void );

        //-----------------------------------------------------------------------------
        //  PushTask
        //  Adds the task to the queue
        //-----------------------------------------------------------------------------
        task_handle_t PushTask( TaskFunc* pFunc, void* pData, uint nCount, uint nChunkSize = 1 );

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

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        task_completion_t   m_nTaskCompletion[ MAX_TASKS ];
        CThread             m_Thread[ MAX_THREADS ];
        uint                m_nCurrentHandle;
        atomic_t            m_nActiveTasks;
        System::semaphore_t m_pSleep;

        uint    m_nNumThreads;
        bool    m_bShutdown;
        bool    m_bThreadsIdle;
    };

} // namespace Riot


#endif // #ifndef _TASKMANAGER_H_
