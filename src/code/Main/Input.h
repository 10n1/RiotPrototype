/*********************************************************\
File:       Input.h
Purpose:    User IO system
\*********************************************************/
#ifndef _INPUT_H_
#define _INPUT_H_
#include "Common.h"
#include "Types.h"
#include "Memory.h"
#include "IRefCounted.h"

#if defined( WIN32 ) || defined( WIN64 )
#include <Windows.h>
#endif // #if defined( WIN32 ) || defined( WIN64 )

enum eMouseButton
{
    eLMouseButton = 0x1,
    eRMouseButton = 0x2,
};

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

    //-----------------------------------------------------------------------------
    //  GetMouseDelta
    //  Returns how much the mouse has changed since last frame
    //-----------------------------------------------------------------------------
    sint GetMouseDeltaX( void );
    sint GetMouseDeltaY( void );

    //-----------------------------------------------------------------------------
    //  SetMouseButtonState
    //  Sets whether or not the mouse buttons are down
    //  TODO: Handle all input with messages like this
    //-----------------------------------------------------------------------------
    void SetMouseButtonState( uint8 nMouseState );

    //-----------------------------------------------------------------------------
    //  IsMouseDown
    //  Returns if the mouse button is down
    //-----------------------------------------------------------------------------
    bool IsMouseDown( uint8 nButton );

private:

//---------------------------------------------------------------------------------
//  Members
private:
    uint8   m_pKeys[256];
    sint    m_nMouseX;
    sint    m_nMouseY;
    sint    m_nMouseDeltaX;
    sint    m_nMouseDeltaY;
    uint8   m_nMouseState;
};


#endif //#ifndef _INPUT_H_
