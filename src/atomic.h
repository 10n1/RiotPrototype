/*********************************************************\
File:           atomic.h
Purpose:        Platform independent atomic operations
                (InterlockedIncrement, etc)
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/9/2011 6:10:27 PM
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
sint64 AtomicIncrement64( volatile sint64* pValue );
sint64 AtomicDecrement64( volatile sint64* pValue );

//-----------------------------------------------------------------------------
sint AtomicAdd( volatile sint* pValue, sint nValue );
sint64 AtomicAdd64( volatile sint64* pValue, sint64 nValue );

//-----------------------------------------------------------------------------
sint AtomicOr( volatile sint* pValue, sint nMask );
sint AtomicAnd( volatile sint* pValue, sint nMask );

//-----------------------------------------------------------------------------
sint AtomicExchange( volatile sint* pValue, sint nNewValue );
sint AtomicCompareAndSwap( volatile sint* pValue, sint nNewValue, sint nComparison );

} // namespace Riot

#endif // #ifndef _ATOMIC_H_
