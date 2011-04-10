/*********************************************************\
 File:           atomic.cpp
 Author:         Kyle Weicht
 Created:        4/8/2011
 Modified:       4/9/2011 6:10:27 PM
 Modified by:    Kyle Weicht
 \*********************************************************/
#include "Rand.h"
#include <time.h>       // TODO: Figure out better way to do this?

namespace Riot
{

CRandom::CRandom() 
    : m_nIndex( StateSize )
{
    m_nMT[0] = (uint)time(0); // Seed based on time
    for( uint i = 1; i < StateSize; ++i )
    {
        m_nMT[i] = ( 0x6c078965 * ( m_nMT[i-1] ^ ( m_nMT[i-1] >> 30 ) ) + 1 );
    }
    Reset();
}

CRandom::CRandom( uint nSeed ) 
    : m_nIndex( StateSize )
{
    m_nMT[0] = nSeed;
    for( uint i = 1; i < StateSize; ++i )
    {
        m_nMT[i] = ( 0x6c078965 * ( m_nMT[i-1] ^ ( m_nMT[i-1] >> 30 ) ) + 1 );
    }
    Reset();
}

void CRandom::Reset()
{
    for( int i = 0; i < 624; ++i )
    {
        int n = ( m_nMT[i] & 0x80000000 ) | ( m_nMT[ (i + 1) % 624 ] & 0x7FFFFFFF );
        m_nMT[i] = m_nMT[ (i + 397) % 624 ] ^ ( n >> 1 );
        m_nMT[i] ^= ( ( n & 0x1 ) ? 0x9908b0df : 0x0 );
    }
    m_nIndex = StateSize;
}

uint CRandom::Rand()
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

} // namespace Riot