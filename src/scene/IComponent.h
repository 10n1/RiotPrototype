/*********************************************************\
File:           IComponent.h
Purpose:        The definition for the base component
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 9:14:29 PM
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
        eComponentMessageCollision,

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
        eComponentNewtonPhysics,
        eComponentCollidable,

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

        //-----------------------------------------------------------------------------
        //  Rettach
        //  Reattaches a component to an object, using it's last data
        //-----------------------------------------------------------------------------
        virtual void Reattach( uint nObject  ) = 0;

        //-----------------------------------------------------------------------------
        //  Detach
        //  Detaches a component from an object, discarding the old data
        //-----------------------------------------------------------------------------
        virtual void Detach( uint nObject ) = 0;
        
        //-----------------------------------------------------------------------------
        //  DetachAndSave
        //  Detaches a component from an object, saving the old data
        //-----------------------------------------------------------------------------
        virtual void DetachAndSave( uint nObject ) = 0;

        //-----------------------------------------------------------------------------
        //  RemoveInactive
        //  Removes the inactive component
        //-----------------------------------------------------------------------------
        virtual void RemoveInactive( uint nObject ) = 0;

        static const uint MaxComponents = MAX_OBJECTS;
        
    protected:
        uint*       m_pObjectIndices;
        uint*       m_pComponentIndices;
        atomic_t    m_nNumActiveComponents;
        atomic_t    m_nNumInactiveComponents;
    };

    
    /*****************************************************************************\
    \*****************************************************************************/
    //
    #define BEGIN_DEFINE_COMPONENT( Component )         \
        Component* Component::m_pInstance = NULL;       \
        Component::Component()                          \
        {                                               \
            m_nNumInactiveComponents = MaxComponents;   \
            m_nNumActiveComponents = 0;                 \
        }                                               \
        Component::~Component() { }                     \
        const eComponentMessageType Component::MessagesReceived[] =    \
        {
    //

    //
    #define END_DEFINE_COMPONENT( Component )   \
        };                                      \
        const uint Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
    //

    //
    #define COMPONENT_REORDER_DATA( Data )  \
        Data[nIndex] = Data[ m_nNumActiveComponents ]

    //
    
    //
    #define COMPONENT_REORDER_SAVE_DATA( Data ) \
        Data[nNewIndex] = Data[nOldIndex];       \
        Data[nOldIndex] = Data[ m_nNumActiveComponents ]

    //

    //
    #define COMPONENT_USE_PREV_DATA( Data ) \
        Data[nIndex] = Data[nOldIndex];     \
        Data[nOldIndex] = Data[m_nNumInactiveComponents] 
    //
    
    //
    #define COMPONENT_REMOVE_PREV_DATA( Data ) \
        Data[nIndex] = Data[m_nNumInactiveComponents]
    //

    //
    #define COMPONENT_DEFAULT_PRE_ATTACH    \
        uint nIndex = AtomicIncrement( &m_nNumActiveComponents ) - 1
    //
    
    //
    #define COMPONENT_DEFAULT_POST_ATTACH   \
        m_pObjectIndices[nIndex] = nObject; \
        m_pComponentIndices[nObject] = nIndex
    //

    //
    #define COMPONENT_DEFAULT_PRE_REATTACH  \
        uint nOldIndex = m_pComponentIndices[nObject]; \
        uint nIndex = AtomicIncrement( &m_nNumActiveComponents ) - 1
    //

    //
    #define COMPONENT_DEFAULT_POST_REATTACH  \
        m_pObjectIndices[nIndex] = nObject; \
        m_pObjectIndices[nOldIndex] = m_pObjectIndices[ m_nNumInactiveComponents ]; \
        AtomicIncrement( &m_nNumInactiveComponents );   \
        m_pComponentIndices[nObject] = nIndex
    //

    //
    #define COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE   \
        uint nIndex = m_pComponentIndices[nObject]
    //
    
    //
    #define COMPONENT_DEFAULT_POST_REMOVE_INACTIVE  \
        m_pObjectIndices[ nIndex ] = m_pObjectIndices[ m_nNumInactiveComponents ]; \
        AtomicIncrement( &m_nNumInactiveComponents )
    //

    //
    #define COMPONENT_DEFAULT_PRE_DETACH_SAVE   \
        AtomicDecrement( &m_nNumActiveComponents );   \
        uint nOldIndex = m_pComponentIndices[nObject]; \
        uint nNewIndex = AtomicDecrement( &m_nNumInactiveComponents )
    //
    
    //
    #define COMPONENT_DEFAULT_POST_DETACH_SAVE  \
        m_pObjectIndices[nNewIndex] = nObject;  \
        m_pObjectIndices[nOldIndex] = m_pObjectIndices[ m_nNumActiveComponents ]; \
        m_pComponentIndices[m_pObjectIndices[ m_nNumActiveComponents ]] = nOldIndex; \
        m_pComponentIndices[nObject] = nNewIndex
    //
    
    //
    #define COMPONENT_DEFAULT_PRE_DETACH \
        uint nIndex = m_pComponentIndices[nObject]; \
        AtomicDecrement( &m_nNumActiveComponents )
    //

    //
    #define COMPONENT_DEFAULT_POST_DETACH
    //

    /*****************************************************************************\
    \*****************************************************************************/


} // namespace Riot

#endif // #ifndef _ICOMPONENT_H_
