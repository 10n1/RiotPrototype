/*********************************************************\
File:           InputManager.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 8:31:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "InputManager.h"
#include "Message.h"
#include "Engine.h"

namespace Riot
{
    /***************************************\
    | class members                         |
    \***************************************/
    const MessageType    CInputManager::MessagesReceived[] =
    {
        mHardwareKeyboardDown,
        mHardwareKeyboardUp,
        mHardwareMouseMove,
        mHardwareMouseButton,
    };
    const uint           CInputManager::NumMessagesReceived = ARRAY_LENGTH(MessagesReceived);

    // CInputManager constructor
    CInputManager::CInputManager()
    {
    }

    // CInputManager destructor
    CInputManager::~CInputManager()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CInputManager::Initialize( void )
    {
        for( uint i = 0; i < 256; ++i )
        {
            m_pKeys[i] = KEY_UP;
        }
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CInputManager::Shutdown( void )
    {
    }

    //-----------------------------------------------------------------------------
    //  ProcessInput
    //  Processes any input (sends messages for keys that are down)
    //-----------------------------------------------------------------------------
    void CInputManager::ProcessInput( void )
    {
        for( uint i = 0; i < 256; ++i )
        {
            if( m_pKeys[i] == KEY_DOWN )
            {
                Engine::PostMsg( TMessage( mKeyDown, i ) );
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CInputManager::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        case mHardwareKeyboardDown:
            {
                uint8 nKey = msg.nMessage;
                uint8& theKey = m_pKeys[ nKey ];

                if( theKey == KEY_UP )
                {
                    theKey = KEY_DOWN;

                    Engine::SendMsg( TMessage( mKeyPressed, nKey ) );
                }

                break;
            }
        case mHardwareKeyboardUp:
            {
                uint8 nKey = msg.nMessage;
                uint8& theKey = m_pKeys[ nKey ];

                if( theKey != KEY_UP )
                {
                    theKey = KEY_UP;

                    Engine::SendMsg( TMessage( mKeyUp, nKey ) );
                }

                break;
            }
        case mHardwareMouseButton:
            {
                break;
            }
        case mHardwareMouseMove:
            {
                break;
            }
        }
    }

} // namespace Riot
