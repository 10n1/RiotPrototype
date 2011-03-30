/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/29/2011 11:59:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENT_H_
#define _COMPONENT_H_
#include "common.h"
#include "IRefCounted.h"

#include <Windows.h>
#include <xnamath.h>

#define MAX_OBJECTS (16*1024)

enum eComponentMessageType
{
    eComponentMessagePosition,
    eComponentMessageOrientation,
    eComponentMessageUpdate,


    eNUMCOMPONENTMESSAGES
};

enum eComponentType
{
    eComponentPosition,
    eComponentUpdate,

    eNUMCOMPONENTS
};

class CObject;

struct CComponentMessage
{
    eComponentMessageType   m_nMessageType;
    CObject*                m_pTargetObject;
    union
    {
        pvoid       m_pData;
        nativeuint  m_nData;
    };
};


class CComponent : public IRefCounted
{
public:
    // CComponent constructor
    CComponent();

    // CComponent destructor
    virtual ~CComponent();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  AddComponent
    //  "Adds" a component to an object
    //-----------------------------------------------------------------------------
    virtual uint AddComponent( CObject* pObject );

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

    static const eComponentType ComponentType = eNUMCOMPONENTS;
    //-----------------------------------------------------------------------------
protected:
    /***************************************\
    | class members                         |
    \***************************************/
    CObject**   m_ppObjects;
    uint*       m_pFreeSlots;
    uint        m_nNumFreeSlots;
    uint        m_nNumComponents;
};

class CPositionComponent : public CComponent
{
public:
    // CPositionComponent constructor
    CPositionComponent();

    // CPositionComponent destructor
    ~CPositionComponent();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  AddComponent
    //  "Adds" a component to an object
    //-----------------------------------------------------------------------------
    uint AddComponent( CObject* pObject );



    //-----------------------------------------------------------------------------
    //  Messages sent and recieved by this component are defined here
    static const eComponentMessageType MessagesSent[];
    static const eComponentMessageType MessagesReceived[];
    static const uint NumMessagesSent;
    static const uint NumMessagesReceived;

    static const eComponentType ComponentType = eComponentPosition;
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
    XMVECTOR    m_vPosition[MAX_OBJECTS];
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
    //  AddComponent
    //  "Adds" a component to an object
    //-----------------------------------------------------------------------------
    uint AddComponent( CObject* pObject );

    //-----------------------------------------------------------------------------
    //  ProcessComponent
    //  Processes the component as necessary
    //-----------------------------------------------------------------------------
    void ProcessComponent( void );

    //-----------------------------------------------------------------------------
    //  Messages sent and recieved by this component are defined here
    static const eComponentMessageType MessagesSent[];
    static const eComponentMessageType MessagesReceived[];
    static const uint NumMessagesSent;
    static const uint NumMessagesReceived;

    static const eComponentType ComponentType = eComponentUpdate;
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
};

#endif // #ifndef _COMPONENT_H_
