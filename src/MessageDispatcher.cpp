/*********************************************************\
File:           MessageDispatcher.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 4:09:03 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "MessageDispatcher.h"

namespace Riot
{
    // CMessageDispatcher constructor
    CMessageDispatcher::CMessageDispatcher()
    {
    }

    // CMessageDispatcher destructor
    CMessageDispatcher::~CMessageDispatcher()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::Initialize( void )
    {
        // Zero out arrays
        for( uint i = 0; i < mNUMMESSAGES; ++i )
        {
            for( uint j = 0; j < MAX_REGISTERED_PER_MESSAGE; ++j )
            {
                m_ppListeners[i][j] = NULL;
            }
            m_nNumRegistered[i] = 0;
        }

        // We have no messages pending
        m_nMessageIndex = 0;
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::Shutdown( void )
    {
    }

    //-----------------------------------------------------------------------------
    //  PostMsg
    //  Posts a message to be processed later
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::PostMsg( const TMessage& msg )
    {
        // If the queue is full, process it immediately
        if( m_nMessageIndex >= MESSAGE_QUEUE_LENGTH )
        {
            DispatchMsg( msg );
            return;
        }

        uint nIndex = AtomicIncrement( &m_nMessageIndex ) - 1;
        m_pMessages[ nIndex ] = msg;
    }

    //-----------------------------------------------------------------------------
    //  SendMsg
    //  Sends a message immediately
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::SendMsg( const TMessage& msg )
    {
        DispatchMsg( msg );
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessages
    //  Processes all posted messages
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::ProcessMessages( void )
    {
        uint nCount = m_nMessageIndex;
        for( uint i = 0; i < nCount; ++i )
        {
            DispatchMsg( m_pMessages[i] );
        }

        m_nMessageIndex = 0;
    }

    //-----------------------------------------------------------------------------
    //  DispatchMsg
    //  Sends the message to its listeners
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::DispatchMsg( const TMessage& msg )
    {
        uint nCount = m_nNumRegistered[ msg.nType ];

        for( uint i = 0; i < nCount; ++i )
        {
            m_ppListeners[ msg.nType ][i]->ProcessMessage( msg );
        }
    }

    //-----------------------------------------------------------------------------
    //  RegisterListener
    //  Registers the listener for messages
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::RegisterListener( IListener* pListener, const MessageType* pMessages, uint nCount )
    {
        for( uint i = 0; i < nCount; ++i )
        {
            MessageType nMessageType = pMessages[i];
            uint nIndex = m_nNumRegistered[ nMessageType ]++;
            m_ppListeners[ nMessageType ][ nIndex ] = pListener;
        }
    }

} // namespace Riot
