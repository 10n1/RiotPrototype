/*********************************************************\
File:           memory.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 11:20:46 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "memory.h"
#include <malloc.h>

enum { GLOBAL_MEMORY_ALLOCATION = 16*1024*1024 };

static const byte* AllocateGlobalMemory( void );

static const byte*  gs_pGlobalPool      = AllocateGlobalMemory();
static byte*        gs_pCurrAlloc       = (byte*)gs_pGlobalPool;
static byte*        gs_pPrevAlloc       = NULL;
static uint         gs_nPrevAllocSize   = 0;
static uint         gs_nActiveMemory    = 0;

//-----------------------------------------------------------------------------
//  Allocates the main global array of memory, that everything else is allocated from
//-----------------------------------------------------------------------------
static const byte* AllocateGlobalMemory( void )
{
    byte* pAlloc = (byte*)malloc(GLOBAL_MEMORY_ALLOCATION);
    gs_pGlobalPool = pAlloc;
    return pAlloc;
}

//-----------------------------------------------------------------------------
//  Frees the array
//-----------------------------------------------------------------------------
void ReleasePool( void )
{
    if( gs_pGlobalPool )
    {
        free( (void*)gs_pGlobalPool );
    }
}


//-----------------------------------------------------------------------------
void* __cdecl operator new(size_t nSize)
{
    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = nSize;

    // Return it
    return pNewAlloc;
};

void* __cdecl operator new[](size_t nSize)
{
    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = nSize;

    // Return it
    return pNewAlloc;
};

void __cdecl operator delete(void* pVoid)
{
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
    if( pVoid == gs_pPrevAlloc )
    {
        // Move the pointer back down
        gs_pCurrAlloc = gs_pPrevAlloc;
        gs_nActiveMemory -= gs_nPrevAllocSize;

        // Now there is no previous allocation
        gs_pPrevAlloc = NULL;
    }
};
//-----------------------------------------------------------------------------
