/*********************************************************\
File:           rand.h
Purpose:        Implementation of Mersenne twister
TODO:           Further optimize, SIMD
Author:         Kyle Weicht
Created:        7/17/2010
Modified:       4/9/2011 10:16:01 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RAND_H_
#define _RAND_H_
#include "types.h"
#include "config.h"

#if 0

namespace Riot
{

//-----------------------------------------------------------------------------
//  Random Number Generator -- Better than C rand()
//-----------------------------------------------------------------------------
#define StateSize 624   // Array size
#define MiddleWord 397  // Middle word
#include <time.h>       // TODO: Figure out better way to do this?

class CRandom
{
private: 
    uint m_nMT[StateSize];
    volatile sint m_nIndex;
public:
    inline CRandom() : m_nIndex( StateSize )
    {
        m_nMT[0] = (uint)time(0); // Seed based on time
        for( uint i = 1; i < StateSize; ++i )
        {
            m_nMT[i] = ( 0x6c078965 * ( m_nMT[i-1] ^ ( m_nMT[i-1] >> 30 ) ) + 1 );
        }
        Reset();
    }

    inline CRandom( uint nSeed ) : m_nIndex( StateSize )
    {
        m_nMT[0] = nSeed;
        for( uint i = 1; i < StateSize; ++i )
        {
            m_nMT[i] = ( 0x6c078965 * ( m_nMT[i-1] ^ ( m_nMT[i-1] >> 30 ) ) + 1 );
        }
        Reset();
    }

    inline void Reset()
    {
        for( int i = 0; i < 624; ++i )
        {
            int n = ( m_nMT[i] & 0x80000000 ) | ( m_nMT[ (i + 1) % 624 ] & 0x7FFFFFFF );
            m_nMT[i] = m_nMT[ (i + 397) % 624 ] ^ ( n >> 1 );
            m_nMT[i] ^= ( ( n & 0x1 ) ? 0x9908b0df : 0x0 );
        }
        m_nIndex = StateSize;
    }

    inline uint Rand()
    {
        if( !(AtomicDecrement(&m_nIndex)) )
        {
            Reset();
        }

        uint n = m_nMT[m_nIndex];
        n ^= n >> 11;
        n ^= ( n << 7 ) & 0x9d2c5680;
        n ^= ( n << 15 ) & 0xefc60000;
        n ^= n >> 18;

        return n;
    }
};

#undef StateSize
#undef MiddleWord

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

#define rand() Rand()   // Redefine rand(), range 0-UINT_MAX
//-----------------------------------------------------------------------------

}

#endif

#endif // #ifndef _RAND_H_