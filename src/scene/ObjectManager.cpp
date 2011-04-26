/*********************************************************\
File:           ObjectManager.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/25/2011 7:15:59 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ObjectManager.h"
#include "Engine.h"
#include "TaskManager.h"

namespace Riot
{
    /*****************************************************************************\
    \*****************************************************************************/
    #define LOAD_COMPONENT( TheComponent )                                          \
        TheComponent::m_pInstance = new TheComponent;                               \
        m_pComponents[ TheComponent::ComponentType ] = TheComponent::m_pInstance;   \
        TheComponent::m_pInstance->m_pObjectIndices = m_pObjectIndices[ TheComponent::ComponentType ]; \
        TheComponent::m_pInstance->m_pComponentIndices = m_pComponentIndices[ TheComponent::ComponentType ]; \
        for( uint i = 0; i < TheComponent::NumMessagesReceived; ++i )               \
        {                                                                           \
            m_bRegistered[ TheComponent::MessagesReceived[i] ][ TheComponent::ComponentType ] = true; \
        } 
    /*****************************************************************************\
    \*****************************************************************************/

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
        
        // Reset all object indices
        Memset( m_pObjectIndices, -1, sizeof( m_pObjectIndices ) );
        Memset( m_pComponentIndices, COMPONENT_FRESH, sizeof( m_pComponentIndices ) );

        // Zero everything out
        Memset( m_bRegistered, 0, sizeof( m_bRegistered ) );
        Memset( m_pComponents, 0, sizeof( m_pComponents ) );
        Memset( m_pMessages, 0, sizeof( m_pMessages ) );

        m_nNumMessages = 0;
        
        // Do this for each component
        LOAD_COMPONENT( CRenderComponent );
        LOAD_COMPONENT( CLightComponent );
        LOAD_COMPONENT( CNewtonPhysicsComponent );
        LOAD_COMPONENT( CCollidableComponent );
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CObjectManager::Shutdown( void )
    {
        for( sint i = 0; i < m_nNumObjects; ++i )
        {
            // Reset them all so the active components can free themselves
            ResetObject( i );
        }
        
        for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        {
            SAFE_DELETE( m_pComponents[i] );
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

        AtomicIncrement( &m_nNumObjects );
        uint nFreeIndex = AtomicDecrement( &m_nNumFreeSlots );

        uint nObjectIndex = m_pFreeSlots[ nFreeIndex ];

        return nObjectIndex;
    }
    
    //-----------------------------------------------------------------------------
    //  AddComponent
    //  Adds a component to the specified object
    //-----------------------------------------------------------------------------
    void CObjectManager::AddComponent( uint nObject, eComponentType nType )
    {
        CScopedMutex lock( &m_ObjectMutex );

        uint*       pComponentIndices   = m_pComponents[nType]->m_pComponentIndices;
        uint*       pObjectIndices      = m_pComponents[nType]->m_pObjectIndices;

        uint& nComponentIndex = pComponentIndices[nObject];

        atomic_t*   pActiveComponents   = &m_pComponents[nType]->m_nNumActiveComponents;
        atomic_t*   pInactiveComponents = &m_pComponents[nType]->m_nNumInactiveComponents;

        if( nComponentIndex == COMPONENT_FRESH )
        {
            // Let the component attach itself
            m_pComponents[nType]->Attach( nObject );

        }
        else if( nComponentIndex & COMPONENT_REMOVED )
        {
            nComponentIndex &= COMPONENT_RESET_REMOVED;

            // The component was added and removed
            // Let the component attach itself
            m_pComponents[nType]->Reattach( nObject );

        }
        else
        {
            // The component is already attached
        }
    }

    //-----------------------------------------------------------------------------
    //  RemoveComponent
    //  Removes a component from the specified object
    //-----------------------------------------------------------------------------
    void CObjectManager::RemoveComponent( uint nObject, eComponentType nType, bool bSave )
    {
        CScopedMutex lock( &m_ObjectMutex );

        uint*       pObjectIndices      = m_pComponents[nType]->m_pObjectIndices;
        uint*       pComponentIndices   = m_pComponents[nType]->m_pComponentIndices;

        uint&       nComponentIndex = pComponentIndices[nObject];

        if( nComponentIndex == COMPONENT_FRESH )
        {
            // There is no component to remove
            return;
        }

        if( (nComponentIndex & COMPONENT_REMOVED) && bSave == false )
        {
            nComponentIndex &= COMPONENT_RESET_REMOVED;

            // The component has been removed, but we want to nuke the data
            m_pComponents[nType]->RemoveInactive( nObject );

            nComponentIndex = COMPONENT_FRESH;
            return;
        }

        // Remove the component
        if( bSave )
        {            
            m_pComponents[nType]->DetachAndSave( nObject);

            nComponentIndex |= COMPONENT_REMOVED;
        }
        else
        {
            m_pComponents[nType]->Detach( nObject );

            nComponentIndex = COMPONENT_FRESH;
        }
    }

    //-----------------------------------------------------------------------------
    //  DeleteObject
    //  "Deletes" the object, freeing that slot
    //-----------------------------------------------------------------------------
    void CObjectManager::DeleteObject( uint nObject )
    {
        assert( m_nNumFreeSlots < MAX_OBJECTS );
        assert( nObject < m_nNumObjects );

        ResetObject( nObject );

        m_pFreeSlots[ AtomicIncrement(&m_nNumFreeSlots) ] = nObject;
        --m_nNumObjects;
    }

    //-----------------------------------------------------------------------------
    //  ResetObject
    //  Removes all components and resets the object
    //-----------------------------------------------------------------------------
    void CObjectManager::ResetObject( uint nObject )
    {
        for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        {
            eComponentType nType = (eComponentType)i;
            RemoveComponent( nObject, nType, false );
        }
    }

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    uint CObjectManager::GetNumObjects( void )
    {
        return m_nNumObjects;
    }
    
    //-----------------------------------------------------------------------------
    //  ProcessComponents
    //  Updates all the components, then resolves issues
    //-----------------------------------------------------------------------------
    void CObjectManager::ProcessComponents( void )
    {
#if PARALLEL_UPDATE
        static CTaskManager* pTaskManager = CTaskManager::GetInstance();

        // First update the components...
        task_handle_t nProcessTask = pTaskManager->PushTask( ParallelProcessComponents, this, eNUMCOMPONENTS, 1 );
        pTaskManager->WaitForCompletion( nProcessTask );

        // ...then resolve any discrepencies and handle messages
        task_handle_t nMessageTask = pTaskManager->PushTask( ParallelProcessComponentMessages, this, m_nNumMessages, 16 );
        pTaskManager->WaitForCompletion( nMessageTask );
#else
        ParallelProcessComponents( this, 0, 0, eNUMCOMPONENTS );
        ParallelProcessComponentMessages( this, 0, 0, m_nNumMessages );
#endif
        m_nNumMessages = 0;
    }
    void CObjectManager::ParallelProcessComponents( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CObjectManager* pManager = (CObjectManager*)pData;

        uint nEnd = nStart + nCount;
        for( uint i = nStart; i < nEnd; ++i )
        {
            pManager->m_pComponents[i]->ProcessComponent();
        }
    }

    void CObjectManager::ParallelProcessComponentMessages( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CObjectManager* pManager = (CObjectManager*)pData;

        uint nEnd = nStart + nCount;
        for( uint nMessage = nStart; nMessage < nEnd; ++nMessage )
        {   // Loop through the messages        
            pManager->SendMessage( pManager->m_pMessages[ nMessage ] );
        }
    }
    
    //-----------------------------------------------------------------------------
    //  PostMessage
    //  Posts a message to be processed
    //-----------------------------------------------------------------------------
    void CObjectManager::PostMessage( CComponentMessage& msg )
    {
        ASSERT( m_nNumMessages < MAX_COMPONENT_MESSAGES );

        sint nIndex = AtomicIncrement( &m_nNumMessages ) - 1;
        m_pMessages[ nIndex ] = msg;
    }

    void CObjectManager::PostMessage( eComponentMessageType nType, uint nObject, pvoid pData, eComponentType nOrigin )
    {
        //PostMessage( nType, nObject, (nativeuint)pData, nOrigin );
        ASSERT( m_nNumMessages < MAX_COMPONENT_MESSAGES );

        sint nIndex = AtomicIncrement( &m_nNumMessages ) - 1;

        m_pMessages[ nIndex ].m_pData = pData;
        m_pMessages[ nIndex ].m_nMessageType = nType;
        m_pMessages[ nIndex ].m_nTargetObject = nObject;
        m_pMessages[ nIndex ].m_nOrigin = nOrigin;
    }

    void CObjectManager::PostMessage( eComponentMessageType nType, uint nObject, nativeuint nData, eComponentType nOrigin )
    {
        ASSERT( m_nNumMessages < MAX_COMPONENT_MESSAGES );

        sint nIndex = AtomicIncrement( &m_nNumMessages ) - 1;

        m_pMessages[ nIndex ].m_nData = nData;
        m_pMessages[ nIndex ].m_nMessageType = nType;
        m_pMessages[ nIndex ].m_nTargetObject = nObject;
        m_pMessages[ nIndex ].m_nOrigin = nOrigin;
    }


    //-----------------------------------------------------------------------------
    //  SendMessage
    //  Sends the message
    //-----------------------------------------------------------------------------
    void CObjectManager::SendMessage( CComponentMessage& msg )
    {
        for( sint nComponent = 0; nComponent < eNUMCOMPONENTS; ++nComponent )
        {
            if(     m_bRegistered[ msg.m_nMessageType ][ nComponent ] == false
                ||  msg.m_nOrigin == nComponent )
            {
                // This component hasn't registered for this message type
                // OR this is the originating component
                continue;
            }

            uint*   pObjectIndices      = m_pComponents[nComponent]->m_pObjectIndices;
            uint*   pComponentIndices   = m_pComponents[nComponent]->m_pComponentIndices;

            sint nIndex = pComponentIndices[ msg.m_nTargetObject ] & COMPONENT_RESET_REMOVED;

            if( nIndex != COMPONENT_FRESH )
            {
                m_pComponents[ nComponent ]->ReceiveMessage( nIndex, msg );
            }
        }
    }

    void CObjectManager::SendMessage( eComponentMessageType nType, uint nObject, pvoid pData, eComponentType nOrigin )
    {
        CComponentMessage msg;
        
        msg.m_pData = pData;
        msg.m_nMessageType = nType;
        msg.m_nTargetObject = nObject;
        msg.m_nOrigin = nOrigin;

        SendMessage( msg );
    }

    void CObjectManager::SendMessage( eComponentMessageType nType, uint nObject, nativeuint nData, eComponentType nOrigin )
    {
        CComponentMessage msg;

        msg.m_nData = nData;
        msg.m_nMessageType = nType;
        msg.m_nTargetObject = nObject;
        msg.m_nOrigin = nOrigin;

        SendMessage( msg );
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
