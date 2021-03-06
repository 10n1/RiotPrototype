/*********************************************************\
File:           config.h
Purpose:        Includes platform and build configurations
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       5/23/2011 12:25:50 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "types.h"

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

#define ARCH_IA32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Make sure <Windows.h> is lightweight when its included
#endif // #ifndef WIN32_LEAN_AND_MEAN

#define SYSTEM_API_CONVENTION __stdcall

#ifdef _M_X64
#define _64BIT
#else
#define _32BIT
#endif // #ifdef _M_X64

#pragma warning( disable:4291 ) // no matching operator delete found
#pragma warning( disable:4996 ) // deprecated "unsafe" C stdlib functions

#endif // #if defined( WIN32 ) || defined( WIN64 )
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Compiling on OS X
#if defined( __APPLE__ )

#include <TargetConditionals.h>

#define SYSTEM_API_CONVENTION


#if TARGET_OS_IPHONE
#define OS_IOS
#define ARCH_ARM
#else
#define OS_OSX
#define ARCH_IA32
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

#define ARCH_IA32

#define SYSTEM_API_CONVENTION

#ifdef __LP64__
#define _64BIT
#else
#define _32BIT
#endif // #ifdef __LP64__

#endif // #if defined( __LINUX__ )
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  CPU Capabilities
extern const uint8  gs_nNumHardwareThreads;
extern const uint8  gs_nSSESupported;
extern const uint8  gs_nSSE2Supported;
extern const uint8  gs_nSSE3Supported;
extern const uint8  gs_nSSSE3Supported;
extern const uint8  gs_nSSE41Supported;
extern const uint8  gs_nSSE42Supported;
extern const uint8  gs_nAVXSupported;

extern const uint8  gs_nSIMDWidth;  
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Build configuration
//-----------------------------------------------------------------------------

#define USE_OPENGL                      0

//////////////////////////////////////////
// Graphics API selection
#ifndef OS_WINDOWS
// If we're not in Windows, OpenGL is the
//  only option
#undef  USE_OPENGL
#define USE_OPENGL                      1
#else
#if USE_OPENGL
#define USE_DIRECTX                     0
#else
#define USE_DIRECTX                     1
#endif
#endif
//////////////////////////////////////////

#define MULTITHREADED           1
#define MAX_THREADS             32
#define MAX_TASKS_PER_THREAD    2048
#define MAX_TASKS               1024
#define MAX_SUB_TASKS           (1024*16)
#define MAX_OBJECTS             (16*1024)
#define MAX_LIGHTS              8
#define MAX_RENDER_COMMANDS     (32*1024)
#define MAX_CHARACTERS          1
#define MAX_MESHES              256
#define MAX_TEXTURES            256

#define RELEASE_ASSERTS         0

#define PARALLEL_UPDATE         (1 && MULTITHREADED)
#define PIPELINED_RENDER        (1 && MULTITHREADED)

#define RIOT_ALIGN_MEMORY           1
#define RIOT_USE_INTRINSICS         1

#ifndef OS_OSX
#define RIOT_USE_CUSTOM_ALLOCATOR   1
#else
#define RIOT_USE_CUSTOM_ALLOCATOR   0
#endif

#endif // #ifndef _CONFIG_H_
