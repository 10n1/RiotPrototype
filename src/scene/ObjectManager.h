/*********************************************************\
File:           ObjectManager.h
Purpose:        Handles allocation of objects
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       4/19/2011 10:53:40 PM
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
        void RemoveComponent( uint nObject, eComponentType nType );

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
        //  GetComponentIndex
        //  Returns which slot in the component this object owns
        //-----------------------------------------------------------------------------
        sint GetComponentIndex( uint nObject, eComponentType nComponent );

        //-----------------------------------------------------------------------------
        //  Accessors/mutators
        //-----------------------------------------------------------------------------
        uint GetNumObjects( void );
        
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

        //CObject     m_pObjects[MAX_OBJECTS];
        sint        m_pComponentIndices[MAX_OBJECTS][eNUMCOMPONENTS];

        uint        m_pFreeSlots[MAX_OBJECTS];
        atomic_t    m_nNumFreeSlots;

        atomic_t    m_nNumObjects;
    };

} // namespace Riot

#endif // #ifndef _OBJECTMANAGER_H_
