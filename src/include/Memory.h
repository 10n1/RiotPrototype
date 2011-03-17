/*********************************************************\
File:       Memory.h
Purpose:    Memory allocation/tracking
TODO:       Figure out how to do this without size_t
\*********************************************************/
#ifndef _MEMORY_H_
#define _MEMORY_H_
#include "Types.h"

#ifdef DEBUG

void __cdecl DumpMemoryLeaks(void);

void* __cdecl operator new( size_t nSize, const char* szFile, unsigned int nLine );
void* __cdecl operator new[]( size_t nSize, const char* szFile, unsigned int nLine );

#define DEBUG_NEW new( __FILE__, __LINE__ )

#else

#define DumpMemoryLeaks()

void* __cdecl operator new(size_t nSize);
void* __cdecl operator new[](size_t nSize);

#define DEBUG_NEW new

#endif

void __cdecl operator delete(void* pVoid) throw();
void __cdecl operator delete[](void* pVoid) throw();


#endif // #ifndef _MEMORY_H_