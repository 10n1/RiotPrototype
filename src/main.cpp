/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/8/2011 11:41:43 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include <stdlib.h> // For atexit on UNIX systems
#include <stdio.h>

#include "System.h"
#include "TaskManager.h"

void TestFunc( void* pVoid )
{
    printf( "%d thread time!\n", *((int*)pVoid) );
}

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( ReleasePool );

    //////////////////////////////////////////
    // Initialize sub systems
    System::Initialize();
    CTaskManager::Initialize();

    int x = 83;
    CThread thread;
    thread.Start( TestFunc, &x );


    //////////////////////////////////////////
    //  Clean up subsystems
    CTaskManager::Shutdown();
    System::Shutdown();

    return 0;
}
