#include "RiotTypes.h"
#include "RiotMemory.h"
#include <iostream>

int main()
{
    uint nTemp = 42;
    printf( "Hello world! %d\n", nTemp );

    int* pTest = new int[100];
    float2* pVec2 = new float2();
    float3* pVec3 = new float3();
    float4* pVec4 = new float4();
    float3x3* pVec3x3 = new float3x3();
    float4x4* pVec4x4 = new float4x4();

    // DON'T CODE BELOW THIS LINE
    DumpMemoryLeaks();
    return 0;
}
