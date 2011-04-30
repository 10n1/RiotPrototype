/*********************************************************\
File:           ComponentCharacter.h
Purpose:        Component for the players
Author:         Kyle Weicht
Created:        4/30/2011
Modified:       4/30/2011 12:31:55 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTCHARACTER_H_
#define _COMPONENTCHARACTER_H_
#include "IComponent.h"
#include "View.h"

/*
CComponentCharacter
MAX_CHARACTERS
2
CView* m_pView
RVector3 m_vVelocity
*/

namespace Riot
{
    class CView;

    class CComponentCharacter : public IComponent
    {
        friend class CObjectManager;
    public:
        // CComponentCharacter constructor
        CComponentCharacter();

        // CComponentCharacter destructor
        ~CComponentCharacter();
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

        static const eComponentType ComponentType = eComponentCharacter;
        static const uint MaxComponents = MAX_CHARACTERS;
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesReceived;

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentCharacter* m_pInstance;

        CView       m_pView[MaxComponents];
        RVector3    m_vVelocity[MaxComponents];

    };

} // namespace Riot

#endif // #ifndef _COMPONENTCHARACTER_H_
