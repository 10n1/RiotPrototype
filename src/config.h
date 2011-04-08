/*********************************************************\
File:           config.h
Purpose:        Includes platform and build configurations
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/7/2011 10:39:58 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

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

//-----------------------------------------------------------------------------
//  Build configuration
//-----------------------------------------------------------------------------
#define RIOT_USE_INTRINSICS

#endif // #ifndef _CONFIG_H_