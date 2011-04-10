/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 6:10:27 PM
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

class CSemaphore
{
public:
    // CSemaphore constructor
    CSemaphore();

    // CSemaphore destructor
    /***************************************\
    | class methods                         |
    \***************************************/

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CMutex  m_Mutex;
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

    uint    m_nNumActiveThreads;
    bool    m_bShutdown;
    bool    m_bThreadsIdle;
};

} // namespace Riot


#endif // #ifndef _TASKMANAGER_H_
