/*********************************************************\
File:           IComponent.h
Purpose:        The definition for the base component
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 7:15:58 PM
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

} // namespace Riot

#endif // #ifndef _ICOMPONENT_H_
