/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       4/4/2011 9:38:19 PM
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
    
    eNUMCOMPONENTMESSAGES,
    eNULLCOMPONENTMESSAGE = -1
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
    static const uint MaxComponents = 0;
    //-----------------------------------------------------------------------------
protected:
    /***************************************\
    | class members                         |
    \***************************************/
    CObject**   m_ppObjects;
    uint        m_nNumComponents;
    uint        m_nMaxComponents;
};

/*****************************************************************************\
\*****************************************************************************/

//
#define BEGIN_DECLARE_COMPONENT( Name, MaxCount )               \
class C##Name##Component : public CComponent                    \
{                                                               \
public:                                                         \
    C##Name##Component();                                       \
    ~C##Name##Component();                                      \
    void Attach( uint nIndex );                                 \
    void Detach( uint nIndex );                                 \
    void ProcessComponent( void );                              \
    void ReceiveMessage( uint nSlot, CComponentMessage& msg );  \
                                                                \
    static const eComponentType ComponentType=eComponent##Name; \
    static const uint MaxComponents = MaxCount;                 \
    static const eComponentMessageType MessagesReceived[];      \
    static const uint NumMessagesReceived;                      \
private:
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
BEGIN_DECLARE_COMPONENT( Render, MAX_OBJECTS )
//
DECLARE_COMPONENT_DATA( CMesh*,     m_pMesh );
DECLARE_COMPONENT_DATA( CMaterial*, m_pMaterial );    
DECLARE_COMPONENT_DATA( XMVECTOR,   m_vPosition );
DECLARE_COMPONENT_DATA( XMVECTOR,   m_vOrientation );
DECLARE_COMPONENT_DATA( XMMATRIX,   m_mWorldMatrix );
//
END_DECLARE_COMPONENT;
//

//-----------------------------------------------------------------------------
//  Update component
//  Handles basic updating of objects
//-----------------------------------------------------------------------------
BEGIN_DECLARE_COMPONENT( Update, MAX_OBJECTS )
//
DECLARE_COMPONENT_DATA(Transform, m_Transform );
//
END_DECLARE_COMPONENT;
//

//-----------------------------------------------------------------------------
//  Update component
//  Handles basic updating of objects
//-----------------------------------------------------------------------------
BEGIN_DECLARE_COMPONENT( Light, MAX_LIGHTS )
//
DECLARE_COMPONENT_DATA(XMVECTOR, m_vPosition );
DECLARE_COMPONENT_DATA(XMVECTOR, m_vOrientation );
DECLARE_COMPONENT_DATA(bool, m_bUpdated );
//
END_DECLARE_COMPONENT;
//

#endif // #ifndef _COMPONENT_H_
