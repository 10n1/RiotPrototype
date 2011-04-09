/*********************************************************\
File:           Thread.h
Purpose:        Interface for hardware threads
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 11:44:48 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _THREAD_H_
#define _THREAD_H_
#include "common.h"
#include "System.h"

//////////////////////////////////////////
//  All user functions have this signature
typedef void (*WorkThread)(void*);

class CThread
{
public:
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
    void Start( WorkThread pFunc, void* pData );

    //-----------------------------------------------------------------------------
    //  _ThreadProc
    //  The most basic OS thread function
    //-----------------------------------------------------------------------------
#ifdef OS_WINDOWS
    static unsigned long _ThreadProc( void* pData );
#else
    static void* _ThreadProc( void* pData );
#endif

private:
    /***************************************\
    | class members                         |
    \***************************************/
    handle      m_pThread;
    WorkThread  m_pFunc;
    void*       m_pData;
    uint        m_nThreadId;
};



#endif // #ifndef _THREAD_H_
