/*********************************************************\
File:           memory.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 11:20:46 PM
Modified by:    Kyle Weicht
 
TODO:           Add alignment support? Should be ultra easy
\*********************************************************/
#include "memory.h"

#ifdef ARCH_IA32
#include <emmintrin.h>
#else
#include <stdlib.h>
#endif

enum { GLOBAL_MEMORY_ALLOCATION = 16*1024*1024 };

static const byte* AllocateGlobalMemory( void );

static const byte*  gs_pGlobalPool      = AllocateGlobalMemory();
static byte*        gs_pCurrAlloc       = (byte*)gs_pGlobalPool;
static byte*        gs_pPrevAlloc       = NULL;
static uint         gs_nPrevAllocSize   = 0;
static uint         gs_nActiveMemory    = 0;


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

//-----------------------------------------------------------------------------
//  Frees the array
//-----------------------------------------------------------------------------
void ReleasePool( void )
{
    if( gs_pGlobalPool )
    {
        _mm_free( (void*)gs_pGlobalPool );
    }
}


//-----------------------------------------------------------------------------
void* __cdecl operator new(size_t nSize)
{
    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;

#ifdef RIOT_ALIGN_MEMORY
    nSize = RoundUp( nSize );
#endif
    
    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = (uint)nSize;

    // Return it
    return pNewAlloc;
};

void* __cdecl operator new[](size_t nSize)
{
    // Grab the current pointer
    byte* pNewAlloc = gs_pCurrAlloc;
    
#ifdef RIOT_ALIGN_MEMORY
    nSize = RoundUp( nSize );
#endif
    
    // Increment the counters
    gs_pCurrAlloc       += nSize;
    gs_nActiveMemory    += nSize;
    gs_pPrevAlloc       = pNewAlloc;
    gs_nPrevAllocSize   = (uint)nSize;
    
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

void* memcpy( void* pDest, const void* pSource, uint nSize )
{
#ifndef RIOT_USE_INTRINSICS
    uint64* a = (uint64*)pDest;
    const uint64* b = (const uint64*)pSource;
    
    while( nSize )
    {
        *a = *b;
        a++, b++;
        nSize -= sizeof( uint64 );
    }
    return pDest;
#else    
    byte* a = (byte*)pDest;
    const byte* b = (const byte*)pSource;
    
    uint nVectorWidth = sizeof( __m128 );
    
    while( nSize > 0 )
    {
        __m128 v0 =  _mm_loadu_ps( (float*)b );        
        _mm_storeu_ps( (float*)a, v0);
        
        a += nVectorWidth, b += nVectorWidth;
        nSize -= nVectorWidth;
    }
    
    return pDest;
#endif
}
