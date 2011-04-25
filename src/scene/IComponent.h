/*********************************************************\
File:           IComponent.h
Purpose:        The definition for the base component
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 4:12:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _ICOMPONENT_H_
#define _ICOMPONENT_H_
#include "common.h"

namespace Riot
{
    class CComponentMessage;

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
        virtual void Attach( uint nIndex ) = 0;

        //-----------------------------------------------------------------------------
        //  Rettach
        //  Reattaches a component to an object, using it's last data
        //-----------------------------------------------------------------------------
        virtual void Reattach( uint nIndex, uint nOldIndex ) = 0;

        //-----------------------------------------------------------------------------
        //  Detach
        //  Detaches a component from an object, discarding the old data
        //-----------------------------------------------------------------------------
        virtual void Detach( uint nIndex ) = 0;
        
        //-----------------------------------------------------------------------------
        //  DetachAndSave
        //  Detaches a component from an object, saving the old data
        //-----------------------------------------------------------------------------
        virtual void DetachAndSave( uint nOldIndex, uint nNewIndex ) = 0;

        //-----------------------------------------------------------------------------
        //  RemoveInactive
        //  Removes the inactive component
        //-----------------------------------------------------------------------------
        virtual void RemoveInactive( uint nIndex ) = 0;
        
    protected:
        uint*       m_pObjects;
        atomic_t    m_nNumActiveComponents;
        atomic_t    m_nNumInactiveComponents;
    };

} // namespace Riot

#endif // #ifndef _ICOMPONENT_H_
