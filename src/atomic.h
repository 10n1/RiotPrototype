/*********************************************************\
File:           atomic.h
Purpose:        Platform independent atomic operations
                (InterlockedIncrement, etc)
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 12:45:41 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ATOMIC_H_
#define _ATOMIC_H_
#include "types.h"
#include "config.h"

//-----------------------------------------------------------------------------
uint AtomicIncrement( volatile uint* pValue );
uint AtomicDecrement( volatile uint* pValue );
uint64 AtomicIncrement64( volatile uint64* pValue );
uint64 AtomicDecrement64( volatile uint64* pValue );

//-----------------------------------------------------------------------------
uint AtomicAdd( volatile uint* pValue, uint nValue );
uint64 AtomicAdd64( volatile uint64* pValue, uint nValue );

//-----------------------------------------------------------------------------
uint AtomicOr( volatile uint* pValue, uint nMask );
uint AtomicAnd( volatile uint* pValue, uint nMask );

#endif // #ifndef _ATOMIC_H_
