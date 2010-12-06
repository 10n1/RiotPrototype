#include "RiotTypes.h"
#include "RiotMemory.h"
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
