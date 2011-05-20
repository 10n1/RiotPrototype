/*********************************************************\
File:           ComponentCharacter.cpp
Author:         Kyle Weicht
Created:        4/30/2011
Modified:       4/30/2011 12:38:14 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentCharacter.h"
#include "Engine.h"
#include "ObjectManager.h"

/*
CComponentCharacter
2
eComponentMessageTerrainCollision
eComponentMessageTransform
1
CView* m_pView
*/

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    CComponentCharacter* CComponentCharacter::m_pInstance = NULL;

    const eComponentMessageType CComponentCharacter::MessagesReceived[] =
    {
        eComponentMessageTransform,
        eComponentMessageTerrainCollision,
    };
    const uint CComponentCharacter::NumMessagesReceived = (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType );

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentCharacter::CComponentCharacter()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;
    }

    CComponentCharacter::~CComponentCharacter() { } 

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponentCharacter::Attach( uint nObject )
    {
        PreAttach( nObject );
        /********************************/

        // Now initialize this component
        m_pView[m_nIndex]       = CView();
        m_vVelocity[m_nIndex]   = RVector3Zero();

        /********************************/
        PostAttach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentCharacter::Reattach( uint nObject )
    {
        PreReattach( nObject );
        /********************************/

        // Perform any custom reattchment

        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_pView );

        /********************************/
        PostReattach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentCharacter::Detach( uint nObject )
    {
        PreDetach( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_pView );

        /********************************/
        PostDetach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentCharacter::DetachAndSave( uint nObject )
    {
        PreDetachAndSave( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_pView );

        /********************************/
        PostDetachAndSave( nObject );
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentCharacter::RemoveInactive( uint nObject )
    {
        PreRemoveInactive( nObject );
        /********************************/

        // Perform any custom removal stuff

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_pView );

        /********************************/
        PostRemoveInactive( nObject );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CComponentCharacter::ProcessComponent( void )
    {
        for( sint i = 0; i < m_nNumActiveComponents; ++i )
        {
            // Update this shit!
        }
    }

    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponentCharacter::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        case eComponentMessageTerrainCollision:
            {
            }
            break;
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                //m_pView[nSlot].m_vp
            }
            break;
        default:
            {
            }
        }
    }

    } // namespace Riot
