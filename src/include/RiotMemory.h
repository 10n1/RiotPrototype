/*********************************************************\
File:       RiotMemory.h
Purpose:    Memory allocation/tracking
TODO:       Figure out how to do this without size_t
\*********************************************************/
#ifndef _RIOTMEMORY_H_
#define _RIOTMEMORY_H_
#include "RiotTypes.h"
#include <memory> // TODO: Is including <memory> too heavyweight?

//-----------------------------------------------------------------------------
//    Memory allocation tracking, currently using aligned memory
//-----------------------------------------------------------------------------
#undef new
#undef delete
#define CACHEALIGNMENT 16

#if defined( _DEBUG )

void AddAllocation(void* pData, uint nSize, const char* szFile, uint nLine);
void RemoveAllocation(void* pData);
void DumpMemoryLeaks(void);

inline void* _cdecl operator new(size_t nSize, const char* szFile, uint nLine)
{
    void* p = _aligned_malloc( nSize, CACHEALIGNMENT );
    // TODO: Handle out of memory error ( p == 0 )
    AddAllocation( p, (uint)nSize, szFile, nLine );
    return p;
};
inline void* _cdecl operator new[](size_t nSize, const char* szFile, uint nLine)
{
    void* p = _aligned_malloc( nSize, CACHEALIGNMENT );
    // TODO: Handle out of memory error ( p == 0 )
    AddAllocation( p, (uint)nSize, szFile, nLine );
    return p;
};
inline void _cdecl operator delete(void* pVoid)
{
    RemoveAllocation(pVoid);
    _aligned_free(pVoid);
};
inline void _cdecl operator delete[](void* pVoid)
{
    RemoveAllocation( pVoid );
    _aligned_free( pVoid );
};
#define DEBUG_NEW new(__FILE__, __LINE__)

#else // #if notdefined( _DEBUG )

#define DumpMemoryLeaks(x)

inline void* __cdecl operator new(size_t nSize)
{
    return _aligned_malloc( nSize, CACHEALIGNMENT );
};

inline void* __cdecl operator new[](size_t nSize)
{
    return _aligned_malloc( nSize, CACHEALIGNMENT );
};

inline void __cdecl operator delete(void* pVoid)
{
    _aligned_free( pVoid );
};

inline void __cdecl operator delete[](void* pVoid)
{
    _aligned_free( pVoid );
};

#define DEBUG_NEW new
#endif // #if defined( _DEBUG )

#define new DEBUG_NEW
// Disable the warning for a new with no matching delete
// The compiler is wrong, it can't find the overidden deletes
#pragma warning(disable:4291)
#undef CACHEALIGNMENT
//-----------------------------------------------------------------------------

#endif // #ifndef _RIOTMEMORY_H_
