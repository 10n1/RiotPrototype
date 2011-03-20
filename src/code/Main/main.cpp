/*********************************************************\
File:      main.cpp
Purpose:   Main entry point for the program
\*********************************************************/
#include "Riot.h"

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
