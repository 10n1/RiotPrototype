/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/10/2011 4:30:20 AM
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

static CMutex lock;

#define NUM_TASKS 8192
#define TASK_CHUNK 64
#define NUM_MATH 2048*1

RMatrix4    matrixA[MAX_THREADS][NUM_MATH];
RMatrix4    matrixB[MAX_THREADS][NUM_MATH];
RMatrix4    matrixC[MAX_THREADS][NUM_MATH];

void TestFunc( void* pData, uint nThreadId, uint nStart, uint nCount )
{
    for( uint i = nStart; i < nStart+nCount; ++i )
    {
        for( uint j = 0; j < NUM_MATH; ++j )
        {
            matrixA[nThreadId][j].r0.x = i;
            matrixC[nThreadId][j] = matrixA[nThreadId][j] * matrixB[nThreadId][j];
        }
    }
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
    
    task_handle_t nTaskHandle = gTaskManager.PushTask( &TestFunc, 0, NUM_TASKS, TASK_CHUNK );
    //sleep( 5 );
    gTaskManager.WaitForCompletion( nTaskHandle );


    float fTime = timer.GetElapsedTime();

    printf("\n%f seconds\n", fTime);

    //////////////////////////////////////////
    //  Clean up subsystems
    gTaskManager.Shutdown();
    System::Shutdown();

    return 0;
}
