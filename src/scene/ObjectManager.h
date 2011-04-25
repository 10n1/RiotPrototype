/*********************************************************\
File:           ObjectManager.h
Purpose:        Handles allocation of objects
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       4/24/2011 9:54:49 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_
#include "common.h"
#include "Component.h"
#include "IListener.h"

namespace Riot
{

    class CObjectManager : public IListener
    {
        friend class Engine;
    public:
        // CObjectManager constructor
        CObjectManager();

        // CObjectManager destructor
        ~CObjectManager();
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
        //  CreateObject
        //  Creates a new object and returns its index
        //-----------------------------------------------------------------------------
        uint CreateObject( void );

        //-----------------------------------------------------------------------------
        //  AddComponent
        //  Adds a component to the specified object
        //-----------------------------------------------------------------------------
        void AddComponent( uint nObject, eComponentType nType );

        //-----------------------------------------------------------------------------
        //  RemoveComponent
        //  Removes a component from the specified object
        //-----------------------------------------------------------------------------
        void RemoveComponent( uint nObject, eComponentType nType, bool bSave = true );

        //-----------------------------------------------------------------------------
        //  ReorderComponent
        //  Lets the object know it has a new index
        //-----------------------------------------------------------------------------
        void ReorderComponent( uint nObject, eComponentType nType, sint nNewIndex );

        //-----------------------------------------------------------------------------
        //  DeleteObject
        //  "Deletes" the object, freeing that slot
        //-----------------------------------------------------------------------------
        void DeleteObject( uint nObject );

        //-----------------------------------------------------------------------------
        //  ResetObject
        //  Removes all components and resets the object
        //-----------------------------------------------------------------------------
        void ResetObject( uint nObject );

        //-----------------------------------------------------------------------------
        //  Accessors/mutators
        //-----------------------------------------------------------------------------
        uint GetNumObjects( void );
        
        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );
        
        //-----------------------------------------------------------------------------
        //  ProcessComponents
        //  Updates all the components, then resolves issues
        //-----------------------------------------------------------------------------
        void ProcessComponents( void );

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
        
    private:
        //-----------------------------------------------------------------------------
        //  Parallel functions
        //  The task functions
        //-----------------------------------------------------------------------------
        static void ParallelProcessComponents( void* pData, uint nThreadId, uint nStart, uint nCount );
        static void ParallelProcessComponentMessages( void* pData, uint nThreadId, uint nStart, uint nCount );

    private:
        enum 
        { 
            COMPONENT_REMOVED       = 0x80000000,
            COMPONENT_RESET_REMOVED = 0x7FFFFFFF,
            COMPONENT_FRESH         = 0x40000000,

            MAX_COMPONENT_MESSAGES  = 1024*16,
        };
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        // These two arrays are functionally identical and store the
        //  same data. The ObjectIndices array is solely for human
        //  readability. Every object is contiguous, wheras the
        //  ComponentIndices array is laid out for Components
        uint        m_pObjectIndices[eNUMCOMPONENTS][MAX_OBJECTS];
        uint        m_pComponentIndices[eNUMCOMPONENTS][MAX_OBJECTS];

        IComponent* m_pComponents[eNUMCOMPONENTS];

        CComponentMessage   m_pMessages[MAX_COMPONENT_MESSAGES];
        bool                m_bRegistered[eNUMCOMPONENTMESSAGES][eNUMCOMPONENTS];

        uint        m_pFreeSlots[MAX_OBJECTS];
        atomic_t    m_nNumFreeSlots;

        atomic_t    m_nNumObjects;

        atomic_t    m_nNumMessages;
    };

} // namespace Riot

#endif // #ifndef _OBJECTMANAGER_H_
