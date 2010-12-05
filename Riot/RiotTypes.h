/*********************************************************\
File:           RiotTypes.h
Purpose:        Defining basic types
\*********************************************************/
#ifndef _RIOTTYPES_H_
#define _RIOTTYPES_H_

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
typedef unsigned char      byte;
typedef unsigned char      uint8;
typedef signed   char      sint8;
typedef signed   char      int8;
typedef unsigned short     uint16;
typedef signed   short     sint16;
typedef signed   short     int16;
typedef unsigned int       uint32;
typedef signed   int       sint32;
typedef signed   int       int32;
typedef unsigned long long uint64;
typedef signed   long long sint64;
typedef signed   long long int64;
#endif // defined( WIN32 ) || defined( WIN64 )

typedef uint32 uint;
typedef sint32 sint;
typedef float float32;
typedef double float64;

typedef void* handle;
typedef void* pvoid;
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

#endif // #ifndef _RIOTTYPES_H_
