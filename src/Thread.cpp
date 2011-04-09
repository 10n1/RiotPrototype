/*********************************************************\
File:           Thread.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 11:44:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Thread.h"
#include "System.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#else
#endif

// CThread constructor
CThread::CThread()
    : m_pThread( 0 )
    , m_pFunc( 0 )
    , m_nThreadId( 0 )
{
}

// CThread destructor
CThread::~CThread()
{
}
/***************************************\
| class methods                         |
\***************************************/

//-----------------------------------------------------------------------------
//  Start
//  Starts the thread, running the input function, then returning
//-----------------------------------------------------------------------------
void CThread::Start( WorkThread pFunc, void* pData )
{
    m_pFunc = pFunc;
    m_pData = pData;
    m_nThreadId = 0;

#ifdef OS_WINDOWS
    m_pThread = System::SpawnThread( &_ThreadProc, this );
#else
#endif
}

//-----------------------------------------------------------------------------
//  _ThreadProc
//  The most basic OS thread function
//-----------------------------------------------------------------------------
#ifdef OS_WINDOWS
unsigned long CThread::_ThreadProc( void* pData )
#else
void* CThread::_ThreadProc( void* pData )
#endif
{
    CThread* pThread = (CThread*)pData;

    pThread->m_pFunc( pThread->m_pData );

    return 0;
}
