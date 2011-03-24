/*********************************************************\
File:       IRefCounted.h
Purpose:    Ref counting interface
\*********************************************************/
#ifndef _IREFCOUNTED_H_
#define _IREFCOUNTED_H_
#include "Common.h"
#include "Types.h"

class IRefCounted
{
public:
    IRefCounted()
        : m_nRefCount(1)
    { }
    virtual ~IRefCounted() { }

    _inline void AddRef( void ) { ++m_nRefCount; }
    _inline void Release( void ) 
    { 
        --m_nRefCount; 
        if( m_nRefCount == 0 ) 
        { 
            delete this; 
        } 
    }

private:
    uint m_nRefCount;
};

#endif // #ifndef _IREFCOUNTED_H_
