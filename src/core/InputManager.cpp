/*********************************************************\
File:           InputManager.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       5/2/2011 7:40:33 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "InputManager.h"
#include "Message.h"
#include "Engine.h"
#include "Input.h"
#include "Console.h"

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
        mHardwareMouseDown,
        mHardwareMouseUp,
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
            m_pKeys[i] = UP;
        }

        for( uint i = 0; i < 8; ++i )
        {
            m_nMouseButtons[i] = 0;
        }

        m_nMousePosition    = 0;
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
        if( gnConsoleActive )
        {
            for( uint i = 0; i < 256; ++i )
            {
                if( m_pKeys[i] == DOWN )
                {
                    Engine::GetConsole()->ProcessMessage( TMessage( mKeyDown, i ) );
                }
            }
        }
        else
        {
            for( uint i = 0; i < 256; ++i )
            {
                if( m_pKeys[i] == DOWN )
                {
                    Engine::PostMsg( TMessage( mKeyDown, i ) );
                }
            }
        }

        for( uint i = 0; i < 8; ++i )
        {
            if( m_nMouseButtons[i] == DOWN )
            {
                
                uint64 nMsg = i;
                nMsg = nMsg << 32;
                nMsg |= m_nMousePosition;
                
                Engine::PostMsg( TMessage( mMouseButtonDown, nMsg ) );
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
                uint8 nKey = (uint8)msg.nMessage;
                uint8& theKey = m_pKeys[ nKey ];

                if( theKey == UP )
                {
                    theKey = DOWN;

                    if( gnConsoleActive )
                    {
                        Engine::GetConsole()->ProcessMessage( TMessage( mKeyPressed, nKey ) );
                    }
                    else
                    {
                        Engine::SendMsg( TMessage( mKeyPressed, nKey ) );
                    }
                }

                break;
            }
        case mHardwareKeyboardUp:
            {
                uint8 nKey = (uint8)msg.nMessage;
                uint8& theKey = m_pKeys[ nKey ];

                if( theKey != UP )
                {
                    theKey = UP;

                    if( gnConsoleActive )
                    {
                        Engine::GetConsole()->ProcessMessage( TMessage( mKeyUp, nKey ) );
                    }
                    else
                    {
                        Engine::PostMsg( TMessage( mKeyUp, nKey ) );
                    }
                }

                break;
            }
        case mHardwareMouseDown:
            {
                uint8 nButton = (uint8)msg.nMessage;
                uint8& theButton = m_nMouseButtons[ nButton ];

                
                if( theButton == UP )
                {
                    theButton = DOWN;
                    
                    uint64 nMsg = nButton;
                    nMsg = nMsg << 32;
                    nMsg |= m_nMousePosition;

                    Engine::SendMsg( TMessage( mMouseButtonPressed, nMsg ) );
                }

                break;
            }
        case mHardwareMouseUp:
            {
                uint8 nButton = (uint8)msg.nMessage;
                uint8& theButton = m_nMouseButtons[ nButton ];

                if( theButton != UP )
                {
                    theButton = UP;                    
                    
                    uint64 nMsg = nButton;
                    nMsg = nMsg << 32;
                    nMsg |= m_nMousePosition;

                    Engine::PostMsg( TMessage( mMouseButtonUp, nMsg ) );
                }

                break;
            }
        case mHardwareMouseMove:
            {
                sint nNewPos = (sint)msg.nMessage;
                
                sint16 nXPos = nNewPos >> 16;
                sint16 nYPos = nNewPos & 0xFFFF;

                sint16 nPrevPosX = (m_nMousePosition >> 16);
                sint16 nPrevPosY = (m_nMousePosition & 0XFFFF);

                sint16 nDeltaX = nXPos - nPrevPosX;
                sint16 nDeltaY = nYPos - nPrevPosY;

                sint nDelta = (nDeltaX << 16) | (nDeltaY & 0xFFFF);

                Engine::PostMsg( TMessage( mMouseMove, nDelta ) );

                m_nMousePosition = nNewPos;

                break;
            }
        default:
            {
                break;
            }
        }
    }

} // namespace Riot
