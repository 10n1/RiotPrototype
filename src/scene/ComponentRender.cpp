/*********************************************************\
File:           ComponentRender.cpp
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/26/2011 3:18:25 PM
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
        PreAttach( nObject );
        /********************************/

        // Now initialize this component
        m_Transform[m_nIndex]  = RTransform();
        m_pMesh[m_nIndex]  = NULL;

        /********************************/
        PostAttach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CComponentRender::Reattach( uint nObject )
    {
        PreReattach( nObject );
        /********************************/

        // Perform any custom reattchment
        // Now reorder the data
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_USE_PREV_DATA( m_pMesh );

        /********************************/
        PostReattach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void CComponentRender::Detach( uint nObject )
    {
        PreDetach( nObject );
        /********************************/

        // Perform any custom detachment stuff
        SAFE_RELEASE( m_pMesh[m_nIndex] );

        // Now reorder the data
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_pMesh );

        /********************************/
        PostDetach( nObject );
    }

    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CComponentRender::DetachAndSave( uint nObject )
    {
        PreDetachAndSave( nObject );
        /********************************/

        // Perform any custom detachment stuff
        // Now reorder the data
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_REORDER_SAVE_DATA( m_pMesh );

        /********************************/
        PostDetachAndSave( nObject );
    }

    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CComponentRender::RemoveInactive( uint nObject )
    {
        PreRemoveInactive( nObject );
        /********************************/

        // Perform any custom removal stuff
        SAFE_RELEASE( m_pMesh[m_nIndex] );

        // Now reorder the data
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_REMOVE_PREV_DATA( m_pMesh );

        /********************************/
        PostRemoveInactive( nObject );
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