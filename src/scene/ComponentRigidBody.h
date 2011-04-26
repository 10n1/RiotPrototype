/*********************************************************\
File:           ComponentRigidBody.h
Purpose:        
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 9:55:41 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTRIGIDBODY_H_
#define _COMPONENTRIGIDBODY_H_
#include "IComponent.h"

/*
CComponentRigidBody
0
3
RTransform m_Transform
RVector3 m_vVelocity
bool m_bGravity
*/

namespace Riot
{

    class CComponentRigidBody : public IComponent
    {
        friend class CObjectManager;
    public:
        // CComponentRigidBody constructor
        CComponentRigidBody();

        // CComponentRigidBody destructor
        ~CComponentRigidBody();
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

        static const eComponentType ComponentType = eComponentRigidBody;
        static const uint MaxComponents = IComponent::MaxComponents;
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesReceived;
        
    private:
        static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentRigidBody* m_pInstance;

        RTransform  m_Transform[MaxComponents];
        RVector3    m_vVelocity[MaxComponents];
        bool        m_bGravity[MaxComponents];

    };

} // namespace Riot

#endif // #ifndef _COMPONENTRIGIDBODY_H_
