/*********************************************************\
File:           Thread.h
Purpose:        Interface for hardware threads
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 8:33:30 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _THREAD_H_
#define _THREAD_H_
#include "common.h"
#include "System.h"

namespace Riot
{

class CTaskManager;
class CTaskCompletion;
class CInternalTask;

class CThread
{
    friend class CTaskManager;

private:
    // CThread constructor
    CThread();

    // CThread destructor
    ~CThread();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Start
    //  Starts the thread, running the input function, then returning
    //-----------------------------------------------------------------------------
    void Start( CTaskManager* pTaskManager );
    
    //-----------------------------------------------------------------------------
    //  Destroy
    //  Destroys the thread
    //-----------------------------------------------------------------------------
    void Destroy( void );

    //-----------------------------------------------------------------------------
    //  ThreadProc
    //  The thread's main loop
    //-----------------------------------------------------------------------------
    void ThreadProc( void );

    //-----------------------------------------------------------------------------
    //  MakeMainThread
    //  Attaches this CThread to the main program thread
    //-----------------------------------------------------------------------------
    void MakeMainThread( CTaskManager* pTaskManager );
    
	bool PushTask(CInternalTask* pTask);	/* queue task if there is space, and run it otherwise */ 
	bool _PushTask(CInternalTask* pTask);	/* queue task if there is space (or do nothing) */ 
	
	bool PopTask(CInternalTask** ppTask);				/* pop task from queue */ 
	bool StealTasks();									/* fill queue with work from another thread */ 
	bool GiveUpSomeWork(CThread* pIdleThread);	/* request from an idle thread to give up some work */ 
	
	void WorkUntilDone(CTaskCompletion* pCard);
	void DoWork(CTaskCompletion* pCard);
    
    //-----------------------------------------------------------------------------
    //  _ThreadProc
    //  The most basic OS thread function
    //-----------------------------------------------------------------------------
    static System::thread_return_t _ThreadProc( void* pData );

    //-----------------------------------------------------------------------------
    //  Idle
    //  Idles the thread until it gets work
    //-----------------------------------------------------------------------------
    void Idle( void );
    
private:
    /***************************************\
    | class members                         |
    \***************************************/
    CInternalTask*          m_pTasks[ 256 ];

    System::thread_handle   m_pThread;
    CTaskManager*           m_pTaskManager;
    CTaskCompletion*        m_pCurrentCompletion;

    uint                    m_nNumTasks;

    uint                    m_nThreadId;
    bool                    m_bFinished;
};

} // namespace Riot

#endif // #ifndef _THREAD_H_
