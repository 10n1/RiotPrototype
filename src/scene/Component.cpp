/*********************************************************\
File:           Component.cpp
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/25/2011 7:10:07 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Component.h"
#include "Renderer.h"
#include "Engine.h"
#include "Mesh.h"
#include "TaskManager.h"
#include "ObjectManager.h"

namespace Riot
{

    /*****************************************************************************\
    \*****************************************************************************/
    //
    #define BEGIN_DEFINE_COMPONENT( Component )         \
        Component* Component::m_pInstance = NULL;       \
        Component::Component()                          \
        {                                               \
            m_nNumInactiveComponents = MaxComponents;   \
            m_nNumActiveComponents = 0;                 \
        }                                               \
        Component::~Component() { }                     \
        const eComponentMessageType Component::MessagesReceived[] =    \
        {
    //

    //
    #define END_DEFINE_COMPONENT( Component )   \
        };                                      \
        const uint Component::NumMessagesReceived   =  (MessagesReceived[0] == eNULLCOMPONENTMESSAGE) ? 0 : sizeof( MessagesReceived ) / sizeof( eComponentMessageType )
    //

    //
    #define COMPONENT_REORDER_DATA( Data )  \
        Data[nIndex] = Data[ m_nNumActiveComponents ]

    //
    
    //
    #define COMPONENT_REORDER_SAVE_DATA( Data ) \
        Data[nNewIndex] = Data[nOldIndex];       \
        Data[nOldIndex] = Data[ m_nNumActiveComponents ]

    //

    //
    #define COMPONENT_USE_PREV_DATA( Data ) \
        Data[nIndex] = Data[nOldIndex];     \
        Data[nOldIndex] = Data[m_nNumInactiveComponents] 
    //
    
    //
    #define COMPONENT_REMOVE_PREV_DATA( Data ) \
        Data[nIndex] = Data[m_nNumInactiveComponents]
    //

    //
    #define COMPONENT_DEFAULT_PRE_ATTACH    \
        uint nIndex = AtomicIncrement( &m_nNumActiveComponents ) - 1
    //
    
    //
    #define COMPONENT_DEFAULT_POST_ATTACH   \
        m_pObjectIndices[nIndex] = nObject; \
        m_pComponentIndices[nObject] = nIndex
    //

    //
    #define COMPONENT_DEFAULT_PRE_REATTACH  \
        uint nOldIndex = m_pComponentIndices[nObject]; \
        uint nIndex = AtomicIncrement( &m_nNumActiveComponents ) - 1
    //

    //
    #define COMPONENT_DEFAULT_POST_REATTACH  \
        m_pObjectIndices[nIndex] = nObject; \
        m_pObjectIndices[nOldIndex] = m_pObjectIndices[ m_nNumInactiveComponents ]; \
        AtomicIncrement( &m_nNumInactiveComponents );   \
        m_pComponentIndices[nObject] = nIndex
    //

    //
#define COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE   \
    uint nIndex = m_pComponentIndices[nObject]
    //
    
    //
#define COMPONENT_DEFAULT_POST_REMOVE_INACTIVE  \
    m_pObjectIndices[ nIndex ] = m_pObjectIndices[ m_nNumInactiveComponents ]; \
    AtomicIncrement( &m_nNumInactiveComponents )
    //

    //
#define COMPONENT_DEFAULT_PRE_DETACH_SAVE   \
    uint nOldIndex = m_pComponentIndices[nObject]; \
    uint nNewIndex = AtomicDecrement( &m_nNumInactiveComponents )
    //
    
    //
#define COMPONENT_DEFAULT_POST_DETACH_SAVE  \
    m_pObjectIndices[nNewIndex] = nObject;  \
    m_pObjectIndices[nOldIndex] = m_pObjectIndices[ m_nNumActiveComponents ]; \
    m_pComponentIndices[m_pObjectIndices[ m_nNumActiveComponents ]] = nOldIndex; \
    m_pComponentIndices[nObject] = nNewIndex
    //

    /*****************************************************************************\
    \*****************************************************************************/


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
    void CRenderComponent::Attach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_ATTACH;
        // Now initialize this component
        m_pMesh[nIndex]     = NULL;
        m_Transform[nIndex] = RTransform();
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CRenderComponent::Reattach( uint nObject  )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;

        // Now initialize this component
        COMPONENT_USE_PREV_DATA( m_pMesh );
        COMPONENT_USE_PREV_DATA( m_Transform );

        COMPONENT_DEFAULT_POST_REATTACH;
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
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CRenderComponent::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        // Now initialize this component
        COMPONENT_REORDER_SAVE_DATA( m_pMesh );
        COMPONENT_REORDER_SAVE_DATA( m_Transform );

        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }
    
    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CRenderComponent::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;
        SAFE_RELEASE( m_pMesh[nIndex] );
        COMPONENT_REMOVE_PREV_DATA( m_pMesh );
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CRenderComponent::ProcessComponent( void )
    {
        CRenderer* pRender = Engine::GetRenderer();
        for( sint i = 0; i < m_nNumActiveComponents; ++i )
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
    void CLightComponent::Attach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_ATTACH;
        // Now initialize this component
        m_Transform[nIndex] = RTransform();
        m_bUpdated[nIndex] = true;
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CLightComponent::Reattach( uint nObject  )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;
        // Now initialize this component
        COMPONENT_USE_PREV_DATA( m_bUpdated );
        COMPONENT_USE_PREV_DATA( m_Transform );
        COMPONENT_DEFAULT_POST_REATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CLightComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_bUpdated );
        COMPONENT_REORDER_DATA( m_Transform );
    }
    
    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CLightComponent::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        // Now initialize this component
        COMPONENT_REORDER_SAVE_DATA( m_bUpdated );
        COMPONENT_REORDER_SAVE_DATA( m_Transform );
        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }
    
    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CLightComponent::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;
        COMPONENT_REMOVE_PREV_DATA( m_bUpdated );
        COMPONENT_REMOVE_PREV_DATA( m_Transform );
        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
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
    END_DEFINE_COMPONENT( CCollidableComponent );
    //-----------------------------------------------------------------------------
          
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CCollidableComponent::Attach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_ATTACH;
        // Now initialize this component
        Memset( &m_Volume[nIndex], 0, sizeof(BoundingVolume) );
        m_nVolumeType[nIndex] = BoundingSphere;
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CCollidableComponent::Reattach( uint nObject  )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;
        // Now initialize this component
        COMPONENT_USE_PREV_DATA( m_nVolumeType );
        COMPONENT_USE_PREV_DATA( m_Volume );
        COMPONENT_DEFAULT_POST_REATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CCollidableComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_nVolumeType );
        COMPONENT_REORDER_DATA( m_Volume );
    }
    
    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CCollidableComponent::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        // Now initialize this component
        COMPONENT_REORDER_SAVE_DATA( m_nVolumeType );
        COMPONENT_REORDER_SAVE_DATA( m_Volume );

        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }
    
    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CCollidableComponent::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;

        COMPONENT_REMOVE_PREV_DATA( m_nVolumeType );
        COMPONENT_REMOVE_PREV_DATA( m_Volume );

        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
    }

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CCollidableComponent::ProcessComponent( void )
    {
        CTaskManager*      pTaskManager = CTaskManager::GetInstance();

#if PARALLEL_UPDATE
        task_handle_t   nHandle = pTaskManager->PushTask( ProcessBatch, this, m_nNumActiveComponents, 4 );
        pTaskManager->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumActiveComponents );
#endif
    }
    void CCollidableComponent::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CCollidableComponent* pComponent = (CCollidableComponent*)pData;
        CObjectManager*    pManager = Engine::GetObjectManager();

        uint nEnd = nStart + nCount;
        
        for( sint i = nStart; i < nEnd; ++i )
        {
            for( sint j = 0; j < pComponent->m_nNumActiveComponents; ++j )
            {
                if( i == j ) continue;

                float fRadSq = pComponent->m_Volume[i].sphere.radius + pComponent->m_Volume[j].sphere.radius;

                RVector3 pos1( pComponent->m_Volume[i].sphere.position );
                RVector3 pos2( pComponent->m_Volume[j].sphere.position );

                RVector3 diff = pos2-pos1;

                float fDistance = Abs( MagnitudeSq( diff ) );

                if( fDistance <= fRadSq )
                {
                    pManager->PostMessage( eComponentMessageCollision, pComponent->m_pObjectIndices[ i ], j, pComponent->ComponentType );
                }
            }
        }
    }

    void CCollidableComponent::CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, uint nIndex )
    {
        ASSERT( m_pInstance );

        float3 fExtents = {  0 };

        for( uint i = 0; i < nNumVerts; ++i )
        {
            for( uint j = 0; j < 3; ++j )
            {
                if( Abs(pVerts[i].Pos[j]) > fExtents[j] )
                {
                    fExtents[j] = Abs(pVerts[i].Pos[j]);
                }
            }
        }

        m_pInstance->m_Volume[nIndex].sphere.radius = MagnitudeSq( RVector3(fExtents) );
    }

    void CCollidableComponent::ReceiveMessage( uint nSlot, CComponentMessage& msg )
    {
        switch( msg.m_nMessageType )
        {        
        case eComponentMessageTransform:
            {
                RTransform& transform = *((RTransform*)msg.m_pData);

                switch( m_nVolumeType[nSlot] )
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
    
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::Attach( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_ATTACH;
        // Now initialize this component
        m_Transform[nIndex] = RTransform();
        m_vVelocity[nIndex] = RVector3Zero();
        m_bGravity[nIndex]  = true;
        COMPONENT_DEFAULT_POST_ATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Rettach
    //  Reattaches a component to an object, using it's last data
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::Reattach( uint nObject  )
    {
        COMPONENT_DEFAULT_PRE_REATTACH;
        // Now initialize this component
        COMPONENT_USE_PREV_DATA( m_bGravity );
        COMPONENT_USE_PREV_DATA( m_vVelocity );
        COMPONENT_USE_PREV_DATA( m_Transform );

        COMPONENT_DEFAULT_POST_REATTACH;
    }

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component to an object
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::Detach( uint nIndex )
    {
        // Now initialize this component
        COMPONENT_REORDER_DATA( m_bGravity );
        COMPONENT_REORDER_DATA( m_vVelocity );
        COMPONENT_REORDER_DATA( m_Transform );
    }
    
    //-----------------------------------------------------------------------------
    //  DetachAndSave
    //  Detaches a component from an object, saving the old data
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::DetachAndSave( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_DETACH_SAVE;
        // Now initialize this component
        COMPONENT_REORDER_SAVE_DATA( m_bGravity );
        COMPONENT_REORDER_SAVE_DATA( m_vVelocity );
        COMPONENT_REORDER_SAVE_DATA( m_Transform );

        COMPONENT_DEFAULT_POST_DETACH_SAVE;
    }
    
    //-----------------------------------------------------------------------------
    //  RemoveInactive
    //  Removes the inactive component
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::RemoveInactive( uint nObject )
    {
        COMPONENT_DEFAULT_PRE_REMOVE_INACTIVE;

        COMPONENT_REMOVE_PREV_DATA( m_bGravity );
        COMPONENT_REMOVE_PREV_DATA( m_vVelocity );
        COMPONENT_REMOVE_PREV_DATA( m_Transform );

        COMPONENT_DEFAULT_POST_REMOVE_INACTIVE;
    }



    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void CNewtonPhysicsComponent::ProcessComponent( void )
    {
#if 0 //PARALLEL_UPDATE
        task_handle_t nHandle = Engine::GetTaskManager()->PushTask( ProcessBatch, this, m_nNumActiveComponents, 16 );
        Engine::GetTaskManager()->WaitForCompletion( nHandle );
#else
        ProcessBatch( this, 0, 0, m_nNumActiveComponents );
#endif
    }

    void CNewtonPhysicsComponent::ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        static const RVector3 vGravity = RVector3( 0.0f, -9.8f, 0.0f );
        CNewtonPhysicsComponent* pComponent = (CNewtonPhysicsComponent*)pData;
        CObjectManager*    pManager = Engine::GetObjectManager();
    
        uint nEnd = nStart + nCount;
        
        for( sint i = nStart; i < nEnd; ++i )
        {
            if( pComponent->m_bGravity[i] )
            {
                pComponent->m_vVelocity[i] += vGravity * Engine::m_fElapsedTime;
            }
            else
            {
                pComponent->m_vVelocity[i] = RVector3( 0.0f, 0.0f, 0.0f );
            }

            pComponent->m_Transform[i].position += pComponent->m_vVelocity[i] * Engine::m_fElapsedTime;

            pManager->PostMessage( eComponentMessageTransform, pComponent->m_pObjectIndices[ i ], &pComponent->m_Transform[i], ComponentType );
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
