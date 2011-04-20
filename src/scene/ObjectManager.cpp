/*********************************************************\
File:           ObjectManager.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/19/2011 11:01:40 PM
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
        
        for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        {
            for( uint j = 0; j < MAX_OBJECTS; ++j )
            {
                m_pComponentIndices[j][i] = -1; // -1 means the object doesn't have that component
            }
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
            ResetObject( i );
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
    //  AddComponent
    //  Adds a component to the specified object
    //-----------------------------------------------------------------------------
    void CObjectManager::AddComponent( uint nObject, eComponentType nType )
    {
        //if( m_pObjects[nIndex].m_pComponentIndices[ nType ] == -1 )
        //{    
        //    m_pObjects[nIndex].m_pComponentIndices[ nType ] = Engine::GetComponentManager()->AddComponent( nType, &m_pObjects[nIndex] );
        //}
        
        if( m_pComponentIndices[nObject][ nType ] == -1 )
        {    
            m_pComponentIndices[nObject][ nType ] = Engine::GetComponentManager()->AddComponent( nType, nObject );
        }
        // TODO: Handle case if it already exists
    }

    //-----------------------------------------------------------------------------
    //  RemoveComponent
    //  Removes a component from the specified object
    //-----------------------------------------------------------------------------
    void CObjectManager::RemoveComponent( uint nObject, eComponentType nType )
    {
        // Make sure we have a component to detach
        sint nComponentIndex = m_pComponentIndices[nObject][nType];
        if( nComponentIndex != -1 )
        {
            Engine::GetComponentManager()->RemoveComponent( nType, nComponentIndex );
            m_pComponentIndices[nObject][nType] = -1;
        }

        // TODO: Handle case if it doesnt exist
    }

    //-----------------------------------------------------------------------------
    //  DeleteObject
    //  "Deletes" the object, freeing that slot
    //-----------------------------------------------------------------------------
    void CObjectManager::DeleteObject( uint nObject )
    {
        assert( m_nNumFreeSlots < MAX_OBJECTS );

        ResetObject( nObject );

        m_pFreeSlots[ m_nNumFreeSlots++ ] = nObject;
        --m_nNumObjects;
    }

    //-----------------------------------------------------------------------------
    //  ResetObject
    //  Removes all components and resets the object
    //-----------------------------------------------------------------------------
    void CObjectManager::ResetObject( uint nObject )
    {
        for( eComponentType i = (eComponentType)0; i < eNUMCOMPONENTS; i = (eComponentType)(i + 1) )
        {
            // Make sure we have a component to detach
            if( m_pComponentIndices[nObject][i] != -1 )
            {
                Engine::GetComponentManager()->RemoveComponent( i, m_pComponentIndices[nObject][i] );
                m_pComponentIndices[nObject][i] = -1;
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  GetComponentIndex
    //  Returns which slot in the component this object owns
    //-----------------------------------------------------------------------------
    sint CObjectManager::GetComponentIndex( uint nObject, eComponentType nComponent )
    {
        return m_pComponentIndices[nObject][nComponent];
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
