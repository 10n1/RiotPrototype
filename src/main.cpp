/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/10/2011 2:26:13 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include <stdlib.h> // For atexit on UNIX systems
#include <stdio.h>

#ifndef OS_WINDOWS
#include <unistd.h>
#else
#include <Windows.h>
#define sleep( x ) Sleep( x * 1000 )
#endif

#include "System.h"
#include "TaskManager.h"
#include "timer.h"

using namespace Riot;

CTaskManager    gTaskManager;

void TestFunc( void* pData, uint nThreadId, uint nStart, uint nCount )
{
    static CMutex lock;


    lock.Lock();
    printf( "Thread %d\tcount %d:\t", nThreadId, nCount );
    for( uint i = nStart; i < nStart+nCount; ++i )
    {
        printf( "%d ", i );
    }
    printf( "\n" );
    lock.Unlock();
}

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( ReleasePool );

    //////////////////////////////////////////
    // Initialize sub systems
    System::Initialize();
    gTaskManager.Initialize();

    CTimer timer;
    timer.Reset();
    
    task_handle_t nTaskHandle = gTaskManager.PushTask( &TestFunc, 0, 1024, 9 );
    gTaskManager.WaitForCompletion( nTaskHandle );


    float fTime = timer.GetElapsedTime();

    printf("\n%f seconds\n", fTime);

    //////////////////////////////////////////
    //  Clean up subsystems
    gTaskManager.Shutdown();
    System::Shutdown();

    return 0;
}
