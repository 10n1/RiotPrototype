/*********************************************************\
File:           assert.h
Purpose:        Defines asserts
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 11:38:17 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ASSERT_H_
#define _ASSERT_H_
#include "common.h"

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

#endif

#endif // #ifndef _ASSERT_H_
