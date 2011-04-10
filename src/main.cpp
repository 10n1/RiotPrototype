/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/9/2011 11:47:28 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include <stdlib.h> // For atexit on UNIX systems
#include <stdio.h>
#include <unistd.h>

#include "System.h"
#include "TaskManager.h"
#include "timer.h"

using namespace Riot;

CTaskManager    gTaskManager;

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
    
    sleep( 10 );


    float fTime = timer.GetElapsedTime();

    printf("\n%f seconds\n", fTime);

    //////////////////////////////////////////
    //  Clean up subsystems
    gTaskManager.Shutdown();
    System::Shutdown();

    return 0;
}
