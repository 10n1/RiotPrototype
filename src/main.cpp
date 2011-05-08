/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       5/7/2011 10:26:48 AM
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
    x = sizeof( Riot::CTerrainTile );

    x = Riot::CTerrain::nMaxTerrainDistance;
    x = Riot::CTerrain::nTerrainTileDimensions;
    
    x = Riot::CTerrain::nNumHighTiles;
    x = Riot::CTerrain::nNumMedTiles;
    x = Riot::CTerrain::nNumLowTiles;

    //////////////////////////////////////////
    // Run the engine
    Riot::Engine::GetInstance()->Run();

    return 0;
}
