/*********************************************************\
File:           assert.h
Purpose:        Defines asserts
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/9/2011 11:49:40 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ASSERT_H_
#define _ASSERT_H_
#include "config.h"

#ifdef OS_WINDOWS
#define debugBreak() __debugbreak()
#elif defined( OS_OSX )
#define debugBreak() __asm__( "int $3\n" : : )
#endif

#ifdef DEBUG

#define ASSERT( expression )    \
    if( expression ) { }        \
    else                        \
    {                           \
        debugBreak();           \
    }

#else

#define ASSERT( expression )    \
    if( expression ) { }        \
    else                        \
    {                           \
        debugBreak();           \
    }


#endif

#ifndef assert
#define assert ASSERT
#endif

#endif // #ifndef _ASSERT_H_
