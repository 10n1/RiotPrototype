/*********************************************************\
File:           Thread.h
Purpose:        Interface for hardware threads
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 4:47:02 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _THREAD_H_
#define _THREAD_H_
#include "common.h"
#include "System.h"

namespace Riot
{    
    
    //////////////////////////////////////////
    //  Typedef for user functions
    typedef void (TaskFunc)( void* pData, uint nThreadId, uint nStart, uint nCount );
    typedef uint            task_handle_t;
    typedef volatile sint   task_completion_t;
    static const            task_handle_t INVALID_HANDLE = 0xFFFFFFFF;

    // Task definition
    struct TTask
    {
        TaskFunc*           pFunc;
        void*               pData;
        uint                nStart;
        uint                nCount;
        task_completion_t*  pCompletion;
    };

class CMutex
{
public:
    // CMutex constructor
    CMutex() : m_nLock( 0 ) { }

    /***************************************\
    | class methods                         |
    \***************************************/
    inline void Lock( void )
    {
        while( !TryLock() )
        {
        }
    }

    // Returns true when the lock is acquired
    inline bool TryLock( void )
    {
        return AtomicExchange( &m_nLock, 1 ) == 0;
    }

    void Unlock( void )
    {
        AtomicExchange( &m_nLock, 0 );
    }

private:
    /***************************************\
    | class members                         |
    \***************************************/
    volatile sint   m_nLock;
};


class CScopedMutex
{
public:
    // CScopedMutex constructor
    CScopedMutex( ) : m_pMutex( NULL ) { }
    CScopedMutex( CMutex* pMutex, bool bLock = true ) : m_pMutex( pMutex ) { if( bLock ) m_pMutex->Lock(); }

    // CScopedMutex destructor
    ~CScopedMutex() { if( m_pMutex ) m_pMutex->Unlock(); }

    /***************************************\
    | class methods                         |
    \***************************************/

    // Tests to see if the mutex is already locked.
    //  if it acquires it, it grabs the reference
    bool TryLock( CMutex* pMutex )
    {
        if( !pMutex->TryLock() )
            return false;

        m_pMutex = pMutex;
        return true;
    }

    // Allow unlocking early
    void Unlock( void )
    {
        if( m_pMutex )
        {
            m_pMutex->Unlock();
            m_pMutex = NULL;
        }
    }

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CMutex* m_pMutex;
};

class CTaskManager;

class CThread
{
    friend class CTaskManager;
public:
    
    //-----------------------------------------------------------------------------
    //  GetThreadId
    //  Returns the thread Id
    //-----------------------------------------------------------------------------
    uint GetThreadId( void );

//private:
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
    //  ThreadProc
    //  The thread's main loop
    //-----------------------------------------------------------------------------
    void ThreadProc( void );

    //-----------------------------------------------------------------------------
    //  MakeMainThread
    //  Attaches this CThread to the main program thread
    //-----------------------------------------------------------------------------
    void MakeMainThread( CTaskManager* pTaskManager );    
    
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

    //-----------------------------------------------------------------------------
    //  Wake
    //  Wakes the thread up so it can get to work
    //-----------------------------------------------------------------------------
    void Wake( void );
    
    //-----------------------------------------------------------------------------
    //  PushTask
    //  Pushes a task onto the top of the task queue
    //-----------------------------------------------------------------------------
    void PushTask( TTask& task );
    
    //-----------------------------------------------------------------------------
    //  PopTask
    //  Pops a task off the queue
    //-----------------------------------------------------------------------------
    bool PopTask( TTask* task );

    //-----------------------------------------------------------------------------
    //  DoWork
    //  The thread starts doing work. It'll steal more if it has to
    //-----------------------------------------------------------------------------
    void DoWork( task_completion_t* pCompletion );
        
    //-----------------------------------------------------------------------------
    //  GiveUpWork
    //  Gives up work to the idle thread
    //-----------------------------------------------------------------------------
    bool GiveUpWork( CThread* pIdleThread );

    //-----------------------------------------------------------------------------
    //  StealTasks
    //  Steals tasks from the other threads
    //-----------------------------------------------------------------------------
    bool StealTasks( void );
    
private:
    /***************************************\
    | class members                         |
     \***************************************/
    TTask                       m_pTasks[ MAX_TASKS_PER_THREAD ];
    System::thread_handle_t     m_pThread;
    System::wait_condition_t    m_pWakeCondition;
    System::mutex_t             m_pSystemMutex;
    CTaskManager*               m_pTaskManager;

    CMutex          m_TaskMutex;
    uint            m_nThreadId;
    uint            m_nNumTasks;
    bool            m_bAwake;
    volatile bool   m_bFinished;
};

} // namespace Riot

#endif // #ifndef _THREAD_H_
