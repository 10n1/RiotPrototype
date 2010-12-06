/*********************************************************\
File:       Timer.h
Purpose:    High performance, high precision timer  
\*********************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // #ifdef WIN32

class Timer
{
private:
    double          m_Freq;
#ifdef WIN32
    LARGE_INTEGER   m_PrevTime;
    LARGE_INTEGER   m_CurrTime;
#endif // #ifdef WIN32

public:
    Timer() 
        : m_Freq(0)
    { }
    ~Timer() { }

    // Reset timer
    __forceinline void Reset(void)
    {
#ifdef WIN32
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        m_Freq = 1.0/(double)freq.QuadPart;
        QueryPerformanceCounter(&m_PrevTime);
#endif // #ifdef WIN32
    }

    // Return time in seconds since last GetTime
    __forceinline double GetTime(void)
    {
#ifdef WIN32
        QueryPerformanceCounter(&m_CurrTime);
        double dTime = ((m_CurrTime.QuadPart - m_PrevTime.QuadPart) * m_Freq);
        m_PrevTime = m_CurrTime;
        return dTime;
#endif // #ifdef WIN32
    }
};

#endif // #ifndef _TIMER_H_
