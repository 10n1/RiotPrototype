/*********************************************************\
File:           ComponentManager.h
Purpose:        Manages all the components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/24/2011 3:42:09 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTMANAGER_H_
#define _COMPONENTMANAGER_H_
#include "common.h"
#include "Component.h"
#include "IListener.h"

namespace Riot
{

#define MAX_COMPONENT_MESSAGES (1024*16)

    class CComponentManager : public IListener
    {
        friend class Engine;
    public:
        // CComponentManager constructor
        CComponentManager();

        // CComponentManager destructor
        ~CComponentManager();
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
        //  AddComponent
        //  Adds a component of the specified type
        //-----------------------------------------------------------------------------
        sint AddComponent( eComponentType nType, uint nObject );


        //-----------------------------------------------------------------------------
        //  RemoveComponent
        //  Removes a component of the specified type
        //-----------------------------------------------------------------------------
        void RemoveComponent( eComponentType nType, uint nIndex );


        //-----------------------------------------------------------------------------
        //  ProcessComponents
        //  Updates all the components, then resolves issues
        //-----------------------------------------------------------------------------
        void ProcessComponents( void );
        
        //-----------------------------------------------------------------------------
        //  GetComponent
        //  Returns the 
        //-----------------------------------------------------------------------------

        //-----------------------------------------------------------------------------
        //  PostMessage
        //  Posts a message to be processed
        //-----------------------------------------------------------------------------
        void PostMessage( CComponentMessage& msg );
        void PostMessage( eComponentMessageType nType, uint nObject, pvoid pData, eComponentType nOrigin = eNULLCOMPONENT);
        void PostMessage( eComponentMessageType nType, uint nObject, nativeuint nData, eComponentType nOrigin = eNULLCOMPONENT );


        //-----------------------------------------------------------------------------
        //  SendMessage
        //  Sends the message
        //-----------------------------------------------------------------------------
        void SendMessage( CComponentMessage& msg );
        void SendMessage( eComponentMessageType nType, uint nObject, pvoid pData, eComponentType nOrigin = eNULLCOMPONENT );
        void SendMessage( eComponentMessageType nType, uint nObject, nativeuint nData, eComponentType nOrigin = eNULLCOMPONENT );

#ifdef DEBUG    
        //-----------------------------------------------------------------------------
        //  LoadComponent
        //  This function is for debugging creation of components. In release it becomes
        //      a simple, non-templated macro
        //-----------------------------------------------------------------------------
        template< class TheComponent>
        void LoadComponent( void );
#endif
        
        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );

    private:
        //-----------------------------------------------------------------------------
        //  Parallel functions
        //  The task functions
        //-----------------------------------------------------------------------------
        static void ParallelProcessComponents( void* pData, uint nThreadId, uint nStart, uint nCount );
        static void ParallelProcessComponentMessages( void* pData, uint nThreadId, uint nStart, uint nCount );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        CComponentMessage   m_pMessages[MAX_COMPONENT_MESSAGES];
        CComponent*         m_ppComponents[eNUMCOMPONENTS];
        bool                m_bRegistered[eNUMCOMPONENTMESSAGES][eNUMCOMPONENTS];

        atomic_t            m_nNumMessages;
    };


} // namespace Riot

#endif // #ifndef _COMPONENTMANAGER_H_
