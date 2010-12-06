/*********************************************************\
File:      main.cpp
Purpose:   Main entry point for the program
\*********************************************************/
#include "RiotTypes.h"
#include "RiotMemory.h"
#include "IRefCounted.h"
#include <iostream>

int main()
{
    uint nTemp = 42;
    printf( "Hello world! %d\n", nTemp );

    int* pTest = new int[100];

    // DON'T CODE BELOW THIS LINE
    DumpMemoryLeaks();
    return 0;
}
