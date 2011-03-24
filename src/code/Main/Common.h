/*********************************************************\
File:           Common.h
Purpose:        
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 7:07:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

// Global defines
#include "Types.h"

// Standard headers
#ifdef OS_WINDOWS
#include <crtdbg.h> // Include this to avoid errors when overloading new/delete
#endif

// Memory
#include "memory.h"

// Use overridden new/delete
#define new DEBUG_NEW

#endif // #ifndef _COMMON_H_
