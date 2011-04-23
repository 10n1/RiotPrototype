/*********************************************************\
File:           atomic.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/23/2011 2:15:09 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "atomic.h"

#ifdef OS_WINDOWS
#include <intrin.h>
#elif defined( OS_OSX )
#include <libkern/OSAtomic.h>
#else
#endif

namespace Riot
{
#ifndef USE_MACROS
    //-----------------------------------------------------------------------------
    sint AtomicIncrement( volatile sint* pValue )
    {
#ifdef OS_WINDOWS
        return _InterlockedIncrement( (volatile long*)pValue );
#else
        return OSAtomicIncrement32( pValue );
#endif // #ifdef OS_WINDOWS
    }
    sint AtomicDecrement( volatile sint* pValue )
    {
#ifdef OS_WINDOWS
        return _InterlockedDecrement( (volatile long*)pValue );
#else
        return OSAtomicDecrement32( pValue );
#endif // #ifdef OS_WINDOWS
    }

    sint AtomicAdd( volatile sint* pValue, sint nValue )
    {
#ifdef OS_WINDOWS
        return _InterlockedExchangeAdd( (volatile long*)pValue, nValue ) + nValue;
#else
        return OSAtomicAdd32( nValue, pValue );
#endif // #ifdef OS_WINDOWS
    }

#ifdef _64BIT
    //-----------------------------------------------------------------------------
    sint64 AtomicIncrement64( volatile sint64* pValue )
    {
#ifdef OS_WINDOWS
        return _InterlockedIncrement64( (volatile __int64*)pValue );
#else
        return OSAtomicIncrement64( pValue );
#endif // #ifdef OS_WINDOWS
    }
    sint64 AtomicDecrement64( volatile sint64* pValue )
    {
#ifdef OS_WINDOWS
        return _InterlockedDecrement64( (volatile __int64*)pValue );
#else
        return OSAtomicDecrement64( pValue );
#endif // #ifdef OS_WINDOWS
    }
    sint64 AtomicAdd64( volatile sint64* pValue, sint64 nValue )
    {
#ifdef OS_WINDOWS
        return _InterlockedExchangeAdd64( (volatile __int64*)pValue, nValue ) + nValue;
#else
        return OSAtomicAdd64( nValue, pValue );
#endif // #ifdef OS_WINDOWS
    }

#endif // #ifdef _64BIT

    //-----------------------------------------------------------------------------
    sint AtomicOr( volatile uint* pValue, uint nMask )
    {
#ifdef OS_WINDOWS
        return _InterlockedOr( (volatile long*)pValue, nMask );
#else
        return OSAtomicOr32( nMask, pValue );
#endif // #ifdef OS_WINDOWS
    }
    sint AtomicAnd( volatile uint* pValue, uint nMask )
    {
#ifdef OS_WINDOWS
        return _InterlockedAnd( (volatile long*)pValue, nMask );
#else
        return OSAtomicAnd32( nMask, pValue );
#endif // #ifdef OS_WINDOWS
    }


    //-----------------------------------------------------------------------------
    sint AtomicExchange( volatile sint* pValue, sint nNewValue )
    {    
#ifdef OS_WINDOWS
        return _InterlockedExchange( (volatile long*)pValue, nNewValue );
#else
        sint nOrigValue = *pValue;
        OSAtomicCompareAndSwap32( nOrigValue, nNewValue, pValue );
        return nOrigValue;
#endif // #ifdef OS_WINDOWS
    }

    sint AtomicCompareAndSwap( volatile sint* pValue, sint nNewValue, sint nComparison )
    {
#ifdef OS_WINDOWS
        return _InterlockedCompareExchange( (volatile long*)pValue, nNewValue, nComparison );
#else
        sint nOrigValue = *pValue;
        OSAtomicCompareAndSwap32( nComparison, nNewValue, pValue );
        return nOrigValue;
#endif // #ifdef OS_WINDOWS
    }
#endif
} // namespace Riot
