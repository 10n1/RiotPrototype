/*********************************************************\
File:      main.cpp
Purpose:   Main entry point for the program
\*********************************************************/
#include "Common.h"
#include "Riot.h"
#include <xnamath.h>
#include "Scene\Component.h"

XMVECTOR v;

int main()
{
    //////////////////////////////////////////
    // Make sure Shutdown is the last thing called
    atexit( Riot::Shutdown );
    
    //////////////////////////////////////////
    // Run the game
    Riot::Run();

    return 0;
}
