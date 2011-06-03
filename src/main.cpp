/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       5/9/2011 10:45:06 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "Engine.h"
#include <stdlib.h> // For atexit on UNIX systems

#include "scene/Terrain.h"

#undef new

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( Riot::Memory::ReleasePool );
    
    int* x = new int;

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::GetInstance()->Run();

    return 0;
}
