/*********************************************************\
File:           timer.h
Purpose:        High performance, high-resolution timer
Author:         Kyle Weicht
Created:        4/8/2011
Modified:       4/10/2011 3:15:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_
#include "common.h"

namespace Riot
{

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


        //-----------------------------------------------------------------------------
        //  GetRunningTime
        //  Returns the time (in seconds) since the last call to Reset
        //-----------------------------------------------------------------------------
        double GetRunningTime( void );

    private:
        float64     m_fFrequency;
        uint64      m_fPrevTime;
        uint64      m_fCurrTime;
        uint64      m_fStartTime;
    };

} // namespace Riot


#endif // #ifndef _TIMER_H_
