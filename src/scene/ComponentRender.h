/*********************************************************\
File:           ComponentRender.h
Purpose:        Component used for rendering all basic objects
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/19/2011 11:08:24 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTRENDER_H_
#define _COMPONENTRENDER_H_
#include "IComponent.h"

/*
CComponentRender
0
2
RTransform m_Transform
CMesh* m_pMesh
*/

namespace Riot
{
    class CMesh;

    class CComponentRender : public IComponent
    {
        friend class CObjectManager;
    public:
        // CComponentRender constructor
        CComponentRender();

        // CComponentRender destructor
        ~CComponentRender();
        /***************************************\
        | class methods                         |
        \***************************************/

        void Attach( uint nObject );
        void Reattach( uint nObject );
        void Detach( uint nObject );
        void DetachAndSave( uint nObject );
        void ProcessComponent( void );
        void ReceiveMessage( uint nSlot, CComponentMessage& msg );
        void RemoveInactive( uint nObject );

        static const eComponentType ComponentType = eComponentRender;
        static const uint MaxComponents = IComponent::MaxComponents;
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesReceived;

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentRender* m_pInstance;

        RTransform  m_Transform[MaxComponents];
        sint        m_pMesh[MaxComponents];

    };

} // namespace Riot

#endif // #ifndef _COMPONENTRENDER_H_
