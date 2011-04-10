/*********************************************************\
File:           Thread.h
Purpose:        Interface for hardware threads
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 10:35:41 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _THREAD_H_
#define _THREAD_H_
#include "common.h"
#include "System.h"

namespace Riot
{

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

class CTaskCompletion
{
public:
    // CTaskCompletion constructor
    CTaskCompletion() : m_nBusy( 0 ) { }

    /***************************************\
    | class methods                         |
    \***************************************/

    inline bool IsBusy( void ) const
    {
        return m_nBusy != 0;
    }

    void MarkBusy( void )
    {
        AtomicIncrement( &m_nBusy );
    }

    void MarkComplete( void )
    {
        AtomicDecrement( &m_nBusy );
    }

private:
    /***************************************\
    | class members                         |
    \***************************************/
    volatile sint   m_nBusy;
};


class CTaskManager;
class CTaskCompletion;
class CInternalTask;

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
    CInternalTask*          m_pTasks[ MAX_TASKS_PER_THREAD ];

    System::thread_handle   m_pThread;
    CTaskManager*           m_pTaskManager;
    CTaskCompletion*        m_pCurrentCompletion;

    CMutex                  m_TaskMutex;

    uint                    m_nNumTasks;

    uint                    m_nThreadId;
    bool                    m_bFinished;
};

} // namespace Riot

#endif // #ifndef _THREAD_H_
