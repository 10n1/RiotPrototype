/*********************************************************\
File:       Input.cpp
Purpose:    Definition of the input system
\*********************************************************/
#include "Input.h"

RiotInput::RiotInput( )
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
    if( m_pKeys[nKey] == 0x81 )
        return true;
    
    return false;
}
