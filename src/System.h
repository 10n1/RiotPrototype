/*********************************************************\
File:           System.h
Purpose:        Interface to the base system. All platform
                specific functions will be called here.
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 12:04:29 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "common.h"
namespace System
{
#ifdef OS_WINDOWS
    typedef unsigned long (_SystemThreadProc)( void* pData );
#else
    typedef void* (_SystemThreadProc)( void* pData );
#endif

    /***************************************\
    | class methods                         |
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void Initialize( void );
    
    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void Shutdown( void );

    //-----------------------------------------------------------------------------
    //  GetRunningTime
    //  Returns the time, in seconds, since the application was started
    //-----------------------------------------------------------------------------
    double GetRunningTime( void );
    
    //-----------------------------------------------------------------------------
    //  SpawnThread
    //  Creates and starts the thread, running until the function completes
    //-----------------------------------------------------------------------------
    handle SpawnThread( _SystemThreadProc* pFunc, void* pData );

    
    //-----------------------------------------------------------------------------
    //  GetHardwareThreadCount
    //  Returns the number of hardware threads in the system
    //-----------------------------------------------------------------------------
    uint GetHardwareThreadCound( void );
};

#endif // #ifndef _SYSTEM_H_
