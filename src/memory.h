/*********************************************************\
File:           memory.h
Purpose:        Overloading new/delete. They now allocate
                out of a pre-allocated buffer.
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 11:12:44 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "types.h"
#include "config.h"

void* __cdecl operator new(size_t nSize);
void* __cdecl operator new[](size_t nSize);
void __cdecl operator delete(void* pVoid);
void __cdecl operator delete[](void* pVoid);

void ReleasePool( void );

void* memcpy( void* pDest, const void* pSource, uint nSize );

#endif // #ifndef _MEMORY_H_
