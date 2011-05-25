/*********************************************************\
File:           utility.h
Purpose:        Various stdlib functions
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/10/2011 10:39:55 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "types.h"
#include "config.h"

#define __AVX__ 1
#include <immintrin.h>

//-----------------------------------------------------------------------------
//  C standard library replacement functions
inline void* Memcpy( void* pDest, const void* pSource, uint nSize );
inline void* MemcpySSE( void* pDest, const void* pSource, uint nSize );
inline void* MemcpyAVX( void* pDest, const void* pSource, uint nSize );

void* Memset( void* pDest, uint c, uint nSize );
void* MemsetSSE( void* pDest, uint c, uint nSize );
void* MemsetAVX( void* pDest, uint c, uint nSize );


//-----------------------------------------------------------------------------
void* Memcpy( void* pDest, const void* pSource, uint nSize )
{
    // First try AVX
    if( gs_nAVXSupported )
    {
        return MemcpyAVX( pDest, pSource, nSize );
    }

    // Then try SSE
    if( gs_nSSE2Supported )
    {
        return MemcpySSE( pDest, pSource, nSize );
    }

    // Otherwise do it serially
#ifdef _64BIT
    // 64-bit goes 64 bits at a time
    byte* a = (byte*)pDest;
    const byte* b = (const byte*)pSource;

    // If we're not 8-byte aligned, make it so
    // Go one byte at a time to get to an 8 byte boundry
    while( reinterpret_cast<nativeuint>(a) & 0x7 )
    {
        *a = *b;
        a++, b++;
        --nSize;
    }

    // Then 64 bits at a time
    uint64* a64 = (uint64*)a;
    const uint64* b64 = (const uint64*)b;

    const uint nWidth = sizeof( uint64 );
    while( nSize >= nWidth )
    {
        *a64 = *b64;
        a++, b++;
        nSize -= nWidth;
    }

    // Now finish if we can't go 8 bytes    
    a = (byte*)a64;
    b = (const byte*)b64;

    while( nSize > 0 )
    {
        *a = *b;
        a++, b++;
        nSize--;
    }
#else
    // 32-bit goes 32 bits at a time
  
    byte* a = (byte*)pDest;
    const byte* b = (const byte*)pSource;

    // If we're not 4-byte aligned, make it so
    // Go one byte at a time to get to an 4 byte boundry
    while( reinterpret_cast<nativeuint>(a) & 0x3 )
    {
        *a = *b;
        a++, b++;
        --nSize;
    }

    // Then 32 bits at a time
    uint32* a32 = (uint32*)a;
    const uint32* b32 = (const uint32*)b;

    const uint nWidth = sizeof( uint32 );
    while( nSize >= nWidth )
    {
        *a32 = *b32;
        a++, b++;
        nSize -= nWidth;
    }

    // Now finish if we can't go 8 bytes    
    a = (byte*)a32;
    b = (const byte*)b32;

    while( nSize > 0 )
    {
        *a = *b;
        a++, b++;
        nSize--;
    }
#endif

    return pDest;
}

void* MemcpySSE( void* pDest, const void* pSource, uint nSize )
{
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
        uint32* a32 = (uint32*)a;
        const uint32* b32 = (const uint32*)b;
        const uint nWidth = sizeof( uint32 );
        while( reinterpret_cast<nativeuint>(a32) & 0xF )
        {
            *a32 = *b32;
            a++, b++;
            nSize -= nWidth;
        }

        a = (byte*)a32;
        b = (const byte*)b32;
    }

    // Then go 16 bytes at a time
    const uint nWidth = sizeof( __m128i );

    __m128i         v0;
    __m128i*        a128 = (__m128i*)a;
    const __m128i*  b128 = (const __m128i*)b;

    if(  reinterpret_cast<nativeuint>(b) & 0xF )
    {   
        // if pSource is not 16 byte aligned, we use the unaligned load
        while( nSize >= nWidth )
        {
            v0 =  _mm_loadu_si128( b128 );
            _mm_store_si128( a128, v0 );

            a128++;
            b128++;
            nSize -= nWidth;
        }
    }
    else
    {
        while( nSize >= nWidth )
        {
            v0 =  _mm_load_si128( b128 );
            _mm_store_si128( a128, v0 );

            a128++;
            b128++;
            nSize -= nWidth;
        }
    }

    a = (byte*)a128;
    b = (const byte*)b128;

    // Finally get the rest of the buffer
    while( nSize > 0 )
    {
        *a = *b;
        a++, b++;
        --nSize;
    }

    return pDest;
}

void* MemcpyAVX( void* pDest, const void* pSource, uint nSize )
{
    byte* a = (byte*)pDest;
    const byte* b = (const byte*)pSource;

    // If we're not 32-byte aligned, make it so
    if(  reinterpret_cast<nativeuint>(a) & 0x1F )
    {
        // Go one byte at a time to get to an 8 byte boundry
        while( reinterpret_cast<nativeuint>(a) & 0x7 )
        {
            *a = *b;
            a++, b++;
            --nSize;
        }

        // Then go 8 bytes at at time
        uint64* a64 = (uint64*)a;
        const uint64* b64 = (const uint64*)b;
        const uint nWidth = sizeof( uint64 );
        while( reinterpret_cast<nativeuint>(a64) & 0x1F )
        {
            *a64 = *b64;
            a++, b++;
            nSize -= nWidth;
        }

        a = (byte*)a64;
        b = (const byte*)b64;
    }

    // Then go 32 bytes at a time
    const uint nWidth = sizeof( __m256i );

    __m256i         v0;
    __m256i*        a256 = (__m256i*)a;
    const __m256i*  b256 = (const __m256i*)b;

    if(  reinterpret_cast<nativeuint>(b) & 0x1F )
    {   
        // if pSource is not 32 byte aligned, we use the unaligned load
        while( nSize >= nWidth )
        {
            v0 =  _mm256_load_si256( b256 );
            _mm256_store_si256( a256, v0 );

            a256++;
            b256++;
            nSize -= nWidth;
        }
    }
    else
    {
        while( nSize >= nWidth )
        {
            v0 =  _mm256_load_si256( b256 );
            _mm256_store_si256( a256, v0 );

            a256++;
            b256++;
            nSize -= nWidth;
        }
    }

    a = (byte*)a256;
    b = (const byte*)b256;

    // Finally get the rest of the buffer
    while( nSize > 0 )
    {
        *a = *b;
        a++, b++;
        --nSize;
    }

    return pDest;
}

//-----------------------------------------------------------------------------

#endif // #ifndef _UTILITY_H_
