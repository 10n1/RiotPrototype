/*********************************************************\
File:           IRefCounted.h
Purpose:        A reference counting interface
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/12/2011 8:58:20 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _IREFCOUNTED_H_
#define _IREFCOUNTED_H_
#include "types.h"
#include "config.h"
#include "atomic.h"

namespace Riot
{

    class IRefCounted
    {
    public:
        // IRefCounted constructor
        IRefCounted()
            : m_nRefCount( 1 )
        { }

        // IRefCounted destructor
        virtual ~IRefCounted() { }
        /***************************************\
        | class methods                         |
        \***************************************/

        inline void AddRef( void ) { AtomicIncrement( &m_nRefCount ); }
        inline void Release( void ) 
        { 
            AtomicDecrement( &m_nRefCount ); 
            if( m_nRefCount == 0 ) 
            { 
                delete this; 
            } 
        }

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        atomic_t    m_nRefCount;
    };

} // namespace Riot

#endif // #ifndef _IREFCOUNTED_H_
