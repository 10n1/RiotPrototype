/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 12:04:29 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_
#include "common.h"
#include "Thread.h"

class CTaskManager
{
public:
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    static void Initialize( void );
    
    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    static void Shutdown( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CThread     m_Thread[ MAX_THREADS ];
};



#endif // #ifndef _TASKMANAGER_H_
