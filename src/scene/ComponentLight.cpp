/*********************************************************\
File:           ComponentLight.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/20/2011 7:52:03 AM
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
        PreAttach( nObject );
        /********************************/

        // Now initialize this component
        m_Transform[m_nIndex] = RTransform();
        m_nType[m_nIndex] = eLightPoint;
        m_fRange[m_nIndex] = 10.0f;

        /********************************/
        PostAttach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentLight::Reattach( uint nObject )
    {
        PreReattach( nObject );
        /********************************/

        // Perform any custom reattchment

        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_USE_PREV_DATA( m_nType );
        COMPONENT_USE_PREV_DATA( m_fRange );

        /********************************/
        PostReattach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentLight::Detach( uint nObject )
    {
        PreDetach( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_nType );
        COMPONENT_REORDER_DATA( m_fRange );

        /********************************/
        PostDetach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentLight::DetachAndSave( uint nObject )
    {
        PreDetachAndSave( nObject );
        /********************************/

        // Perform any custom detachment stuff

        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_REORDER_SAVE_DATA( m_nType );
        COMPONENT_REORDER_SAVE_DATA( m_fRange );

        /********************************/
        PostDetachAndSave( nObject );
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentLight::RemoveInactive( uint nObject )
    {
        PreRemoveInactive( nObject );
        /********************************/

        // Perform any custom removal stuff

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_REMOVE_PREV_DATA( m_nType );
        COMPONENT_REMOVE_PREV_DATA( m_fRange );

        /********************************/
        PostRemoveInactive( nObject );
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

        for( uint i = 0; i < m_nNumActiveComponents; ++i )
        {
            switch( m_nType[i] )
            {
            case eLightDir:
                pRender->AddDirLight( m_Transform[i].position );
                break;
            case eLightPoint:
                pRender->AddPointLight( m_Transform[i].position, m_fRange[i] );
                break;
            default:
                ASSERT( 0 );
                false;
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
            }
            break;
        default:
            {
            }
        }
    }

} // namespace Riot
