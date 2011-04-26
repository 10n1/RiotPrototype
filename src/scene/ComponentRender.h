/*********************************************************\
File:           ComponentRender.h
Purpose:        
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 5:42:50 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTRENDER_H_
#define _COMPONENTRENDER_H_
#include "IComponent.h"

namespace Riot
{

    class CComponentRender : public IComponent
    {
    public:
        // CComponentRender constructor
        CComponentRender();

        // CComponentRender destructor
        ~CComponentRender();
        /***************************************\
        | class methods                         |
        \***************************************/

        void Attach( uint nIndex );
        void Reattach( uint nIndex, uint nPrevIndex );
        void Detach( uint nIndex );
        void DetachAndSave( uint nOldIndex, uint nNewIndex );
        void ProcessComponent( void );
        void ReceiveMessage( uint nSlot, CComponentMessage& msg );
        void RemoveInactive( uint nIndex );

        static const eComponentType ComponentType = eComponentRender;
        static const uint MaxComponents = MAX_OBJECTS;
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesReceived;

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        RTransform              m_Transform[MaxComponents];
        RTransformsdafsdfioahsdf    m_Transform[MaxComponents];
        bool                    m_bUpdate[MaxComponents];

    };

} // namespace Riot

#endif // #ifndef _COMPONENTRENDER_H_
