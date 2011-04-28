/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/28/2011 11:51:03 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "Engine.h"
#include <stdlib.h> // For atexit on UNIX systems

#include "scene/ComponentCollidable.h"

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( Riot::Memory::ReleasePool );

    Riot::CComponentCollidable::Plane p( RVector3( 0.0f, 1.0f, 0.0f ), RVector3Zero() );

    RVector3 test = RVector3( 0.0f, -12.0f, 0.0f );

    float fDist = p.DistanceFrom( test.f );

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::GetInstance()->Run();

    return 0;
}
