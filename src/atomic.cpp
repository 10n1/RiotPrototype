/*********************************************************\
File:           atomic.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 12:45:40 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "atomic.h"

#ifdef OS_WINDOWS
#include <intrin.h>
#elif defined( OS_OSX )
#include <libkern/OSAtomic.h>
#else
#endif

//-----------------------------------------------------------------------------
uint AtomicIncrement( volatile uint* pValue )
{
#ifdef OS_WINDOWS
    return _InterlockedIncrement( (volatile long*)pValue );
#else
    return OSAtomicIncrement32( pValue );
#endif // #ifdef OS_WINDOWS
}
uint AtomicDecrement( volatile uint* pValue )
{
#ifdef OS_WINDOWS
    return _InterlockedDecrement( (volatile long*)pValue );
#else
    return OSAtomicDecrement32( pValue );
#endif // #ifdef OS_WINDOWS
}
uint64 AtomicIncrement64( volatile uint64* pValue )
{
#ifdef OS_WINDOWS
    return _InterlockedIncrement64( (volatile __int64*)pValue );
#else
    return OSAtomicIncrement64( pValue );
#endif // #ifdef OS_WINDOWS
}
uint64 AtomicDecrement64( volatile uint64* pValue )
{
#ifdef OS_WINDOWS
    return _InterlockedDecrement64( (volatile __int64*)pValue );
#else
    return OSAtomicDecrement64( pValue );
#endif // #ifdef OS_WINDOWS
}

//-----------------------------------------------------------------------------
uint AtomicAdd( volatile uint* pValue, uint nValue )
{
#ifdef OS_WINDOWS
    return _InterlockedExchangeAdd( (volatile long*)pValue, nValue );
#else
    return OSAtomicAdd32( nValue, pValue );
#endif // #ifdef OS_WINDOWS
}
uint64 AtomicAdd64( volatile uint64* pValue, uint nValue )
{
#ifdef OS_WINDOWS
    return _InterlockedExchangeAdd64( (volatile __int64*)pValue, nValue );
#else
    return OSAtomicAdd64( nValue, pValue );
#endif // #ifdef OS_WINDOWS
}


//-----------------------------------------------------------------------------
uint AtomicOr( volatile uint* pValue, uint nMask )
{
#ifdef OS_WINDOWS
    return _InterlockedOr( (volatile long*)pValue, nMask );
#else
    return OSAtomicOr32( nMask, pValue );
#endif // #ifdef OS_WINDOWS
}
uint AtomicAnd( volatile uint* pValue, uint nMask )
{
#ifdef OS_WINDOWS
    return _InterlockedAnd( (volatile long*)pValue, nMask );
#else
    return OSAtomicAnd32( nMask, pValue );
#endif // #ifdef OS_WINDOWS
}