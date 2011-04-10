/*********************************************************\
File:           Thread.h
Purpose:        Interface for hardware threads
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 6:10:27 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _THREAD_H_
#define _THREAD_H_
#include "common.h"
#include "System.h"

namespace Riot
{

class CTaskManager;

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
    System::thread_handle   m_pThread;
    CTaskManager*           m_pTaskManager;
    uint                    m_nThreadId;
    bool                    m_bFinished;
};

} // namespace Riot

#endif // #ifndef _THREAD_H_
