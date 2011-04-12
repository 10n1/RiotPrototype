/*********************************************************\
File:           MessageDispatcher.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 8:15:41 PM
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
        m_nNumMessages[0] = 0;
        m_nNumMessages[1] = 0;

        m_nCurrentQueue = 0;
        m_pCurrentIndex = &m_nNumMessages[m_nCurrentQueue];
        m_pCurrentQueue = m_pMessages[m_nCurrentQueue];
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
        if( *m_pCurrentIndex >= MESSAGE_QUEUE_LENGTH )
        {
            DispatchMsg( msg );
            return;
        }

        uint nIndex = AtomicIncrement( m_pCurrentIndex ) - 1;
        m_pCurrentQueue[ nIndex ] = msg;
    }
    void CMessageDispatcher::PostMsg( MessageType nType )
    {
        PostMsg( TMessage( nType ) );
    }

    //-----------------------------------------------------------------------------
    //  SendMsg
    //  Sends a message immediately
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::SendMsg( const TMessage& msg )
    {
        DispatchMsg( msg );
    }
    void CMessageDispatcher::SendMsg( MessageType nType )
    {
        DispatchMsg( TMessage(nType) );
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessages
    //  Processes all posted messages
    //-----------------------------------------------------------------------------
    void CMessageDispatcher::ProcessMessages( void )
    {
        uint nQueueToProcess = m_nCurrentQueue;

        m_nCurrentQueue = (++m_nCurrentQueue % 2);
        m_pCurrentIndex = &m_nNumMessages[m_nCurrentQueue];
        m_pCurrentQueue = m_pMessages[m_nCurrentQueue];

        uint nCount = m_nNumMessages[nQueueToProcess];
        for( uint i = 0; i < nCount; ++i )
        {
            DispatchMsg( m_pMessages[nQueueToProcess][i] );
        }

        m_nNumMessages[nQueueToProcess] = 0;
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
