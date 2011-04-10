/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 9:59:59 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_
#include "common.h"
#include "Thread.h"

namespace Riot
{

//________________________________________________________________________________
class CInternalTask
{
public:
	CInternalTask(CTaskCompletion* pCompletion) 
	{
		m_pCompletion = pCompletion;
	}

public:
	virtual bool Run(CThread* pThread) =0;	/* does its work and suicides (or recycles)	*/ 
	
	virtual bool	Split(CThread* pThread, CInternalTask** ppTask) 
	{	/* Keep half the work and put the other half in a new task	*/ 
		return false;
	}	
		
	virtual bool	PartialPop(CThread* pThread, CInternalTask** ppTask) 
	{	/* returns a sub part of the task */ 
		return false;
	}

	virtual bool	Spread(CTaskManager* pPool) 
	{	/* share work across all threads (pool is idle) */ 
		return false;
	}	
	
	
public:
	CTaskCompletion*	m_pCompletion;
};

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
    void PushTask( CInternalTask* pTask );
    
    //-----------------------------------------------------------------------------
    //  WaitForCompletion
    //  Works until the completion is done
    //-----------------------------------------------------------------------------
    void WaitForCompletion( CTaskCompletion* pCompletion );

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

    volatile CTaskCompletion*   m_pMainTaskCompletion;

    uint    m_nNumThreads;
    bool    m_bShutdown;
    bool    m_bThreadsIdle;
};

} // namespace Riot


#endif // #ifndef _TASKMANAGER_H_
