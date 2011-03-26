/*********************************************************\
File:           Component.h
Purpose:        Stores objects components
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 11:42:13 PM
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
