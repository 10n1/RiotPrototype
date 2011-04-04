/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/3/2011 9:15:09 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_
#include "common.h"
#include "IRefCounted.h"

#include <Windows.h>
#include <xnamath.h>
#include "RiotMath.h"

#define MAX_OBJECTS (16*1024)

struct Transform
{
    XMVECTOR    vPosition;
    XMVECTOR    vOrientation;
};

enum eComponentMessageType
{
    eComponentMessageTransform,
    eComponentMessageUpdate,
    
    eNUMCOMPONENTMESSAGES
};

enum eComponentType
{
    eComponentRender,
    eComponentUpdate,
    eComponentLight,

    eNUMCOMPONENTS,
    eNULLCOMPONENT = -1
};

class CObject;
class CComponent;

struct CComponentMessage
{
    eComponentMessageType   m_nMessageType;
    CObject*                m_pTargetObject;
    eComponentType          m_nOrigin;
    union
    {
        nativeuint  m_nData;
        pvoid       m_pData;
    };
};


class CComponent : public IRefCounted
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
    //  AddComponent
    //  "Adds" a component to an object
    //-----------------------------------------------------------------------------
    uint AddComponent( CObject* pObject );
    
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
    static const uint MaxComponents = MAX_OBJECTS;
    //-----------------------------------------------------------------------------
protected:
    /***************************************\
    | class members                         |
    \***************************************/
    CObject**   m_ppObjects;
    uint        m_nNumComponents;
};

class CMesh;
class CMaterial;

class CRenderComponent : public CComponent
{
public:
    // CRenderComponent constructor
    CRenderComponent();

    // CRenderComponent destructor
    ~CRenderComponent();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void Attach( uint nIndex );

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void Detach( uint nIndex );
    
    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void ProcessComponent( void );

    //-----------------------------------------------------------------------------
    //  Messages sent and recieved by this component are defined here
    static const eComponentMessageType MessagesReceived[];
    static const uint NumMessagesReceived;

    static const eComponentType ComponentType = eComponentRender;
    static const uint MaxComponents = MAX_OBJECTS;
    //-----------------------------------------------------------------------------
    
    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    virtual void ReceiveMessage( uint nSlot, CComponentMessage& msg );
private:
    /***************************************\
    | class members                         |
    \***************************************/
    CMesh*      m_pMesh[MAX_OBJECTS];
    CMaterial*  m_pMaterial[MAX_OBJECTS];    
    XMVECTOR    m_vPosition[MAX_OBJECTS];
    XMVECTOR    m_vOrientation[MAX_OBJECTS];
    XMMATRIX    m_mWorldMatrix[MAX_OBJECTS];
};

class CUpdateComponent : public CComponent
{
public:
    // CUpdateComponent constructor
    CUpdateComponent();

    // CUpdateComponent destructor
    ~CUpdateComponent();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void Attach( uint nIndex );
    
    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void Detach( uint nIndex );

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void ProcessComponent( void );

    //-----------------------------------------------------------------------------
    //  Messages sent and recieved by this component are defined here
    static const eComponentMessageType MessagesReceived[];
    static const uint NumMessagesReceived;

    static const eComponentType ComponentType = eComponentUpdate;
    static const uint MaxComponents = MAX_OBJECTS;
    //-----------------------------------------------------------------------------
    
    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    virtual void ReceiveMessage( uint nSlot, CComponentMessage& msg );
private:
    /***************************************\
    | class members                         |
    \***************************************/
    Transform   m_Transform[MAX_OBJECTS];
};

class CLightComponent : public CComponent
{
public:
    // CLightComponent constructor
    CLightComponent();

    // CLightComponent destructor
    ~CLightComponent();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  Attach
    //  Attaches a component to an object
    //-----------------------------------------------------------------------------
    void Attach( uint nIndex );

    //-----------------------------------------------------------------------------
    //  Detach
    //  Detaches a component from an object
    //-----------------------------------------------------------------------------
    void Detach( uint nIndex );
    
    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void ProcessComponent( void );

    //-----------------------------------------------------------------------------
    //  Messages sent and recieved by this component are defined here
    static const eComponentMessageType MessagesReceived[];
    static const uint NumMessagesReceived;

    static const eComponentType ComponentType = eComponentLight;
    static const uint MaxComponents = MAX_LIGHTS;
    //-----------------------------------------------------------------------------
    
    //-----------------------------------------------------------------------------
    //  ReceiveMessage
    //  Receives and processes a message
    //-----------------------------------------------------------------------------
    virtual void ReceiveMessage( uint nSlot, CComponentMessage& msg );
private:
    /***************************************\
    | class members                         |
    \***************************************/
    XMVECTOR    m_vPosition[MAX_LIGHTS];
    XMVECTOR    m_vOrientation[MAX_LIGHTS];
    bool        m_bUpdated[MAX_LIGHTS];
};

#endif // #ifndef _COMPONENT_H_
