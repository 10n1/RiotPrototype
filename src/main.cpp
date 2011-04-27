/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/26/2011 10:39:05 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "Engine.h"
#include <stdlib.h> // For atexit on UNIX systems

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( Riot::Memory::ReleasePool );

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::GetInstance()->Run();

    return 0;
}
