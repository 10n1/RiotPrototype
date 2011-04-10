/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 8:32:48 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_
#include "common.h"
#include "Thread.h"

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
    CScopedMutex() { m_Mutex.Lock(); }

    // CScopedMutex destructor
    ~CScopedMutex() { m_Mutex.Unlock(); }

    /***************************************\
    | class methods                         |
    \***************************************/
private:
    /***************************************\
    | class members                         |
    \***************************************/
    CMutex  m_Mutex;
};

#define SCOPED_MUTEX CScopedMutex m;

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

    uint    m_nNumActiveThreads;
    bool    m_bShutdown;
    bool    m_bThreadsIdle;
};

} // namespace Riot


#endif // #ifndef _TASKMANAGER_H_
