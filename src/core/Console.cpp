/*********************************************************\
File:           Console.cpp
Author:         Kyle Weicht
Created:        5/1/2011
Modified:       5/2/2011 7:44:28 PM
Modified by:    Kyle Weicht
 \*********************************************************/
#include <stdio.h>
#include <string> // TODO: String replacement
#include "Console.h"
#include "Input.h"
#include "Engine.h"
#include "renderer.h"


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
        Memset( m_szCurrCommand, 0, sizeof( m_szCurrCommand ) );
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CConsole::Shutdown( void )
    {
    }


    //-----------------------------------------------------------------------------
    //  AddLine
    //-----------------------------------------------------------------------------
    void CConsole::AddLine( const char* szText )
    {
        strcpy( m_Commands[ m_nCurrCommand ].szCommand, szText );
        m_nCurrCommand++;
    }

    //-----------------------------------------------------------------------------
    //  Render
    //-----------------------------------------------------------------------------
    void CConsole::Render( void )
    {
        static CRenderer* pRenderer = Engine::GetRenderer();

        if( !gnConsoleActive )
            return;

        for( uint i = 0; i < m_nCurrCommand; ++i )
        {
            pRenderer->DrawString( 0, i*15, m_Commands[i].szCommand );
        }
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
        while( szCmd[nIndex] != ' ' && szCmd[nIndex] != 0 )
        {
            szVariable[nIndex] = szCmd[nIndex];

            nIndex++;
        }
        
        // There is no value, just add it and return
        if( szCmd[nIndex] == 0 )
        {
            szVariable[nIndex] = 0;
            AddLine( szVariable );
            return;
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
        int nValue = 0;
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
        case eCommandInteger:
            SetSetting( szVariable, nValue );
            break;
        default:
            break;
        }

        AddLine( szCmd );
        m_nCurrPos = 0;

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
                gnConsoleActive = !gnConsoleActive;
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
