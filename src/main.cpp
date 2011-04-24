/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/24/2011 12:20:29 AM
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

    sint x = -24;

    sint y = x % 13;

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::GetInstance()->Run();

    return y;
}
