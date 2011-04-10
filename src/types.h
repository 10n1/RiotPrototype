/*********************************************************\
File:           types.h
Purpose:        Defining basic types
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/9/2011 6:29:34 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TYPES_H_
#define _TYPES_H_

//-----------------------------------------------------------------------------
//    Types
//-----------------------------------------------------------------------------
#include <limits.h>
#if defined( WIN32 ) || defined( WIN64 ) // TODO: Add a RIOT_WINDOWS define?
typedef unsigned __int8  byte;
typedef unsigned __int8  uint8;
typedef signed   __int8  sint8;
typedef signed   __int8  int8;
typedef unsigned __int16 uint16;
typedef signed   __int16 sint16;
typedef signed   __int16 int16;
typedef unsigned __int32 uint32;
typedef signed   __int32 sint32;
typedef signed   __int32 int32;
typedef unsigned __int64 uint64;
typedef signed   __int64 sint64;
typedef signed   __int64 int64;
#else // not defined( WIN32 ) || defined( WIN64 )
#include <stdint.h>
#include <stddef.h>
typedef int8_t    bool8;
typedef uint8_t   byte;
typedef uint8_t   uint8;
typedef int8_t    sint8;
typedef int8_t    int8;
typedef uint16_t  uint16;
typedef int16_t   sint16;
typedef int16_t   int16;
typedef uint32_t  bool32;
typedef uint32_t  uint32;
typedef int32_t   sint32;
typedef int32_t   int32;
typedef uint64_t  uint64;
typedef int64_t   sint64;
typedef uint64_t  int64;
#endif // defined( WIN32 ) || defined( WIN64 )

typedef uint32  uint;
typedef sint32  sint;
typedef float   float32;
typedef double  float64;

typedef void* handle;
typedef void* pvoid;

static const uint ptr_size = sizeof( pvoid );

#if defined( _M_X64 ) || defined( __LP64__ )
typedef uint64 nativeuint;
#else
typedef uint32 nativeuint;
#endif // #if defined( _M_X64)
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//    Macro defines
//-----------------------------------------------------------------------------
#ifndef NULL
#define NULL 0
#endif // #ifndef NULL

#ifndef TRUE
#define TRUE ( 1 == 1 )
#endif // #ifndef TRUE

#ifndef FALSE
#define FALSE ( 0 == 1 )
#endif // #ifndef FALSE
//-----------------------------------------------------------------------------
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p) { p->Release(); p = NULL; } 
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) if(p) { delete [] p; p = NULL; }
#endif

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH( a ) (sizeof(a) / sizeof(a[0]) )
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
enum Result
{
    kResultSuccess,
    kResultFailure,    
};
//-----------------------------------------------------------------------------

#endif // #ifndef _TYPES_H_
