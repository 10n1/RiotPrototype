/*********************************************************\
File:           timer.cpp
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 10:31:00 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "timer.h"

#ifdef OS_WINDOWS
#include <Windows.h>
#else
#include <mach/mach_time.h>
#endif

CTimer::CTimer()
    : m_fFrequency( 0.0 )
    , m_fCurrTime( 0 )
    , m_fPrevTime( 0 )
    , m_fStartTime( 0 )
{        
}

//-----------------------------------------------------------------------------
//  Reset
//  Resets the start time of the timer, and recalculates precision
//-----------------------------------------------------------------------------
void CTimer::Reset( void )
{
#ifdef OS_WINDOWS
    LARGE_INTEGER   freq;
    QueryPerformanceFrequency( &freq );
    m_fFrequency = 1.0/(double)freq.QuadPart;

    QueryPerformanceCounter( (LARGE_INTEGER*)&m_fStartTime );
    m_fPrevTime = m_fStartTime;
#else
    mach_timebase_info_data_t freq = { 0 };
    mach_timebase_info( &freq );
    m_fFrequency = (float64)freq.numer / (float64)freq.denom;
    m_fFrequency *= 1e-9;
    m_fPrevTime = m_fStartTime = mach_absolute_time();
#endif
}

//-----------------------------------------------------------------------------
//  GetElapsedTime
//  Returns the elapsed time (in seconds) since the last call to GetElapsedTime
//-----------------------------------------------------------------------------
double CTimer::GetElapsedTime( void )
{
    float64 dTime;
#ifdef OS_WINDOWS
    QueryPerformanceCounter( (LARGE_INTEGER*)&m_fCurrTime );
    dTime = ( m_fCurrTime - m_fPrevTime ) * m_fFrequency;
    m_fPrevTime = m_fCurrTime;
#else
    m_fCurrTime = mach_absolute_time();
    dTime = (m_fCurrTime - m_fPrevTime) * m_fFrequency;
    m_fPrevTime = m_fCurrTime;
#endif
    return dTime;
}

//-----------------------------------------------------------------------------
//  GetRunningTime
//  Returns the time (in seconds) since the last call to Reset
//-----------------------------------------------------------------------------
double CTimer::GetRunningTime( void )
{
    float64 dTime;
#ifdef OS_WINDOWS
    QueryPerformanceCounter( (LARGE_INTEGER*)&m_fCurrTime );
    dTime = ( m_fCurrTime - m_fStartTime ) * m_fFrequency;
#else
    m_fCurrTime = mach_absolute_time();
    dTime = (m_fCurrTime - m_fStartTime) * m_fFrequency;
#endif
    return dTime;
}
