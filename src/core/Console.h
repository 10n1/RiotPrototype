/*********************************************************\
File:           Console.h
Purpose:        The console for entering commands
Author:         Kyle Weicht
Created:        5/1/2011
Modified:       5/2/2011 8:45:44 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include "common.h"
#include "IListener.h"
#include "Settings.h"

namespace Riot
{

    class CConsole : public IListener
    {
        friend class Engine;
        struct TCommand;
    public:
        // CConsole constructor
        CConsole();

        // CConsole destructor
        ~CConsole();
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
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );

    private:
        //-----------------------------------------------------------------------------
        //  ParseCommand
        //  Parses the command when ENTER is pressed
        //-----------------------------------------------------------------------------
        void ParseCommand( const char* szCmd );
        
    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        enum
        {
            MAX_COMMANDS = 1024,
            MAX_COMMAND_LENGTH = 256,
        };
        struct TCommand
        {
            char    szCommand[MAX_COMMAND_LENGTH];
        };

        TCommand    m_Commands[MAX_COMMANDS];

        char        m_szCurrCommand[MAX_COMMAND_LENGTH];

        uint        m_nCurrPos;
        uint        m_nCurrCommand;
    };


} // namespace Riot

#endif // #ifndef _CONSOLE_H_
