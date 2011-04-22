/*********************************************************\
File:           memory.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/17/2011 12:39:52 PM
Modified by:    Kyle Weicht

TODO:           Add alignment support? Should be ultra easy
\*********************************************************/
#include "memory.h"
#include "assert.h"

#ifdef ARCH_IA32
#include <emmintrin.h>
#else
// Arm doesn't support SSE
#include <stdlib.h>
#define _mm_malloc( nSize, nAlignment ) malloc( nSize )
#define _mm_free( pData ) free( pData )
#endif

// Included for sprintf/printf
#include <stdio.h>
#include <string.h>

enum { GLOBAL_MEMORY_ALLOCATION = 16*1024*1024 };

static const byte* AllocateGlobalMemory( void );

static const byte*  gs_pGlobalPool      = AllocateGlobalMemory();
static byte*        gs_pCurrAlloc       = (byte*)gs_pGlobalPool;
static byte*        gs_pPrevAlloc       = NULL;
static uint         gs_nPrevAllocSize   = 0;
static uint         gs_nActiveMemory    = 0;

#ifdef DEBUG
void AddAllocation(void* pData, uint nSize, const char* szFile, uint nLine);
void RemoveAllocation(void* pData);
void __cdecl DumpMemoryLeaks(void);
#else
#define AddAllocation()
#define RemoveAllocation()
#define DumpMemoryLeaks()
#endif // #ifdef DEBUG

//-----------------------------------------------------------------------------
//  Helper function to align new memory to 16 byte boundries
//-----------------------------------------------------------------------------
size_t RoundUp( size_t nSize )
{
    if( nSize & 0xF )
    {
        return (nSize + 0x10) & 0xFFFFFFF0;
    }
    return nSize;
}

//-----------------------------------------------------------------------------
//  Allocates the main global array of memory, that everything else is allocated from
//-----------------------------------------------------------------------------
static const byte* AllocateGlobalMemory( void )
{
    byte* pAlloc = (byte*)_mm_malloc(GLOBAL_MEMORY_ALLOCATION, 16);
    gs_pGlobalPool = pAlloc;
    return pAlloc;
}

namespace Riot
{
    namespace Memory
    {
        //-----------------------------------------------------------------------------
        //  Frees the array
        //-----------------------------------------------------------------------------
        void ReleasePool( void )
        {
            DumpMemoryLeaks();

            if( gs_pGlobalPool )
            {
                _mm_free( (void*)gs_pGlobalPool );
                gs_pGlobalPool = NULL;
            }
        }    
    } // namespace Memory
} // namespace Riot

//-----------------------------------------------------------------------------
#ifdef RIOT_USE_CUSTOM_ALLOCATOR

#ifdef DEBUG

void* __cdecl operator new(size_t nSize, const char* szFile, unsigned int nLine)
{
    ASSERT( gs_pGlobalPool );

    ASSERT( gs_nActiveMemory + nSize < GLOBAL_MEMORY_ALLOCATION );

    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

#ifdef RIOT_ALIGN_MEMORY
    nSize = RoundUp( nSize );
#endif

    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += (uint)nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = (uint)nSize;

    AddAllocation( pNewAlloc, nSize, szFile, nLine );

    // Return it
    return pNewAlloc;
};

void* __cdecl operator new[](size_t nSize, const char* szFile, unsigned int nLine)
{
    ASSERT( gs_pGlobalPool );

    ASSERT( gs_nActiveMemory + nSize < GLOBAL_MEMORY_ALLOCATION );

    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

#ifdef RIOT_ALIGN_MEMORY
    nSize = RoundUp( nSize );
#endif

    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += (uint)nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = (uint)nSize;

    AddAllocation( pNewAlloc, nSize, szFile, nLine );

    // Return it
    return pNewAlloc;
};


#else
void* __cdecl operator new(size_t nSize)
{
    ASSERT( gs_pGlobalPool );

    ASSERT( gs_nActiveMemory + nSize < GLOBAL_MEMORY_ALLOCATION );

    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

#ifdef RIOT_ALIGN_MEMORY
    nSize = RoundUp( nSize );
#endif

    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += (uint)nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = (uint)nSize;

    // Return it
    return pNewAlloc;
};

void* __cdecl operator new[](size_t nSize)
{
    ASSERT( gs_pGlobalPool );

    ASSERT( gs_nActiveMemory + nSize < GLOBAL_MEMORY_ALLOCATION );

    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

#ifdef RIOT_ALIGN_MEMORY
    nSize = RoundUp( nSize );
#endif

    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += (uint)nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = (uint)nSize;

    // Return it
    return pNewAlloc;
};

#endif // #ifdef DEBUG

void __cdecl operator delete(void* pVoid)
{
    RemoveAllocation( pVoid );

    if( pVoid == gs_pPrevAlloc )
    {
        // Move the pointer back down
        gs_pCurrAlloc = gs_pPrevAlloc;
        gs_nActiveMemory -= gs_nPrevAllocSize;

        // Now there is no previous allocation
        gs_pPrevAlloc = NULL;
    }
};

void __cdecl operator delete[](void* pVoid)
{
    RemoveAllocation( pVoid );

    if( pVoid == gs_pPrevAlloc )
    {
        // Move the pointer back down
        gs_pCurrAlloc = gs_pPrevAlloc;
        gs_nActiveMemory -= gs_nPrevAllocSize;

        // Now there is no previous allocation
        gs_pPrevAlloc = NULL;
    }
};
#endif // #ifdef RIOT_USE_CUSTOM_ALLOCATOR
//-----------------------------------------------------------------------------

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

#ifdef DEBUG

// Define vector to hold allocations
static MemoryAllocation g_pAllocations[1024*1024];
static uint g_nCurrentAllocations = 0;
static uint g_nCurrentMemoryUsage = 0;
static uint g_nMaxMemoryAllocatedAtOnce = 0;
static uint g_nTotalMemoryAllocated = 0;


void AddAllocation(void* pData, uint nSize, const char* szFile, uint nLine)
{
    MemoryAllocation allocation;
    allocation.nAddress = reinterpret_cast<nativeuint>(pData);
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
    nativeuint nAddress = reinterpret_cast<nativeuint>(pData);
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

        printf( szBuffer );
    }
    sprintf( szBuffer, "Total unfreed: %d bytes\n\n", nTotalUnfreed );
    printf( szBuffer );

    // Divide it into Kilobytes
    if( g_nTotalMemoryAllocated > ( 1024 * 8 ) )
    {
        sprintf( szBuffer, "Total Memory Allocated:\t\t%dK\n", g_nTotalMemoryAllocated >> 10 );
        printf( szBuffer );
        sprintf( szBuffer, "Max Memory Allocated at Once:\t%dK\n", g_nMaxMemoryAllocatedAtOnce >> 10);
        printf( szBuffer );
    }
    else
    {
        sprintf( szBuffer, "Total Memory Allocated:\t\t%d\n", g_nTotalMemoryAllocated );
        printf( szBuffer );
        sprintf( szBuffer, "Max Memory Allocated at Once:\t%d\n", g_nMaxMemoryAllocatedAtOnce );
        printf( szBuffer );
    }
    printf( "\n-----------------------------------------------------------------------------------------------------\n" );
}

#endif // #ifdef DEBUG