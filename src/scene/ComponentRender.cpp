/*********************************************************\
File:           ComponentRender.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 9:37:03 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "ComponentRender.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"

/*
CComponentRender
3
eComponentMessageUpdate
eComponentMessageTransform
eComponentMessageMesh
2
RTransform m_Transform
CMesh* m_pMesh
*/

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/
    CComponentRender* CComponentRender::m_pInstance = NULL;

    const eComponentMessageType CComponentRender::MessagesReceived[] =
    {
        eComponentMessageUpdate,
        eComponentMessageTransform,
        eComponentMessageMesh,
    };
    const uint CComponentRender::NumMessagesReceived = (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType );

    /***************************************\
    | class methods                         |
    \***************************************/
    CComponentRender::CComponentRender()
    {
        m_nNumInactiveComponents = MaxComponents;
        m_nNumActiveComponents = 0;
    }

    CComponentRender::~CComponentRender() { } 

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponentRender::Attach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_ATTACH;
        /********************************/

        // Now initialize this component
        m_Transform[nIndex]  = RTransform();
        m_pMesh[nIndex]  = NULL;

        /********************************/
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentRender::Reattach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;
        /********************************/

        // Perform any custom reattchment
        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_USE_PREV_DATA( m_pMesh );

        /********************************/
        COMPONENT_DEFAULT_POST_REATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentRender::Detach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH;
        /********************************/

        // Perform any custom detachment stuff
        SAFE_RELEASE( m_pMesh[nIndex] );

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_pMesh );

        /********************************/
        COMPONENT_DEFAULT_POST_DETACH;
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentRender::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        /********************************/

        // Perform any custom detachment stuff
        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_REORDER_SAVE_DATA( m_pMesh );

        /********************************/
        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentRender::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;
        /********************************/

        // Perform any custom removal stuff
        SAFE_RELEASE( m_pMesh[nIndex] );

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_REMOVE_PREV_DATA( m_pMesh );

        /********************************/
        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CComponentRender::ProcessComponent( void )
    {
        CRenderer* pRender = Engine::GetRenderer();
        for( sint i = 0; i < m_nNumActiveComponents; ++i )
        {
            // Update this shit!
            // Pass to the render engine
            TRenderCommand cmd = { m_pMesh[i], NULL };
            pRender->AddCommand( cmd, m_Transform[i] );
        }
    }

    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponentRender::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        case eComponentMessageUpdate:
            {
            }
            break;
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                m_Transform[nSlot] = transform;
            }
            break;
        case eComponentMessageMesh:
            {
                m_pMesh[nSlot] = (CMesh*)msg.m_pData;
                m_pMesh[nSlot]->AddRef();
            }
            break;
        default:
            {
            }
        }
    }

} // namespace Riot
