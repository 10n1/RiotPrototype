/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/25/2011 7:07:38 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_
#include "common.h"
#include "VertexFormats.h"
#include "IComponent.h"

namespace Riot
{
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
    void Attach( uint nObject );                                 \
    void Reattach( uint nObject  );              \
    void Detach( uint nIndex );                                 \
    void DetachAndSave( uint nObject );                          \
    void ProcessComponent( void );                              \
    void ReceiveMessage( uint nSlot, CComponentMessage& msg );  \
    void RemoveInactive( uint nObject );                         \
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
