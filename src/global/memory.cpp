/*********************************************************\
File:           memory.cpp
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/10/2011 11:30:46 PM
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

namespace Memory
{
    //-----------------------------------------------------------------------------
    //  Frees the array
    //-----------------------------------------------------------------------------
    void ReleasePool( void )
    {
        if( gs_pGlobalPool )
        {
            _mm_free( (void*)gs_pGlobalPool );
            gs_pGlobalPool = NULL;
        }
    }
    
}


//-----------------------------------------------------------------------------
#ifdef RIOT_USE_CUSTOM_ALLOCATOR
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
#endif // #ifdef RIOT_USE_CUSTOM_ALLOCATOR
//-----------------------------------------------------------------------------

void* Memcpy( void* pDest, const void* pSource, uint nSize )
{
#if RIOT_USE_INTRINSICS
    byte* a = (byte*)pDest;
    const byte* b = (const byte*)pSource;

    const uint nWidth = sizeof( __m128 );
    __m128 v0;

    while( nSize >= nWidth )
    {
        v0 =  _mm_loadu_ps( (float*)b );
        b += nWidth;

        _mm_storeu_ps( (float*)a, v0);
        a += nWidth;

        nSize -= nWidth;
    }

    while( nSize > 0 )
    {
        *a = *b;
        a++, b++;
        --nSize;
    }
#else    
    uint64* a = (uint64*)pDest;
    const uint64* b = (const uint64*)pSource;

    const uint nWidth = sizeof( uint64 );

    while( nSize )
    {
        *a = *b;
        a++, b++;
        nSize -= nWidth;
    }


    while( nSize > 0 )
    {
        *a = *b;
        a++, b++;
        nSize--;
    }
#endif // #ifndef RIOT_USE_INTRINSICS

    return pDest;
}

void* Memset( void* pDest, uint c, uint nSize )
{
#if RIOT_USE_INTRINSICS
    byte* a = (byte*)pDest;

    const uint nVectorWidth = sizeof( __m128 );
    __m128 vFill = _mm_load1_ps( (float*)&c );

    while( nSize >= nVectorWidth )
    {
        _mm_storeu_ps( (float*)a, vFill);
        a += nVectorWidth;

        nSize -= nVectorWidth;
    }
    
    while( nSize > 0 )
    {
        *a = c;
        a++;
        --nSize;
    }
#else
    uint64* a = (uint64*)pDest;

    while( nSize > 0 )
    {
        *a = c;
        a++;
        nSize -= sizeof( uint64 );
    }
#endif // #ifndef RIOT_USE_INTRINSICS

    return pDest;
}
