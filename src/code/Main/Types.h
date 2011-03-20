/*********************************************************\
File:      Types.h
Purpose:   Defining basic types
\*********************************************************/
#ifndef _TYPES_H_
#define _TYPES_H_

//-----------------------------------------------------------------------------
//    Types
//-----------------------------------------------------------------------------
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

typedef uint32 uint;
typedef sint32 sint;
typedef float float32;
typedef double float64;

typedef void* handle;
typedef void* pvoid;

static const uint ptr_size = sizeof( pvoid );

#if defined( _M_X64) || defined( __LP64__ )
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
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Platform defines
//-----------------------------------------------------------------------------
#if defined( DEBUG ) || defined( _DEBUG )
#ifndef DEBUG
#define DEBUG
#endif
#else
#ifndef RELEASE
#define RELEASE
#endif
#endif

//-----------------------------------------------------------------------------
//  Windows
#if defined( WIN32 ) || defined( WIN64 )

#define OS_WINDOWS

#define WIN32_LEAN_AND_MEAN // Make sure <Windows.h> is lightweight when its included

#ifdef _M_X64
#define _64BIT
#else
#define _32BIT
#endif // #ifdef _M_X64

#endif // #if defined( WIN32 ) || defined( WIN64 )
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Compiling on OS X
#if defined( __APPLE__ )

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE
#define OS_IOS
#else
#define OS_OSX
#endif

#if TARGET_RT_64_BIT
#define _64BIT
#else
#define _32BIT
#endif // #ifdef TARGET_RT_64_BIT

#define DECLAREPOOL NSAutoreleasePool* pAPool = [[NSAutoreleasePool alloc] init];

#define RELEASEPOOL [pAPool release];


#endif // #if defined( _APPLE_ )
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Linux
#if defined( __LINUX__ )

// TODO: Support Android
#define OS_LINUX

#ifdef __LP64__
#define _64BIT
#else
#define _32BIT
#endif // #ifdef __LP64__

#endif // #if defined( __LINUX__ )
//-----------------------------------------------------------------------------

#endif // #ifndef _TYPES_H_
