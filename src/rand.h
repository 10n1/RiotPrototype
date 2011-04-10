/*********************************************************\
File:           rand.h
Purpose:        Implementation of Mersenne twister
TODO:           Further optimize, SIMD
Author:         Kyle Weicht
Created:        7/17/2010
Modified:       4/10/2011 12:34:32 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RAND_H_
#define _RAND_H_
#include "types.h"
#include "config.h"
#include "atomic.h"

#if 1

namespace Riot
{

//-----------------------------------------------------------------------------
//  Random Number Generator -- Better than C rand()
//-----------------------------------------------------------------------------
#define StateSize 624   // Array size
#define MiddleWord 397  // Middle word

class CRandom
{
private: 
    uint m_nMT[StateSize];
    atomic_t m_nIndex;
public:
    CRandom();
    
    CRandom( uint nSeed );

    //-----------------------------------------------------------------------------
    //  Reset
    //  Resets the random number generator to a "random" state
    //-----------------------------------------------------------------------------
    void Reset();
    
    //-----------------------------------------------------------------------------
    //  Rand
    //  Returns a random uint
    //-----------------------------------------------------------------------------
    uint Rand();
};

//#undef StateSiz
//#undef MiddleWord

//  Create a static number generator
static CRandom gs_RandomNumberGenerator;

// Functions for accessing the random number generator
#define UINT_RECIP 1.0/UINT_MAX
#define UINT_RECIPF 1.0f/UINT_MAX

inline float64 RandDouble() { return gs_RandomNumberGenerator.Rand() * UINT_RECIP; }
inline float64 RandDouble( float64 fRange ) { return RandDouble() * fRange; }
inline float32 RandFloat() { return gs_RandomNumberGenerator.Rand() * UINT_RECIPF; }
inline float32 RandFloat( float32 fRange ) { return RandFloat() * fRange; }
inline sint RandSint() { return (sint)gs_RandomNumberGenerator.Rand(); }
inline uint RandUint() { return gs_RandomNumberGenerator.Rand(); }
inline uint16 RandShort() { return ( gs_RandomNumberGenerator.Rand() >> 8 ) & 0xFFFF; } // Grab the middle 16 bits, to ensure a well mixed group
inline uint Rand() { return gs_RandomNumberGenerator.Rand(); }
//-----------------------------------------------------------------------------

}

#endif

#endif // #ifndef _RAND_H_