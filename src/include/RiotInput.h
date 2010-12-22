/*********************************************************\
File:       RiotInput.h
Purpose:    User IO system
\*********************************************************/
#ifndef _RIOTINPUT_H_
#define _RIOTINPUT_H_
#include "RiotTypes.h"
#include "RiotMemory.h"
#include "IRefCounted.h"

#if defined( WIN32 ) || defined( WIN64 )
#include <Windows.h>
#endif // #if defined( WIN32 ) || defined( WIN64 )

class RiotInput : public IRefCounted
{
//---------------------------------------------------------------------------------
//  Methods
public:
    RiotInput( void );
    ~RiotInput( void );
    
    //-----------------------------------------------------------------------------
    //  PollInput
    //  Gets the current state of the IO devices
    //-----------------------------------------------------------------------------
    void PollInput( void );

    //-----------------------------------------------------------------------------
    //  IsKeyDown
    //  Returns if the key is down, regardless of previous state
    //-----------------------------------------------------------------------------
    bool IsKeyDown( uint8 nKey );
    bool IsKeyUp( uint8 nKey );

    //-----------------------------------------------------------------------------
    //  WasKeyPressed
    //  Returns if the key was just pressed and isn't held down
    //-----------------------------------------------------------------------------
    bool WasKeyPressed( uint8 nKey );
private:

//---------------------------------------------------------------------------------
//  Members
private:
    uint8   m_pKeys[256];
};


#endif //#ifndef _RIOTINPUT_H_
