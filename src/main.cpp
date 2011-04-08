/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 11:38:17 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include <stdlib.h> // For atexit on UNIX systems

#include <stdio.h>

int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    atexit( ReleasePool );

    ASSERT(true);
    
    ASSERT( false );
    
    printf( "hello ");

    return 0;
}
