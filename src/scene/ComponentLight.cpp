/*********************************************************\
File:           ComponentLight.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 9:43:54 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentLight.h"
#include "Engine.h"
#include "Renderer.h"
#include "ObjectManager.h"

/*
CComponentLight
1
eComponentMessageTransform
2    
RTransform m_Transform
bool m_bUpdated
*/

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    CComponentLight* CComponentLight::m_pInstance = NULL;

    const eComponentMessageType CComponentLight::MessagesReceived[] =
    {
        eComponentMessageTransform,
    };
    const uint CComponentLight::NumMessagesReceived = (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType );

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentLight::CComponentLight()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;
    }

    CComponentLight::~CComponentLight() { } 

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponentLight::Attach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_ATTACH;
        /********************************/

        // Now initialize this component
        m_Transform[nIndex] = RTransform();
        m_bUpdated[nIndex]  = true;

        /********************************/
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentLight::Reattach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;
        /********************************/

        // Perform any custom reattchment

        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_USE_PREV_DATA( m_bUpdated );

        /********************************/
        COMPONENT_DEFAULT_POST_REATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentLight::Detach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH;
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_bUpdated );

        /********************************/
        COMPONENT_DEFAULT_POST_DETACH;
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentLight::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_REORDER_SAVE_DATA( m_bUpdated );

        /********************************/
        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentLight::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;
        /********************************/

        // Perform any custom removal stuff

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_REMOVE_PREV_DATA( m_bUpdated );

        /********************************/
        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CComponentLight::ProcessComponent( void )
    {
        static float fAngle = 0.0f;
        fAngle += Engine::m_fElapsedTime;

        CRenderer* pRender = Engine::GetRenderer();
        CObjectManager* pManager = Engine::GetObjectManager();

        for( sint i = 0; i < m_nNumActiveComponents; ++i )
        {
            m_Transform[i].position.x = sin( fAngle ) * 10.0f;
            m_Transform[i].position.z = cos( fAngle ) * 10.0f;
            m_bUpdated[i] = true;

            if( m_bUpdated[i] == true )
            {
                pRender->SetLight( m_Transform[i].position, i );
                m_bUpdated[i] = false;
                pManager->PostMessage( eComponentMessageTransform, m_pObjectIndices[ i ], &m_Transform[i], ComponentType );
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponentLight::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                m_Transform[nSlot]      = transform;
                m_bUpdated[nSlot]       = true;
            }
            break;
        default:
            {
            }
        }
    }

} // namespace Riot
