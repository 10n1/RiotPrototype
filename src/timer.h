//
//  timer.h
//  riot
//
//  Created by Kyle Weicht on 4/8/11.
//  Copyright 2011 none. All rights reserved.
//
#include "common.h"
#include <mach/mach_time.h>

class CTimer
{
public:
    CTimer()
    {        
        mach_timebase_info_data_t freq = { 0 };
        mach_timebase_info( &freq );
        m_fFrequency = (float64)freq.numer / (float64)freq.denom;
        m_fFrequency *= 1e-9;
        gs_PrevTime = gs_StartTime = mach_absolute_time();
    }
    
    double GetElapsedTime( void )
    {        
        float64 dTime;
        gs_CurrTime = mach_absolute_time();
        dTime = (gs_CurrTime - gs_PrevTime) * m_fFrequency;
        gs_PrevTime = gs_CurrTime;
        return dTime;
    }
    
private:
    float64     m_fFrequency;
    uint64      gs_PrevTime;
    uint64      gs_CurrTime;
    uint64      gs_StartTime;
};
