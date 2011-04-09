/*********************************************************\
File:           TaskManager.h
Purpose:        Task manager
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 11:45:20 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TASKMANAGER_H_
#define _TASKMANAGER_H_
#include "common.h"
#include "Thread.h"

class CThread;

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

};



#endif // #ifndef _TASKMANAGER_H_
