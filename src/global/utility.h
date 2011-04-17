/*********************************************************\
File:           utility.h
Purpose:        Various stdlib functions
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 12:35:58 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "types.h"
#include "config.h"

//-----------------------------------------------------------------------------
//  C standard library replacement functions
void* Memcpy( void* pDest, const void* pSource, uint nSize );
void* Memset( void* pDest, uint c, uint nSize );

#ifndef ZeroMemory
#define ZeroMemory( pDest, nSize ) Memset( pDest, 0, nSize )
#endif

#endif // #ifndef _UTILITY_H_
