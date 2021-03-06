/*********************************************************\
File:           common.h
Purpose:        Global include file
Author:         Kyle Weicht
Created:        4/7/2011
Modified:       4/27/2011 10:25:45 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

// The most basic definition files
#include "types.h"
#include "config.h"

// Standard headers
#ifdef OS_WINDOWS
#include <crtdbg.h> // Include this to avoid errors when overloading new/delete
#pragma warning(disable:4100)   // Disable the warning about unused function parameters
#endif

#include <fstream>
#include <string>

// Very basic, low level functionality
#include "atomic.h"
#include "memory.h"

#if RIOT_USE_CUSTOM_ALLOCATOR
#define new DEBUG_NEW
#endif // #if RIOT_USE_CUSTOM_ALLOCATOR

#include "assert.h"
#include "vectormath.h"
#include "rand.h"
#include "utility.h"

#include "Settings.h"

#endif // #ifndef _COMMON_H_
