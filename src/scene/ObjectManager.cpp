/*********************************************************\
File:           ObjectManager.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 5:15:11 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ObjectManager.h"
#include "Engine.h"
#include "ComponentManager.h"

namespace Riot
{


    /***************************************\
    | class members                         |
    \***************************************/
    const MessageType    CObjectManager::MessagesReceived[] =
    {
        mNULLMESSAGE,
    };
    const uint           CObjectManager::NumMessagesReceived = 0; //ARRAY_LENGTH(MessagesReceived);


    // CComponentManager constructor
    CObjectManager::CObjectManager()
    {
    }

    // CComponentManager destructor
    CObjectManager::~CObjectManager()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CObjectManager::Initialize( void )
    {
        m_nNumObjects   = 0;
        m_nNumFreeSlots = MAX_OBJECTS;

        for( sint i = MAX_OBJECTS - 1, j = 0; i >= 0; --i, ++j )
        {
            m_pFreeSlots[j] = i;
        }
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CObjectManager::Shutdown( void )
    {
        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            // Reset them all so the active components can free themselves
            m_pObjects[i].Reset();
        }
    }

    //-----------------------------------------------------------------------------
    //  CreateObject
    //  Creates a new object and returns its index
    //-----------------------------------------------------------------------------
    uint CObjectManager::CreateObject( void )
    {
        assert( m_nNumObjects < MAX_OBJECTS );
        assert( m_nNumFreeSlots );

        uint nFreeIndex = AtomicDecrement( &m_nNumFreeSlots );

        uint nObjectIndex = m_pFreeSlots[ nFreeIndex ];

        AtomicIncrement( &m_nNumObjects );

        return nObjectIndex;
    }

    //-----------------------------------------------------------------------------
    //  GetObject
    //  Returns the object at the specified index
    //-----------------------------------------------------------------------------
    CObject* CObjectManager::GetObject( uint nIndex )
    {
        return &m_pObjects[nIndex];
    }

    //-----------------------------------------------------------------------------
    //  AddComponent
    //  Adds a component to the specified object
    //-----------------------------------------------------------------------------
    void CObjectManager::AddComponent( uint nIndex, eComponentType nType )
    {
        if( m_pObjects[nIndex].m_pComponentIndices[ nType ] == -1 )
        {    
            m_pObjects[nIndex].m_pComponentIndices[ nType ] = Engine::GetComponentManager()->AddComponent( nType, &m_pObjects[nIndex] );
        }
        // TODO: Handle case if it already exists
    }

    //-----------------------------------------------------------------------------
    //  RemoveComponent
    //  Removes a component from the specified object
    //-----------------------------------------------------------------------------
    void CObjectManager::RemoveComponent( uint nIndex, eComponentType nType )
    {
        // Make sure we have a component to detach
        sint nComponentIndex = m_pObjects[nIndex].m_pComponentIndices[ nType ];
        if( nComponentIndex != -1 )
        {
            Engine::GetComponentManager()->RemoveComponent( nType, nComponentIndex );
            m_pObjects[nIndex].m_pComponentIndices[ nType ] = -1;
        }

        // TODO: Handle case if it doesnt exist
    }

    //-----------------------------------------------------------------------------
    //  DeleteObject
    //  "Deletes" the object, freeing that slot
    //-----------------------------------------------------------------------------
    void CObjectManager::DeleteObject( uint nIndex )
    {
        assert( m_nNumFreeSlots < MAX_OBJECTS );

        m_pObjects[nIndex].Reset();

        m_pFreeSlots[ m_nNumFreeSlots++ ] = nIndex;
        --m_nNumObjects;
    }

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    uint CObjectManager::GetNumObjects( void )
    {
        return m_nNumObjects;
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CObjectManager::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        default:
            {
                break;
            }
        }
    }

} // namespace Riot
