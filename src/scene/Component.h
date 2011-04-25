/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/24/2011 5:35:53 PM
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


    class CComponent
    {
    public:
        // CComponent constructor
        CComponent();

        // CComponent destructor 
        virtual ~CComponent() = 0;
        /***************************************\
        | class methods                         |
        \***************************************/
        
        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        virtual void Shutdown( void );

        //-----------------------------------------------------------------------------
        //  AddComponent
        //  "Adds" a component to an object
        //-----------------------------------------------------------------------------
        sint AddComponent( uint nObject );

        //-----------------------------------------------------------------------------
        //  RemoveComponent
        //  "Removes" a component to an object
        //-----------------------------------------------------------------------------
        void RemoveComponent( uint nIndex );

        //-----------------------------------------------------------------------------
        //  Attach
        //  Attaches a component to an object
        //-----------------------------------------------------------------------------
        virtual void Attach( uint nIndex );

        //-----------------------------------------------------------------------------
        //  Rettach
        //  Reattaches a component to an object, using it's last data
        //-----------------------------------------------------------------------------
        virtual void Reattach( uint nIndex, uint nOldIndex );

        //-----------------------------------------------------------------------------
        //  Detach
        //  Detaches a component from an object
        //-----------------------------------------------------------------------------
        virtual void Detach( uint nIndex );

        //-----------------------------------------------------------------------------
        //  ProcessComponent
        //  Processes the component as necessary
        //-----------------------------------------------------------------------------
        virtual void ProcessComponent( void );

        //-----------------------------------------------------------------------------
        //  ReceiveMessage
        //  Receives and processes a message
        //-----------------------------------------------------------------------------
        virtual void ReceiveMessage( uint nSlot, CComponentMessage& msg );

        //-----------------------------------------------------------------------------
        //  Messages sent and recieved by this component are defined here
        //  THESE ARE EMPTY IN THE BASE COMPONENT, THEY'RE HERE FOR REFERENCE
        static const eComponentMessageType MessagesSent[];
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesSent;
        static const uint NumMessagesReceived;

        static const eComponentType ComponentType = eNULLCOMPONENT;
        static const uint MaxComponents = 0;
        //-----------------------------------------------------------------------------
    protected:
        /***************************************\
        | class members                         |
        \***************************************/
        sint            m_pObjectIndices[MAX_OBJECTS];  // Each object gets a slot
        uint*           m_pObjects;
        atomic_t        m_nNumComponents;
        sint            m_nMaxComponents;
        atomic_t        m_nFreeSlot;
        eComponentType  m_nType;
    };

    /*****************************************************************************\
    \*****************************************************************************/

    //
#define BEGIN_DECLARE_COMPONENT( Component, Type, MaxCount )    \
    class Component : public CComponent                         \
    {                                                           \
    friend class CComponentManager;                             \
    public:                                                     \
    Component();                                                \
    ~Component();                                               \
    void Attach( uint nIndex );                                 \
    void Reattach( uint nIndex, uint nPrevIndex );              \
    void Detach( uint nIndex );                                 \
    void ProcessComponent( void );                              \
    void ReceiveMessage( uint nSlot, CComponentMessage& msg );  \
    static const eComponentType ComponentType=Type;             \
    static const uint MaxComponents = MaxCount;                 \
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
    class CMaterial;

    //-----------------------------------------------------------------------------
    //  Render component
    //  Component used for rendering all basic objects
    //-----------------------------------------------------------------------------
    BEGIN_DECLARE_COMPONENT( CRenderComponent, eComponentRender, MAX_OBJECTS );
    //
    DECLARE_COMPONENT_DATA( CMesh*,     m_pMesh );
    DECLARE_COMPONENT_DATA( RTransform, m_Transform );
    void Shutdown( void );
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
