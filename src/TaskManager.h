/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 11:46:57 PM
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
    void PushTask( void );
    
    //-----------------------------------------------------------------------------
    //  WaitForCompletion
    //  Works until the completion is done
    //-----------------------------------------------------------------------------
    void WaitForCompletion( void );

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
    CThread             m_Thread[ MAX_THREADS ];
    System::semaphore_t m_pSleep;
    System::semaphore_t m_pWake;

    uint    m_nNumThreads;
    bool    m_bShutdown;
    bool    m_bThreadsIdle;
};

} // namespace Riot


#endif // #ifndef _TASKMANAGER_H_
