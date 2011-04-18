/*********************************************************\
File:           InputManager.h
Purpose:        Interface between the engine and the hardware
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/17/2011 6:45:41 PM
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
            DOWN    = 0x1,
            UP      = 0,
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
        uint8   m_nMouseButtons[ 8 ];

        sint    m_nMousePosition;
    };

}

#endif // #ifndef _INPUTMANAGER_H_
