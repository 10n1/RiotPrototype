/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/17/2011 5:22:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "Renderer.h"
#include "Engine.h"
#include "Object.h"
#include "ComponentManager.h"
#include "Mesh.h"

namespace Riot
{

    /*****************************************************************************\
    \*****************************************************************************/
    //
    #define BEGIN_DEFINE_COMPONENT( Name )  \
    C##Name##Component::C##Name##Component()            \
    {                                                   \
        m_nMaxComponents = MaxComponents;               \
        m_ppObjects = new CObject*[MaxComponents];      \
        Memset( m_ppObjects, 0, sizeof(CObject*) * MaxComponents ); \
    }                                                   \
    C##Name##Component::~C##Name##Component() { }       \
    const eComponentMessageType C##Name##Component::MessagesReceived[] =    \
    {
    //

    //
    #define END_DEFINE_COMPONENT( Name ) \
    };                                            \
    const uint C##Name##Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
    //

    //
    #define COMPONENT_REORDER_DATA( Data ) Data[nIndex] = Data[ m_nNumComponents - 1  ]
    //

    /*****************************************************************************\
    \*****************************************************************************/

    // CComponent constructor
    CComponent::CComponent()
        : m_nNumComponents( 0 )
        , m_nMaxComponents( 0 )
    {
    }

    // CComponent destructor
    CComponent::~CComponent()
    {
        SAFE_DELETE_ARRAY( m_ppObjects );
    }

    //-----------------------------------------------------------------------------
    const eComponentMessageType CComponent::MessagesSent[] = 
    {
        eNUMCOMPONENTMESSAGES, // can't make a zero-length array
    };
    const eComponentMessageType CComponent::MessagesReceived[] = 
    {
        eNUMCOMPONENTMESSAGES, // can't make a zero-length array
    };
    const uint CComponent::NumMessagesSent       = 0;
    const uint CComponent::NumMessagesReceived   = 0;
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CComponent::ProcessComponent( void )
    {
    }

    //-----------------------------------------------------------------------------
    //  AddComponent
    //  "Adds" a component to an object
    //-----------------------------------------------------------------------------
    sint CComponent::AddComponent( CObject* pObject )
    {
        if( m_nNumComponents >= m_nMaxComponents )
        {
            // TODO: Handle error more gracefully
            return -1;
        }

        // Calculate the free spot for this component
        uint nIndex = m_nNumComponents++;
        m_ppObjects[ nIndex ] = pObject;

        // ...then attach to it
        Attach( nIndex );

        return nIndex;
    }

    //-----------------------------------------------------------------------------
    //  RemoveComponent
    //  "Removes" a component to an object
    //-----------------------------------------------------------------------------
    void CComponent::RemoveComponent( uint nIndex )
    {
        // First detach...
        Detach( nIndex );

        // ...then clean up the list
        m_ppObjects[ nIndex ] = m_ppObjects[ --m_nNumComponents ];
    }

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponent::Attach( uint nIndex ) { }
    
    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CComponent::Detach( uint nIndex ) { }

    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg ) { }

    /*********************************************************************************\
    |*********************************************************************************|
    |*********************************************************************************|
    |*********************************************************************************|
    \*********************************************************************************/

    //-----------------------------------------------------------------------------
    BEGIN_DEFINE_COMPONENT( Render )
        eComponentMessageUpdate, 
        eComponentMessageTransform,
        eComponentMessageMesh
    END_DEFINE_COMPONENT( Render );
    //-----------------------------------------------------------------------------


    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CRenderComponent::Attach( uint nIndex )
    {
        // Now initialize this component
        m_pMesh[nIndex]     = NULL;
        m_Transform[nIndex] = RTransform();
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CRenderComponent::Detach( uint nIndex )
    {
        SAFE_RELEASE( m_pMesh[nIndex] );

        // Now initialize this component
        COMPONENT_REORDER_DATA( m_pMesh );
        COMPONENT_REORDER_DATA( m_Transform );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CRenderComponent::ProcessComponent( void )
    {
        CRenderer* pRender = Engine::GetRenderer();
        for( uint i = 0; i < m_nNumComponents; ++i )
        {          
            // Pass to the render engine
            pRender->AddCommand( m_pMesh[i], m_Transform[i] );
        }
    }


    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CRenderComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
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



    /*********************************************************************************\
    |*********************************************************************************|
    |*********************************************************************************|
    |*********************************************************************************|
    \*********************************************************************************/

    //-----------------------------------------------------------------------------
    BEGIN_DEFINE_COMPONENT( Update )
        eComponentMessageTransform,
    END_DEFINE_COMPONENT( Update );
    //-----------------------------------------------------------------------------


    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CUpdateComponent::Attach( uint nIndex )
    {
        // Now initialize this component
        m_Transform[nIndex] = RTransform();
    }


    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CUpdateComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_Transform );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CUpdateComponent::ProcessComponent( void )
    {
        CComponentManager* pManager = Engine::GetComponentManager();
        for( uint i = 0; i < m_nNumComponents; ++i )
        {
            m_Transform[ i ].TranslateLocalY( Engine::m_fElapsedTime * 0.05f );

            pManager->PostMessage( eComponentMessageTransform, m_ppObjects[ i ], &m_Transform[i], ComponentType );
        }
    }


    //-----------------------------------------------------------------------------
    //  ReceiveMessage

    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CUpdateComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                m_Transform[nSlot] = transform;
            }
            break;

        default:
            {
            }
        }
    }


    /*********************************************************************************\
    |*********************************************************************************|
    |*********************************************************************************|
    |*********************************************************************************|
    \*********************************************************************************/

    //-----------------------------------------------------------------------------
    BEGIN_DEFINE_COMPONENT( Light )
        eComponentMessageTransform
    END_DEFINE_COMPONENT( Light );
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CLightComponent::Attach( uint nIndex )
    {
        // Now initialize this component
        CObject* pObject = m_ppObjects[ nIndex ];
        m_Transform[nIndex] = RTransform();
        m_bUpdated[nIndex] = true;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CLightComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_bUpdated );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CLightComponent::ProcessComponent( void )
    {
        static float fAngle = 0.0f;
        fAngle += Engine::m_fElapsedTime;

        CRenderer* pRender = Engine::GetRenderer();
        CComponentManager* pManager = Engine::GetComponentManager();

        for( uint i = 0; i < m_nNumComponents; ++i )
        {
            m_Transform[i].position.x = sin( fAngle ) * 10.0f;
            m_Transform[i].position.z = cos( fAngle ) * 10.0f;
            m_bUpdated[i] = true;

            if( m_bUpdated[i] == true )
            {
                pRender->SetLight( Homogonize(m_Transform[i].position), i );
                m_bUpdated[i] = false;
                pManager->PostMessage( eComponentMessageTransform, m_ppObjects[ i ], &m_Transform[i], ComponentType );
            }
        }
    }


    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    void CLightComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {
        //case MessagesReceived[0]: <-- // TODO: is there a way to do that?
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
