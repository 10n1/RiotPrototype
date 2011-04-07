/*********************************************************\
File:       Input.cpp
Purpose:    Definition of the input system
\*********************************************************/
#include "Input.h"
#include "Common.h"

RiotInput::RiotInput( )
    : m_nMouseDeltaX( 0 )
    , m_nMouseDeltaY( 0 )
    , m_nMouseX( 0 )
    , m_nMouseY( 0 )
{
    for( int i = 0; i < 256; ++i )
    {
        m_pKeys[i] = 0;
    }
}

RiotInput::~RiotInput( )
{
}

//-----------------------------------------------------------------------------
//  PollInput
//  Gets the current state of the IO devices
//-----------------------------------------------------------------------------
void RiotInput::PollInput( void )
{
    uint8 pKeys[256];

    GetKeyboardState( pKeys );

    for( int i = 0; i < 256; ++i )
    {
        if( pKeys[i] & 0x80 )
        { // The key is currently down
            if( m_pKeys[i] )
            {
                m_pKeys[i] = 0x81; // The key is held down
            }
            else
            {
                m_pKeys[i] = 0x80; // The key was just pressed
            }
        }
        else
        {
            m_pKeys[i] = 0x00; // Not down
        }
    }

    // Poll mouse
    POINT curPos;
    GetCursorPos( &curPos );
    m_nMouseDeltaX = curPos.x - m_nMouseX;
    m_nMouseDeltaY = curPos.y - m_nMouseY;

    m_nMouseX = curPos.x;
    m_nMouseY = curPos.y;
}

//-----------------------------------------------------------------------------
//  IsKeyDown
//  Returns if the key is down, regardless of previous state
//-----------------------------------------------------------------------------
bool RiotInput::IsKeyDown( uint8 nKey )
{
    if( m_pKeys[nKey] != 0x00 )
        return true;

    return false;
}

bool RiotInput::IsKeyUp( uint8 nKey )
{    
    if( m_pKeys[nKey] == 0x00 )
        return true;

    return false;
}

//-----------------------------------------------------------------------------
//  WasKeyPressed
//  Returns if the key was just pressed and isn't held down
//-----------------------------------------------------------------------------
bool RiotInput::WasKeyPressed( uint8 nKey )
{
    if( m_pKeys[nKey] == 0x80 )
        return true;

    return false;
}

//-----------------------------------------------------------------------------
//  GetMouseDelta
//  Returns how much the mouse has changed since last frame
//-----------------------------------------------------------------------------
sint RiotInput::GetMouseDeltaX( void )
{
    return m_nMouseDeltaX;
}

sint RiotInput::GetMouseDeltaY( void )
{
    return m_nMouseDeltaY;
}

//-----------------------------------------------------------------------------
//  SetMouseButtonState
//  Sets whether or not the mouse buttons are down
//  TODO: Handle all input with messages like this
//-----------------------------------------------------------------------------
void RiotInput::SetMouseButtonState( uint8 nMouseState )
{
    m_nMouseState = nMouseState;
}

//-----------------------------------------------------------------------------
//  IsMouseDown
//  Returns if the mouse button is down
//-----------------------------------------------------------------------------
bool RiotInput::IsMouseDown( uint8 nButton )
{
    return m_nMouseState & nButton;
}
