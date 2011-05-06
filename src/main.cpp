/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       5/5/2011 9:05:52 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "Engine.h"
#include <stdlib.h> // For atexit on UNIX systems

#include "scene/Terrain.h"

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( Riot::Memory::ReleasePool );

    int x = sizeof( Riot::CTerrain );

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::GetInstance()->Run();

    return 0;
}
