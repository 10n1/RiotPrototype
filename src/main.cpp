/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/10/2011 12:28:59 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "Engine.h"
#include <stdlib.h> // For atexit on UNIX systems


int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( ReleasePool );

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::Run();

    return 0;
}
