/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/24/2011 10:46:09 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_
#include "common.h"
#include "VertexFormats.h"

namespace Riot
{

    enum eComponentMessageType
    {
        eComponentMessageTransform,
        eComponentMessageUpdate,
        eComponentMessageMesh,
        eComponentMessageBoundingVolumeType,
        eComponentMessageCollision,

        eNUMCOMPONENTMESSAGES,
        eNULLCOMPONENTMESSAGE = -1
    };

    enum eComponentType
    {
        eComponentRender,
        eComponentLight,
        eComponentNewtonPhysics,
        eComponentCollidable,

        eNUMCOMPONENTS,
        eNULLCOMPONENT = -1
    };

    class CComponent;

    struct CComponentMessage
    {
        eComponentMessageType   m_nMessageType;
        uint                    m_nTargetObject;
        eComponentType          m_nOrigin;
        union
        {
            nativeuint  m_nData;
            pvoid       m_pData;
        };
    };
    
    class IComponent
    {
        friend class CObjectManager;
    public:
        virtual ~IComponent() { }
        
        //-----------------------------------------------------------------------------
        //  ProcessComponent
        //  Processes the component as necessary
        //-----------------------------------------------------------------------------
        virtual void ProcessComponent( void ) = 0;

        //-----------------------------------------------------------------------------
        //  ReceiveMessage
        //  Receives and processes a message
        //-----------------------------------------------------------------------------
        virtual void ReceiveMessage( uint nSlot, CComponentMessage& msg ) = 0;
        
        //-----------------------------------------------------------------------------
        //  Attach
        //  Attaches a component to an object
        //-----------------------------------------------------------------------------
        virtual void Attach( uint nIndex ) = 0;

        //-----------------------------------------------------------------------------
        //  Rettach
        //  Reattaches a component to an object, using it's last data
        //-----------------------------------------------------------------------------
        virtual void Reattach( uint nIndex, uint nOldIndex ) = 0;

        //-----------------------------------------------------------------------------
        //  Detach
        //  Detaches a component from an object, discarding the old data
        //-----------------------------------------------------------------------------
        virtual void Detach( uint nIndex ) = 0;
        
        //-----------------------------------------------------------------------------
        //  DetachAndSave
        //  Detaches a component from an object, saving the old data
        //-----------------------------------------------------------------------------
        virtual void DetachAndSave( uint nOldIndex, uint nNewIndex ) = 0;

        //-----------------------------------------------------------------------------
        //  RemoveInactive
        //  Removes the inactive component
        //-----------------------------------------------------------------------------
        virtual void RemoveInactive( uint nIndex ) = 0;
        
    protected:
        uint*       m_pObjects;
        atomic_t    m_nNumActiveComponents;
        atomic_t    m_nNumInactiveComponents;
    };

    /*****************************************************************************\
    \*****************************************************************************/

    //
#define BEGIN_DECLARE_COMPONENT( Component, Type, MaxCount )    \
    class Component : public IComponent                         \
    {                                                           \
    friend class CObjectManager;                                \
    public:                                                     \
    Component();                                                \
    ~Component();                                               \
    void Attach( uint nIndex );                                 \
    void Reattach( uint nIndex, uint nPrevIndex );              \
    void Detach( uint nIndex );                                 \
    void DetachAndSave( uint nOldIndex, uint nNewIndex );                          \
    void ProcessComponent( void );                              \
    void ReceiveMessage( uint nSlot, CComponentMessage& msg );  \
    void RemoveInactive( uint nIndex );                         \
    static const eComponentType ComponentType=Type;             \
    static const uint MaxComponents = MAX_OBJECTS;              \
    static const eComponentMessageType MessagesReceived[];      \
    static const uint NumMessagesReceived;                      \
    private:                                                    \
    static Component*  m_pInstance;
    //

    //
#define DECLARE_COMPONENT_DATA( Type, Name ) Type Name[MaxComponents]
    //

    //
#define END_DECLARE_COMPONENT }
    //

    /*****************************************************************************\
    \*****************************************************************************/
    
    class CMesh;

    //-----------------------------------------------------------------------------
    //  Render component
    //  Component used for rendering all basic objects
    //-----------------------------------------------------------------------------
    BEGIN_DECLARE_COMPONENT( CRenderComponent, eComponentRender, MAX_OBJECTS );
    //
    DECLARE_COMPONENT_DATA( CMesh*,     m_pMesh );
    DECLARE_COMPONENT_DATA( RTransform, m_Transform );
    //
    END_DECLARE_COMPONENT;
    //

    //-----------------------------------------------------------------------------
    //  Light component
    //  Turns an object into a light!
    //-----------------------------------------------------------------------------
    BEGIN_DECLARE_COMPONENT( CLightComponent, eComponentLight, MAX_LIGHTS );
    //
    DECLARE_COMPONENT_DATA( RTransform, m_Transform );
    DECLARE_COMPONENT_DATA( bool,       m_bUpdated );
    //
    END_DECLARE_COMPONENT;
    //
    
    //-----------------------------------------------------------------------------
    //  Collidable component
    //  Allows an object to collide with others
    //-----------------------------------------------------------------------------
    BEGIN_DECLARE_COMPONENT( CCollidableComponent, eComponentCollidable, MAX_OBJECTS );
    //
    enum VolumeType
    {
        BoundingSphere,
        AABB,
    };
    union BoundingVolume
    {
        struct _sphere
        {
            float3  position;
            float   radius; // Radius is stored pre-squared
        } sphere;
        struct _AABB
        {
            float3  min;
            float3  max;
        } aabb;
    };
    DECLARE_COMPONENT_DATA( BoundingVolume, m_Volume );
    DECLARE_COMPONENT_DATA( VolumeType,     m_nVolumeType );

    static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );
    public:
    static void CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, uint nIndex );
    //
    END_DECLARE_COMPONENT;
    //

    //-----------------------------------------------------------------------------
    //  NewtonPhysics component
    //  Makes an object behave with standard NewtonPhysicsian physics
    //-----------------------------------------------------------------------------
    BEGIN_DECLARE_COMPONENT( CNewtonPhysicsComponent, eComponentNewtonPhysics, MAX_OBJECTS );
    //
    DECLARE_COMPONENT_DATA( RTransform, m_Transform );
    DECLARE_COMPONENT_DATA( RVector3,   m_vVelocity );
    DECLARE_COMPONENT_DATA( bool,       m_bGravity );
    static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );
    //
    END_DECLARE_COMPONENT;
    //

} // namespace Riot

#endif // #ifndef _COMPONENT_H_
