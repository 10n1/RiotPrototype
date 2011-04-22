/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/20/2011 9:23:12 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "Renderer.h"
#include "Engine.h"
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
        m_pObjects = new uint[MaxComponents];           \
        Memset( m_pObjects, 0, sizeof(uint) * MaxComponents ); \
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
    #define COMPONENT_REORDER_DATA( Data ) Data[nIndex] = Data[ m_nNumComponents ]
    //

    /*****************************************************************************\
    \*****************************************************************************/

    // CComponent constructor
    CComponent::CComponent()
        : m_nNumComponents( 0 )
        , m_nMaxComponents( 0 )
        , m_pObjects( NULL )
    {
    }

    // CComponent destructor
    CComponent::~CComponent()
    {
        SAFE_DELETE_ARRAY( m_pObjects );
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
    sint CComponent::AddComponent( uint nObject )
    {
        if( m_nNumComponents >= m_nMaxComponents )
        {
            // TODO: Handle error more gracefully
            return -1;
        }

        // Calculate the free spot for this component
        uint nIndex = AtomicIncrement( &m_nNumComponents ) - 1;
        m_pObjects[ nIndex ] = nObject;

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
        // Decremnt the counter first
        AtomicDecrement( &m_nNumComponents );

        // First detach...
        Detach( nIndex );

        // ...then clean up the list
        m_pObjects[ nIndex ] = m_pObjects[ m_nNumComponents ];
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
            TRenderCommand cmd = { m_pMesh[i], NULL };
            pRender->AddCommand( cmd, m_Transform[i] );
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
        //CComponentManager* pManager = Engine::GetComponentManager();
        //for( uint i = 0; i < m_nNumComponents; ++i )
        //{
        //    m_Transform[ i ].TranslateLocalY( Engine::m_fElapsedTime * 0.05f );
        //
        //    pManager->PostMessage( eComponentMessageTransform, m_pObjects[ i ], &m_Transform[i], ComponentType );
        //}
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
                pRender->SetLight( m_Transform[i].position, i );
                m_bUpdated[i] = false;
                pManager->PostMessage( eComponentMessageTransform, m_pObjects[ i ], &m_Transform[i], ComponentType );
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

    
    /*********************************************************************************\
    |*********************************************************************************|
    |*********************************************************************************|
    |*********************************************************************************|
    \*********************************************************************************/

    //-----------------------------------------------------------------------------
    //  NewtonPhysics component
    //  Makes an object behave with standard NewtonPhysicsian physics
    //-----------------------------------------------------------------------------
    BEGIN_DEFINE_COMPONENT( NewtonPhysics )
        eComponentMessageTransform
    END_DEFINE_COMPONENT( NewtonPhysics );
    
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::Attach( uint nIndex )
    {
        // Now initialize this component
        m_Transform[nIndex] = RTransform();
        m_vVelocity[nIndex] = RVector3Zero();
        m_bGravity[nIndex]  = true;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_Transform );
        COMPONENT_REORDER_DATA( m_vVelocity );
        COMPONENT_REORDER_DATA( m_bGravity );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::ProcessComponent( void )
    {
        static const RVector3 vGravity = RVector3( 0.0f, -9.8f, 0.0f );

        CRenderer* pRender = Engine::GetRenderer();
        CComponentManager* pManager = Engine::GetComponentManager();

        for( uint i = 0; i < m_nNumComponents; ++i )
        {
            if( m_bGravity[i] )
            {
                m_vVelocity[i] += vGravity * Engine::m_fElapsedTime;
            }

            m_Transform[i].position += m_vVelocity[i] * Engine::m_fElapsedTime;

            pManager->PostMessage( eComponentMessageTransform, m_pObjects[ i ], &m_Transform[i], ComponentType );
        }
    }


    void CNewtonPhysicsComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
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

} // namespace Riot