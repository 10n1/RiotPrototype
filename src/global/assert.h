/*********************************************************\
File:           assert.h
Purpose:        Defines asserts
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/24/2011 1:02:12 AM
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

#if RELEASE_ASSERTS
#define ASSERT( expression )    \
    if( expression ) { }        \
    else                        \
    {                           \
        debugBreak();           \
    }
#else
#define ASSERT( expression )
#endif // RELEASE_ASSERTS

#endif // #ifdef DEBUG

#ifndef assert
#define assert ASSERT
#endif

#endif // #ifndef _ASSERT_H_
