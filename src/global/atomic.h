/*********************************************************\
File:           atomic.h
Purpose:        Platform independent atomic operations
(InterlockedIncrement, etc)
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/24/2011 12:58:03 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ATOMIC_H_
#define _ATOMIC_H_
#include "types.h"
#include "config.h"

//#define USE_MACROS

namespace Riot
{

    //-----------------------------------------------------------------------------
#ifndef USE_MACROS
    sint AtomicIncrement( volatile sint* pValue );
    sint AtomicDecrement( volatile sint* pValue );
    sint AtomicAdd( volatile sint* pValue, sint nValue );
    //-----------------------------------------------------------------------------

#ifdef _64BIT
    sint64 AtomicIncrement64( volatile sint64* pValue );
    sint64 AtomicDecrement64( volatile sint64* pValue );
    sint64 AtomicAdd64( volatile sint64* pValue, sint64 nValue );
#endif // #ifdef _64BIT

    //-----------------------------------------------------------------------------
    sint AtomicOr( volatile uint* pValue, uint nMask );
    sint AtomicAnd( volatile uint* pValue, uint nMask );

    //-----------------------------------------------------------------------------
    sint AtomicExchange( volatile sint* pValue, sint nNewValue );
    sint AtomicCompareAndSwap( volatile sint* pValue, sint nNewValue, sint nComparison );

#else    
    #define AtomicIncrement( pValue )   _InterlockedIncrement( (volatile long*)pValue )
    #define AtomicDecrement( pValue )   _InterlockedDecrement( (volatile long*)pValue )
    #define AtomicAdd( pValue, nValue ) _InterlockedExchangeAdd( (volatile long*)pValue, nValue ) + nValue
    //-----------------------------------------------------------------------------

#ifdef _64BIT
    #define AtomicIncrement64( pValue ) _InterlockedIncrement64( (volatile __int64*)pValue )
    #define AtomicDecrement64( pValue ) _InterlockedDecrement64( (volatile __int64*)pValue )
    #define AtomicAdd64( pValue, nValue ) _InterlockedExchangeAdd64( (volatile __int64*)pValue, nValue ) + nValue
#endif // #ifdef _64BIT

    //-----------------------------------------------------------------------------
    #define AtomicOr( pValue, nMask )_InterlockedOr( (volatile long*)pValue, nMask )
    #define AtomicAnd( pValue, nMask )_InterlockedAnd( (volatile long*)pValue, nMask )

    //-----------------------------------------------------------------------------
#define AtomicExchange( pValue, nNewValue ) _InterlockedExchange( (volatile long*)pValue, nNewValue )
#define AtomicCompareAndSwap( pValue, nNewValue, nComparison ) _InterlockedCompareExchange( (volatile long*)pValue, nNewValue, nComparison )
#endif

    typedef volatile sint   atomic_t;

} // namespace Riot

#endif // #ifndef _ATOMIC_H_
