/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/22/2011 8:36:10 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "Renderer.h"
#include "Engine.h"
#include "ComponentManager.h"
#include "Mesh.h"
#include "TaskManager.h"

namespace Riot
{

    /*****************************************************************************\
    \*****************************************************************************/
    //
    #define BEGIN_DEFINE_COMPONENT( Component )  \
    Component::Component()            \
    {                                                   \
        m_nMaxComponents = MaxComponents;               \
        m_pObjects = new uint[MaxComponents];           \
        Memset( m_pObjects, 0, sizeof(uint) * MaxComponents ); \
    }                                                   \
    Component::~Component() { }       \
    const eComponentMessageType Component::MessagesReceived[] =    \
    {
    //

    //
    #define END_DEFINE_COMPONENT( Component ) \
    };                                            \
    const uint Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
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
    BEGIN_DEFINE_COMPONENT( CRenderComponent )
        eComponentMessageUpdate, 
        eComponentMessageTransform,
        eComponentMessageMesh
    END_DEFINE_COMPONENT( CRenderComponent );
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
        for( sint i = 0; i < m_nNumComponents; ++i )
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
    BEGIN_DEFINE_COMPONENT( CLightComponent )
        eComponentMessageTransform
    END_DEFINE_COMPONENT( CLightComponent );
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

        for( sint i = 0; i < m_nNumComponents; ++i )
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
    //  Collidable component
    //  Makes an object behave with standard Collidableian physics
    //-----------------------------------------------------------------------------
    BEGIN_DEFINE_COMPONENT( CCollidableComponent )
        eComponentMessageTransform,
        eComponentMessageBoundingVolumeType,
        eComponentMessageCollision,
        eComponentMessageCalculateCollidable,
    END_DEFINE_COMPONENT( CCollidableComponent );
    
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CCollidableComponent::Attach( uint nIndex )
    {
        // Now initialize this component
        Memset( &m_Volume[nIndex], 0, sizeof(BoundingVolume) );
        m_nType[nIndex] = BoundingSphere;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CCollidableComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_Volume );
        COMPONENT_REORDER_DATA( m_nType );
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CCollidableComponent::ProcessComponent( void )
    {
        CComponentManager* pManager = Engine::GetComponentManager();
        CTaskManager*      pTaskManager = CTaskManager::GetInstance();

#if PARALLEL_UPDATE
        task_handle_t   nHandle = pTaskManager->PushTask( ProcessBatch, this, m_nNumComponents, 1 );
        pTaskManager->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumComponents );
#endif
    }
    void CCollidableComponent::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CCollidableComponent* pComponent = (CCollidableComponent*)pData;
        CComponentManager*    pManager = Engine::GetComponentManager();

        uint nEnd = nStart + nCount;
        
        for( sint i = nStart; i < nEnd; ++i )
        {
            for( sint j = 0; j < pComponent->m_nNumComponents; ++j )
            {
                if( i == j ) continue;

                float fRadSq = pComponent->m_Volume[i].sphere.radius + pComponent->m_Volume[j].sphere.radius;

                RVector3 pos1( pComponent->m_Volume[i].sphere.position );
                RVector3 pos2( pComponent->m_Volume[j].sphere.position );

                RVector3 diff = pos2-pos1;

                float fDistance = Abs( MagnitudeSq( diff ) );

                if( fDistance <= fRadSq )
                {
                    pManager->PostMessage( eComponentMessageCollision, pComponent->m_pObjects[ i ], j, pComponent->ComponentType );
                }
            }
        }
    }


    void CCollidableComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {        
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                switch( m_nType[nSlot] )
                {
                case BoundingSphere:
                    {
                        m_Volume[nSlot].sphere.position[0] = transform.position[0];
                        m_Volume[nSlot].sphere.position[1] = transform.position[1];
                        m_Volume[nSlot].sphere.position[2] = transform.position[2];
                    }
                    break;
                case AABB:
                    {
                    }
                    break;
                default:
                    {
                    }
                };
            }
            break;
        case eComponentMessageCalculateCollidable:
            {
                MeshData* pData = (MeshData*)msg.m_pData;


                float fExtents[3] = {  0 };

                for( uint i = 0; i < pData->nVerts; ++i )
                {
                    for( uint j = 0; j < 3; ++j )
                    {
                        if( Abs(pData->pVerts[i].Pos[j]) > fExtents[j] )
                        {
                            fExtents[j] = Abs(pData->pVerts[i].Pos[j]);
                        }
                    }
                }

                m_Volume[nSlot].sphere.radius = MagnitudeSq( RVector3(fExtents) );
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
    BEGIN_DEFINE_COMPONENT( CNewtonPhysicsComponent )
        eComponentMessageTransform,
        eComponentMessageCollision,
    END_DEFINE_COMPONENT( CNewtonPhysicsComponent );
    
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

        CComponentManager* pManager = Engine::GetComponentManager();

        for( sint i = 0; i < m_nNumComponents; ++i )
        {
            if( m_bGravity[i] )
            {
                m_vVelocity[i] += vGravity * Engine::m_fElapsedTime;
            }
            else
            {
                m_vVelocity[i] = RVector3( 0.0f, 0.0f, 0.0f );
            }

            m_Transform[i].position += m_vVelocity[i] * Engine::m_fElapsedTime;

            pManager->PostMessage( eComponentMessageTransform, m_pObjects[ i ], &m_Transform[i], ComponentType );
        }
    }


    void CNewtonPhysicsComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {        
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                m_Transform[nSlot] = transform;
            }
            break;
        case eComponentMessageCollision:
            {
                m_bGravity[nSlot] = false;
            }
            break;
        default:
            {
            }
        }
    }

} // namespace Riot
