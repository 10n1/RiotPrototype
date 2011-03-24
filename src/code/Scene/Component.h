/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 10:31:58 PM
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

};

enum eComponentType
{
    eComponentPosition,

    eNUMCOMPONENTS
};

class CObject;

class CComponentMessage
{
public:
    // CComponentMessage constructor
    CComponentMessage();

    // CComponentMessage destructor
    virtual ~CComponentMessage();
    /***************************************\
    | class methods                         |
    \***************************************/

private:
    /***************************************\
    | class members                         |
    \***************************************/
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
    //  ProcessComponents
    //  Loops through each component, processing it
    //-----------------------------------------------------------------------------
    virtual void ProcessComponents( void );
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

private:
    /***************************************\
    | class members                         |
    \***************************************/
    XMVECTOR*   m_vPosition;
};



#endif // #ifndef _COMPONENT_H_
