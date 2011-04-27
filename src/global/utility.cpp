/*********************************************************\
File:           utility.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/27/2011 10:57:43 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "utility.h"

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
