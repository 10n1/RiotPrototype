/*********************************************************\
File:           ComponentLight.h
Purpose:        Turns an object into a light!
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/19/2011 8:30:51 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTLIGHT_H_
#define _COMPONENTLIGHT_H_
#include "IComponent.h"

/*
CComponentLight
MAX_LIGHTS
2
RTransform m_Transform
bool m_bUpdated
*/

namespace Riot
{

    class CComponentLight : public IComponent
    {
        friend class CObjectManager;
    public:
        // CComponentLight constructor
        CComponentLight();

        // CComponentLight destructor
        ~CComponentLight();
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

        static const eComponentType ComponentType = eComponentLight;
        static const uint MaxComponents = MAX_LIGHTS;
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesReceived;

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentLight* m_pInstance;

        RTransform  m_Transform[MaxComponents];

    };

} // namespace Riot

#endif // #ifndef _COMPONENTLIGHT_H_
