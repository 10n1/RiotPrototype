/*********************************************************\
File:           timer.h
Purpose:        High performance, high-resolution timer
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/8/2011 11:19:04 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_
#include "common.h"


class CTimer
{
public:
    CTimer();

    //-----------------------------------------------------------------------------
    //  Reset
    //  Resets the start time of the timer, and recalculates precision
    //-----------------------------------------------------------------------------
    void Reset( void );
    
    //-----------------------------------------------------------------------------
    //  GetElapsedTime
    //  Returns the elapsed time (in seconds) since the last call to GetElapsedTime
    //-----------------------------------------------------------------------------
    double GetElapsedTime( void );

private:
    float64     m_fFrequency;
    uint64      m_fPrevTime;
    uint64      m_fCurrTime;
    uint64      m_fStartTime;
};


#endif // #ifndef _TIMER_H_
