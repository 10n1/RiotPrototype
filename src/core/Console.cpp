/*********************************************************\
File:           Console.cpp
Author:         Kyle Weicht
Created:        5/1/2011
Modified:       5/2/2011 8:44:04 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Console.h"
#include "Input.h"
#include "Engine.h"

#include <stdio.h>

namespace Riot
{
    enum eCommandType
    {
        eCommandInteger,
        eCommandBoolean,
        eCommandNull,
    };
    /***************************************\
    | class members                         |
    \***************************************/
    const MessageType    CConsole::MessagesReceived[] =
    {
        mKeyPressed,
    };
    const uint           CConsole::NumMessagesReceived = ARRAY_LENGTH(MessagesReceived);


    // CConsole constructor
    CConsole::CConsole()
    {
    }

    // CConsole destructor
    CConsole::~CConsole()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CConsole::Initialize( void )
    {
        m_nCurrCommand = 0;

        m_nCurrPos = 0;

        Memset( m_Commands, 0, sizeof( m_Commands ) );
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CConsole::Shutdown( void )
    {
    }

    //-----------------------------------------------------------------------------
    //  ParseCommand
    //  Parses the command when ENTER is pressed
    //-----------------------------------------------------------------------------
    void CConsole::ParseCommand( const char* szCmd )
    {
        static const uint szBoolTrueHash = StringHash32CaseInsensitive( "true" );
        static const uint szBoolFalseHash = StringHash32CaseInsensitive( "false" );


        char szVariable[256] = { 0 };
        char szValue[256] = { 0 };

        uint nIndex = 0;
        while( szCmd[nIndex] != ' ' )
        {
            szVariable[nIndex] = szCmd[nIndex];

            nIndex++;
        }

        while( szCmd[nIndex] == ' ' )
        {
            nIndex++;
        }

        uint nValueIndex = 0;
        
        while( szCmd[nIndex] != 0 )
        {
            szValue[nValueIndex++] = szCmd[nIndex];
            nIndex++;
        }

        uint nVariableHash = StringHash32CaseInsensitive( szValue );

        bool bValue = true;
        uint nValue = 0;
        eCommandType nType = eCommandInteger;
        if( nVariableHash == szBoolTrueHash )
        {
            nType = eCommandBoolean;
            bValue = true;
        }
        else if( nVariableHash == szBoolFalseHash ) 
        {
            nType = eCommandBoolean;
            bValue = false;
        }
        else
        {
            uint nNewValue = 0;
            nIndex = 0;
            while( szValue[nIndex] != 0 )
            {
                if( szValue[nIndex] < '0' || szValue[nIndex] > '9' )
                {
                    nType = eCommandNull;
                    break;
                }
                nIndex++;
            }

            if( nType != eCommandNull )
            {
                // TODO: convert to integer
                nValue = szValue[nIndex-1] - '0';
            }
        }

        switch( nType )
        {
        case eCommandBoolean:
            SetBoolSetting( szVariable, bValue );
            break;
        case eCommandInteger:
            break;
        default:
            break;
        }

        m_nCurrPos = 0;
        m_nCurrCommand++;

        Memset( m_szCurrCommand, 0, sizeof( m_szCurrCommand ) );
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CConsole::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        case mKeyPressed:

            switch( msg.nMessage )
            {
            case KEY_ESCAPE:
                Engine::PostMsg( msg );
                break;
            case KEY_TILDE:
                gbConsoleActive = !gbConsoleActive;
                break;
            case KEY_ENTER:
                m_szCurrCommand[ m_nCurrPos++ ] = 0;
                ParseCommand( m_szCurrCommand );
                break;
            case KEY_BACKSPACE:
                if( m_nCurrPos > 0 )
                {
                    m_szCurrCommand[ --m_nCurrPos ] = 0;
                }
                break;
            default:                
                m_szCurrCommand[ m_nCurrPos++ ] = (char)msg.nMessage;
                //printf( "%c", (char)msg.nMessage );
                printf( "%s\n", m_szCurrCommand );
            }
            break;
        default:
            break;
        }
    }

} // namespace Riot
