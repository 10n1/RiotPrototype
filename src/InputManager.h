/*********************************************************\
File:           InputManager.h
Purpose:        Interface between the engine and the hardware
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 8:29:49 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _INPUTMANAGER_H_
#define _INPUTMANAGER_H_
#include "common.h"
#include "IListener.h"

namespace Riot
{

    class CInputManager : public IListener
    {
    private:
        friend class Engine;

        enum
        {
            KEY_PRESSED = 0x1,
            KEY_DOWN    = 0x2,
            KEY_UP      = 0,
        };
    public:
        // CInputManager constructor
        CInputManager();

        // CInputManager destructor
        ~CInputManager();
        /***************************************\
        | class methods                         |
        \***************************************/
        
        //-----------------------------------------------------------------------------
        //  Initialize
        //-----------------------------------------------------------------------------
        void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        void Shutdown( void );

        //-----------------------------------------------------------------------------
        //  ProcessInput
        //  Processes any input (sends messages for keys that are down)
        //-----------------------------------------------------------------------------
        void ProcessInput( void );

        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        uint8   m_pKeys[ 256 ];
    };

}

#endif // #ifndef _INPUTMANAGER_H_
