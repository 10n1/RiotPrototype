/*********************************************************\
File:           memory.h
Purpose:        Overloading new/delete. They now allocate
out of a pre-allocated buffer.
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/17/2011 12:35:58 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "types.h"
#include "config.h"

//-----------------------------------------------------------------------------
//  Overloaded new/delete, to use the custom allocation pool
#if RIOT_USE_CUSTOM_ALLOCATOR

#ifdef DEBUG
void* __cdecl operator new(size_t nSize, const char* szFile, uint nLine);
void* __cdecl operator new[](size_t nSize, const char* szFile, uint nLine);
#define DEBUG_NEW new( __FILE__, __LINE__ )
#else
void* __cdecl operator new(size_t nSize);
void* __cdecl operator new[](size_t nSize);
#define DEBUG_NEW new
#endif // #ifdef DEBUG

void __cdecl operator delete(void* pVoid);
void __cdecl operator delete[](void* pVoid);
#endif // #ifdef RIOT_USE_CUSTOM_ALLOCATOR

namespace Riot
{

    namespace Memory
    {
        void ReleasePool( void );
    } // namespace Memory

} // namespace Riot
//-----------------------------------------------------------------------------



#endif // #ifndef _MEMORY_H_
