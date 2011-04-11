/*********************************************************\
File:           MessageDispatcher.h
Purpose:        Dispatches engine messages
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 5:31:51 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MESSAGEDISPATCHER_H_
#define _MESSAGEDISPATCHER_H_
#include "common.h"
#include "Message.h"
#include "IListener.h"

namespace Riot
{
    class CMessageDispatcher
    {
    private:
        enum
        {
            MAX_REGISTERED_PER_MESSAGE = 16,
            MESSAGE_QUEUE_LENGTH = 1024,
        };
    public:
        // CMessageDispatcher constructor
        CMessageDispatcher();

        // CMessageDispatcher destructor
        ~CMessageDispatcher();
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
        //  RegisterListener
        //  Counts number registered for each message
        //-----------------------------------------------------------------------------
        void RegisterListener( IListener* pListener, const MessageType* pMessages, uint nCount );

        //-----------------------------------------------------------------------------
        //  PostMsg
        //  Posts a message to be processed later
        //-----------------------------------------------------------------------------
        void PostMsg( const TMessage& msg );
        void PostMsg( MessageType nType );
        
        //-----------------------------------------------------------------------------
        //  SendMsg
        //  Sends a message immediately
        //-----------------------------------------------------------------------------
        void SendMsg( const TMessage& msg );
        void SendMsg( MessageType nType );
        
        //-----------------------------------------------------------------------------
        //  ProcessMessages
        //  Processes all posted messages
        //-----------------------------------------------------------------------------
        void ProcessMessages( void );
        
    private:
        //-----------------------------------------------------------------------------
        //  DispatchMsg
        //  Sends the message to its listeners
        //-----------------------------------------------------------------------------
        void DispatchMsg( const TMessage& msg );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        IListener*  m_ppListeners[mNUMMESSAGES][MAX_REGISTERED_PER_MESSAGE];
        TMessage    m_pMessages[ MESSAGE_QUEUE_LENGTH ];
        uint        m_nNumRegistered[mNUMMESSAGES];

        atomic_t    m_nMessageIndex;

    };

} // namespace Riot

#endif // #ifndef _MESSAGEDISPATCHER_H_
