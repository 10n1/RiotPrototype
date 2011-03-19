/*********************************************************\
File:       Memory.cpp
Purpose:    Memory allocation/tracking
\*********************************************************/
#include "Memory.h"
#include "Types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // For printf

#ifdef DEBUG

void AddAllocation(void* pData, uint nSize, const char* szFile, uint nLine);
void RemoveAllocation(void* pData);

void* __cdecl operator new( size_t nSize, const char* szFile, unsigned int nLine )
{
    void* p = malloc( nSize );
    // TODO: Handle out of memory error ( p == 0 )
    AddAllocation( p, (uint)nSize, szFile, nLine );
    return p;
};
void* __cdecl operator new[]( size_t nSize, const char* szFile, unsigned int nLine )
{
    void* p = malloc( nSize );
    // TODO: Handle out of memory error ( p == 0 )
    AddAllocation( p, (uint)nSize, szFile, nLine );
    return p;
};
void __cdecl operator delete(void* pVoid) throw()
{
    RemoveAllocation(pVoid);
    free(pVoid);
};
void __cdecl operator delete[](void* pVoid) throw()
{
    RemoveAllocation( pVoid );
    free( pVoid );
};

//-----------------------------------------------------------------------------
//    Memory allocation tracking
//-----------------------------------------------------------------------------
struct MemoryAllocation
{
    char        szFile[128];
    uint        nSize;
    uint        nLine;
    nativeuint  nAddress;
};

// Define vector to hold allocations
static MemoryAllocation g_pAllocations[512];
static uint g_nCurrentAllocations = 0;
static uint g_nCurrentMemoryUsage = 0;
static uint g_nMaxMemoryAllocatedAtOnce = 0;
static uint g_nTotalMemoryAllocated = 0;


void AddAllocation(void* pData, uint nSize, const char* szFile, uint nLine)
{
    MemoryAllocation allocation;
    allocation.nAddress = (nativeuint)(pData);
    allocation.nSize = nSize;
    strcpy(allocation.szFile, szFile);
    allocation.nLine = nLine;

    g_pAllocations[g_nCurrentAllocations++] = allocation;
    
    g_nCurrentMemoryUsage += nSize;
    g_nTotalMemoryAllocated += nSize;
    g_nMaxMemoryAllocatedAtOnce = ( g_nCurrentMemoryUsage > g_nMaxMemoryAllocatedAtOnce ) ? g_nCurrentMemoryUsage : g_nMaxMemoryAllocatedAtOnce;
}

void RemoveAllocation(void* pData)
{
    nativeuint nAddress = (nativeuint)(pData);
    for(uint i = 0; i < g_nCurrentAllocations; ++i)
    {
        if(g_pAllocations[i].nAddress == nAddress)
        {
            g_nCurrentMemoryUsage -= g_pAllocations[i].nSize;
            g_pAllocations[i] = g_pAllocations[--g_nCurrentAllocations];
            break;
        }
    }
}

void __cdecl DumpMemoryLeaks(void)
{
    char szBuffer[1024] = { 0 };
    uint nTotalUnfreed = 0;    
    printf( "\n----------------------------------------Dumping Memory Leaks-----------------------------------------\n" );
    for(uint i = 0; i < g_nCurrentAllocations; ++i)
    {
#if defined( _64BIT )
        sprintf( szBuffer, "%s, Line - %d:\t\tAddress - 0x%.16llu,\t\t%d unfreed\n",
#else
        sprintf( szBuffer, "%s, Line - %d:\t\tAddress - 0x%.8X,\t\t% ba d unfreed\n",
#endif //#if defined( _64BIT )
                    g_pAllocations[i].szFile,
                    g_pAllocations[i].nLine,
                    g_pAllocations[i].nAddress,
                    g_pAllocations[i].nSize );

        nTotalUnfreed += g_pAllocations[i].nSize;

        free( (void*)g_pAllocations[i].nAddress );

        printf( szBuffer );
    }
    sprintf( szBuffer, "Total unfreed: %d bytes\n\n", nTotalUnfreed );
    printf( szBuffer );

    sprintf( szBuffer, "Total Memory Allocated:\t\t%d\n", g_nTotalMemoryAllocated );
    printf( szBuffer );
    sprintf( szBuffer, "Max Memory Allocated at Once:\t%d\n", g_nMaxMemoryAllocatedAtOnce );
    printf( szBuffer );
    printf( "\n-----------------------------------------------------------------------------------------------------\n" );
}

#else // #if notdefined( _DEBUG )

void* __cdecl operator new(size_t nSize)
{
    return malloc( nSize );
};

void* __cdecl operator new[](size_t nSize)
{
    return malloc( nSize );
};

void __cdecl operator delete(void* pVoid) throw()
{
    free( pVoid );
};

void __cdecl operator delete[](void* pVoid) throw()
{
    free( pVoid );
};

#endif // #ifdef debug

