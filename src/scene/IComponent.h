/*********************************************************\
File:           IComponent.h
Purpose:        The definition for the base component
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/30/2011 12:21:00 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ICOMPONENT_H_
#define _ICOMPONENT_H_
#include "common.h"

namespace Riot
{
    
    /*****************************************************************************\
    \*****************************************************************************/

    //-----------------------------------------------------------------------------
    //  eComponentMessageType
    //  These are the types of the inter-component messages
    enum eComponentMessageType
    {
        eComponentMessageTransform,
        eComponentMessageUpdate,
        eComponentMessageMesh,
        eComponentMessageBoundingVolumeType,
        eComponentMessageObjectCollision,
        eComponentMessageTerrainCollision,

        eNUMCOMPONENTMESSAGES,
        eNULLCOMPONENTMESSAGE = -1
    };
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  eComponentType
    //  These are the actual types of the components
    enum eComponentType
    {
        eComponentRender,
        eComponentLight,
        eComponentCollidable,
        eComponentRigidBody,
        eComponentCharacter,

        eNUMCOMPONENTS,
        eNULLCOMPONENT = -1
    };
    //-----------------------------------------------------------------------------


    //-----------------------------------------------------------------------------
    //  CComponentMessage
    // These are the messages passed between components
    struct CComponentMessage
    {
        eComponentMessageType   m_nMessageType;
        uint                    m_nTargetObject;
        eComponentType          m_nOrigin;
        union
        {
            nativeuint  m_nData;
            pvoid       m_pData;
        };
    };
    //-----------------------------------------------------------------------------

    /*****************************************************************************\
    \*****************************************************************************/


    class IComponent
    {
        friend class CObjectManager;
    protected:
        virtual ~IComponent() { }
        
        //-----------------------------------------------------------------------------
        //  ProcessComponent
        //  Processes the component as necessary
        //-----------------------------------------------------------------------------
        virtual void ProcessComponent( void ) = 0;

        //-----------------------------------------------------------------------------
        //  ReceiveMessage
        //  Receives and processes a message
        //-----------------------------------------------------------------------------
        virtual void ReceiveMessage( uint nSlot, CComponentMessage& msg ) = 0;
        
        //-----------------------------------------------------------------------------
        //  Attach
        //  Attaches a component to an object
        //-----------------------------------------------------------------------------
        virtual void Attach( uint nObject ) = 0;
        inline void PreAttach( uint nObject );
        inline void PostAttach( uint nObject );

        //-----------------------------------------------------------------------------
        //  Rettach
        //  Reattaches a component to an object, using it's last data
        //-----------------------------------------------------------------------------
        virtual void Reattach( uint nObject  ) = 0;
        inline void PreReattach( uint nObject  );
        inline void PostReattach( uint nObject  );

        //-----------------------------------------------------------------------------
        //  Detach
        //  Detaches a component from an object, discarding the old data
        //-----------------------------------------------------------------------------
        virtual void Detach( uint nObject ) = 0;
        inline void PreDetach( uint nObject );
        inline void PostDetach( uint nObject );
        
        //-----------------------------------------------------------------------------
        //  DetachAndSave
        //  Detaches a component from an object, saving the old data
        //-----------------------------------------------------------------------------
        virtual void DetachAndSave( uint nObject ) = 0;
        inline void PreDetachAndSave( uint nObject );
        inline void PostDetachAndSave( uint nObject );

        //-----------------------------------------------------------------------------
        //  RemoveInactive
        //  Removes the inactive component
        //-----------------------------------------------------------------------------
        virtual void RemoveInactive( uint nObject ) = 0;
        inline void PreRemoveInactive( uint nObject );
        inline void PostRemoveInactive( uint nObject );

        static const uint MaxComponents = MAX_OBJECTS;
        
    protected:
        uint*       m_pObjectIndices;
        uint*       m_pComponentIndices;

        uint    m_nNumActiveComponents;
        uint    m_nNumInactiveComponents;

        uint    m_nIndex;
        uint    m_nNewIndex;
        uint    m_nOldIndex;
    };

    
    /*****************************************************************************\
    \*****************************************************************************/
    //
    //#define BEGIN_DEFINE_COMPONENT( Component )         \
    //    Component* Component::m_pInstance = NULL;       \
    //    Component::Component()                          \
    //    {                                               \
    //        m_nNumInactiveComponents = MaxComponents;   \
    //        m_nNumActiveComponents = 0;                 \
    //    }                                               \
    //    Component::~Component() { }                     \
    //    const eComponentMessageType Component::MessagesReceived[] =    \
    //    {
    //

    //
    //#define END_DEFINE_COMPONENT( Component )   \
    //    };                                      \
    //    const uint Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
    //

    //
    #define COMPONENT_REORDER_DATA( Data )  \
        Data[m_nIndex] = Data[ m_nNumActiveComponents ]

    //
    
    //
    #define COMPONENT_REORDER_SAVE_DATA( Data ) \
        Data[m_nNewIndex] = Data[m_nOldIndex];       \
        Data[m_nOldIndex] = Data[ m_nNumActiveComponents ]

    //

    //
    #define COMPONENT_USE_PREV_DATA( Data ) \
        Data[m_nIndex] = Data[m_nOldIndex];     \
        Data[m_nOldIndex] = Data[m_nNumInactiveComponents] 
    //
    
    //
    #define COMPONENT_REMOVE_PREV_DATA( Data ) \
        Data[m_nIndex] = Data[m_nNumInactiveComponents]
    //

    //
    //#define COMPONENT_DEFAULT_PRE_ATTACH    \
    //    uint nIndex = AtomicIncrement( &m_nNumActiveComponents ) - 1
    //
    
    //
    //#define COMPONENT_DEFAULT_POST_ATTACH   \
    //    m_pObjectIndices[nIndex] = nObject; \
    //    m_pComponentIndices[nObject] = nIndex
    //

    //
    //#define COMPONENT_DEFAULT_PRE_REATTACH  \
    //    uint nOldIndex = m_pComponentIndices[nObject]; \
    //    uint nIndex = AtomicIncrement( &m_nNumActiveComponents ) - 1
    //

    //
    //#define COMPONENT_DEFAULT_POST_REATTACH  \
    //    m_pObjectIndices[nIndex] = nObject; \
    //    m_pObjectIndices[nOldIndex] = m_pObjectIndices[ m_nNumInactiveComponents ]; \
    //    AtomicIncrement( &m_nNumInactiveComponents );   \
    //    m_pComponentIndices[nObject] = nIndex
    //

    //
    //#define COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE   \
    //    uint nIndex = m_pComponentIndices[nObject]
    //
    
    //
    //#define COMPONENT_DEFAULT_POST_REMOVE_INACTIVE  \
    //    m_pObjectIndices[ nIndex ] = m_pObjectIndices[ m_nNumInactiveComponents ]; \
    //    AtomicIncrement( &m_nNumInactiveComponents )
    //

    //
    //#define COMPONENT_DEFAULT_PRE_DETACH_SAVE   \
    //    AtomicDecrement( &m_nNumActiveComponents );   \
    //    uint nOldIndex = m_pComponentIndices[nObject]; \
    //    uint nNewIndex = AtomicDecrement( &m_nNumInactiveComponents )
    //
    
    //
    //#define COMPONENT_DEFAULT_POST_DETACH_SAVE  \
    //    m_pObjectIndices[nNewIndex] = nObject;  \
    //    m_pObjectIndices[nOldIndex] = m_pObjectIndices[ m_nNumActiveComponents ]; \
    //    m_pComponentIndices[m_pObjectIndices[ m_nNumActiveComponents ]] = nOldIndex; \
    //    m_pComponentIndices[nObject] = nNewIndex
    //
    
    //
    //#define COMPONENT_DEFAULT_PRE_DETACH \
    //    uint nIndex = m_pComponentIndices[nObject]; \
    //    AtomicDecrement( &m_nNumActiveComponents )
    //

    //
    //#define COMPONENT_DEFAULT_POST_DETACH
    //

    /*****************************************************************************\
    \*****************************************************************************/


    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void IComponent::PreAttach( uint nObject )
    {
        ASSERT( m_nNumActiveComponents < MaxComponents );
        ASSERT( m_nNumActiveComponents < m_nNumInactiveComponents );

        m_nIndex = m_nNumActiveComponents++;
    }

    void IComponent::PostAttach( uint nObject )
    {
        m_pObjectIndices[m_nIndex]      = nObject;
        m_pComponentIndices[nObject]    = m_nIndex;
    }


    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void IComponent::PreReattach( uint nObject )
    {
        ASSERT( m_nNumActiveComponents < MaxComponents );
        ASSERT( m_nNumActiveComponents < m_nNumInactiveComponents );

        m_nOldIndex = m_pComponentIndices[nObject];
        m_nIndex    = m_nNumActiveComponents++;
    }

    void IComponent::PostReattach( uint nObject )
    {
        m_pObjectIndices[m_nIndex]      = nObject;
        m_pObjectIndices[m_nOldIndex]   = m_pObjectIndices[ m_nNumInactiveComponents ];
        m_pComponentIndices[nObject]    = m_nIndex;
        m_nNumInactiveComponents        += 1;
    }


    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object, discarding the old data
    //-----------------------------------------------------------------------------
    void IComponent::PreDetach( uint nObject )
    {
        m_nIndex                = m_pComponentIndices[nObject];
        m_nNumActiveComponents  -= 1;
    }

    void IComponent::PostDetach( uint nObject )
    {
    }


    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void IComponent::PreDetachAndSave( uint nObject )
    {
        m_nNumActiveComponents  -= 1; 
        m_nOldIndex             = m_pComponentIndices[nObject];
        m_nNewIndex             = --m_nNumInactiveComponents;
    }

    void IComponent::PostDetachAndSave( uint nObject )
    {
        uint nOldObject = m_pObjectIndices[ m_nNumActiveComponents ];

        m_pObjectIndices[m_nNewIndex] = nObject;
        m_pObjectIndices[m_nOldIndex] = nOldObject;

        m_pComponentIndices[nOldObject] = m_nOldIndex;
        m_pComponentIndices[nObject]    = m_nNewIndex;
    }


    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void IComponent::PreRemoveInactive( uint nObject )
    {
        m_nIndex = m_pComponentIndices[nObject];
    }

    void IComponent::PostRemoveInactive( uint nObject )
    {
        m_pObjectIndices[ m_nIndex ]    = m_pObjectIndices[ m_nNumInactiveComponents ];
        m_nNumInactiveComponents        += 1;
    }


} // namespace Riot

#endif // #ifndef _ICOMPONENT_H_
