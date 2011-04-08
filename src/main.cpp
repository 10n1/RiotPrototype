/*********************************************************\
File:           main.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 11:38:17 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "common.h"
#include "timer.h"
#include <stdlib.h> // For atexit on UNIX systems

#include <emmintrin.h>

#include <stdio.h>

//#include <memory.h>


int main( int argc, char* argv[] )
{
    //////////////////////////////////////////
    // Free our memory last
    //atexit( ReleasePool );
    
    CTimer t;
    double fStartTime = 0.0f;
    double fEndTime = 0.0f;
    byte*  p = new byte[1024*1024];
    
    fStartTime = t.GetElapsedTime();
    
    for( uint i = 0; i < 1024*32; ++i )
    {
        fStartTime += 1024.0f;
        fStartTime /= 2.0f;
        
        byte* pNew = new byte[1024*1024];
        
        memcpy(pNew, p, sizeof(byte) * 1024 * 1024 );
        
        delete [] pNew;
        
        //printf( "%d\n", i );
    }
    
    fEndTime = t.GetElapsedTime();
    
    printf( "%f seconds, %f start\n", fEndTime, fStartTime );

    return 0;
}
