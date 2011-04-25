/*********************************************************\
File:           utility.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/24/2011 6:28:44 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "utility.h"

#ifdef ARCH_IA32
#include <emmintrin.h>
#endif // #ifdef ARCH_IA32

void* Memcpy( void* pDest, const void* pSource, uint nSize )
{
#if RIOT_USE_INTRINSICS
    byte* a = (byte*)pDest;
    const byte* b = (const byte*)pSource;

    // If we're not 16-byte aligned, make it so
    if(  reinterpret_cast<nativeuint>(a) & 0xF )
    {
        // Go one byte at a time to get to an 4 byte boundry
        while( reinterpret_cast<nativeuint>(a) & 0x3 )
        {
            *a = *b;
            a++, b++;
            --nSize;
        }

        // Then go 4 bytes at at time
        uint32* a4 = (uint32*)a;
        const uint32* b4 = (const uint32*)b;
        const uint nWidth = sizeof( uint32 );
        while( reinterpret_cast<nativeuint>(a4) & 0xF )
        {
            *a4 = *b4;
            a++, b++;
            nSize -= nWidth;
        }

        a = (byte*)a4;
        b = (const byte*)b4;
    }

    // Then go 16 bytes at a time
    const uint nWidth = sizeof( __m128 );
    __m128 v0;

    if(  reinterpret_cast<nativeuint>(b) & 0xF )
    {   // if pSource is 16 byte aligned, we can use the aligned load
        while( nSize >= nWidth )
        {
            v0 =  _mm_load_ps( (float*)b );
            b += nWidth;

            _mm_store_ps( (float*)a, v0);
            a += nWidth;

            nSize -= nWidth;
        }
    }
    else
    {
        while( nSize >= nWidth )
        {
            v0 =  _mm_loadu_ps( (float*)b );
            b += nWidth;

            _mm_store_ps( (float*)a, v0);
            a += nWidth;

            nSize -= nWidth;
        }
    }

    // Finally get the rest of the buffer
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
    while( nSize >= nWidth )
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

    // First go byte by byte until we align on a 16 byte boundry
    while( reinterpret_cast<nativeuint>(a) & 0xF )
    {
        *(uint*)a = c;
        a += sizeof(uint);
        nSize -= sizeof(uint);
    }

    // Now go 16 bytes at a time
    const uint nVectorWidth = sizeof( __m128 );
    __m128 vFill = _mm_load1_ps( (float*)&c );

    while( nSize >= nVectorWidth )
    {
        _mm_store_ps( (float*)a, vFill);
        a += nVectorWidth;

        nSize -= nVectorWidth;
    }
    
    // Finally get the rest of the buffer
    while( nSize > 0 )
    {
        *a = (byte)c;
        a++;
        --nSize;
    }
#else
    uint64* a = (uint64*)pDest;
    const uint nWidth = sizeof( uint64 );

    while( nSize > nWidth )
    {
        *a = c;
        a++;
        nSize -= sizeof( uint64 );
    }
    
    // Finally get the rest of the buffer
    while( nSize > 0 )
    {
        *a = c;
        a++;
        --nSize;
    }
#endif // #ifndef RIOT_USE_INTRINSICS

    return pDest;
}
