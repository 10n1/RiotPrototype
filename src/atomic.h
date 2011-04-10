/*********************************************************\
File:           atomic.h
Purpose:        Platform independent atomic operations
                (InterlockedIncrement, etc)
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 12:33:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ATOMIC_H_
#define _ATOMIC_H_
#include "types.h"
#include "config.h"

namespace Riot
{

//-----------------------------------------------------------------------------
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

typedef volatile sint   atomic_t;

} // namespace Riot

#endif // #ifndef _ATOMIC_H_
