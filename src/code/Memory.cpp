/*********************************************************\
File:       Memory.cpp
Purpose:    Memory allocation/tracking
\*********************************************************/
#include "Memory.h"
#include "vector.h"

//-----------------------------------------------------------------------------
//    Memory allocation tracking, currently using aligned memory
//-----------------------------------------------------------------------------
#if defined( _DEBUG )
struct MemoryAllocation
{
    char        szFile[128];
    nativeuint  nAddress;
    uint        nSize;
    uint        nLine;
};

// Define vector to hold allocations
#include "vector.h"
static vector<MemoryAllocation> g_pAllocations;

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For OutputDebugString


void AddAllocation(void* pData, uint nSize, const char* szFile, uint nLine)
{
    MemoryAllocation allocation;
    allocation.nAddress = (nativeuint)pData;
    allocation.nSize = nSize;
    memcpy_s(allocation.szFile, 128, szFile, strlen(szFile)+1);
    allocation.nLine = nLine;
    g_pAllocations.push_back(allocation);
}

void RemoveAllocation(void* pData)
{
    nativeuint nAddress = (nativeuint)pData;
    for(uint i = 0; i < g_pAllocations.size(); ++i)
    {
        if(g_pAllocations[i].nAddress == nAddress)
        {
            g_pAllocations.quick_erase(i);
            break;
        }
    }
}

void DumpMemoryLeaks(void)
{
    char szBuffer[1024] = { 0 };
    uint nTotalUnfreed = 0;
    OutputDebugString( "\n----------------------------------------Dumping Memory Leaks-----------------------------------------\n" );
    for(uint i = 0; i < g_pAllocations.size(); ++i)
    {
#if defined( _M_X64 )
        sprintf_s(  szBuffer, 1024, "%s, Line - %d:\t\tAddress - 0x%.16X,\t\t%d unfreed\n",
#else
        sprintf_s(  szBuffer, 1024, "%s, Line - %d:\t\tAddress - 0x%.8X,\t\t%d unfreed\n",
#endif //#if defined( _M_X64 ) 
                    g_pAllocations[i].szFile,
                    g_pAllocations[i].nLine,
                    g_pAllocations[i].nAddress,
                    g_pAllocations[i].nSize );

        nTotalUnfreed += g_pAllocations[i].nSize;
        OutputDebugString( szBuffer );

        _aligned_free( (void*)g_pAllocations[i].nAddress );
    }
    sprintf_s( szBuffer, 1024, "Total unfreed: %d bytes\n", nTotalUnfreed );

    OutputDebugString(szBuffer);
    OutputDebugString( "-----------------------------------------------------------------------------------------------------\n\n" );
}

#endif // #if defined( _DEBUG )
