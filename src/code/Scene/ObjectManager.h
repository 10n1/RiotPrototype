/*********************************************************\
File:           ObjectManager.h
Purpose:        Handles allocation of objects
Author:         Kyle Weicht
Created:        3/31/2011
Modified:       3/31/2011 10:59:46 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_
#include "common.h"
#include "Object.h"
#include "ComponentManager.h"


class CObjectManager
{
public:
    // CObjectManager constructor
    CObjectManager();

    // CObjectManager destructor
    ~CObjectManager();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  GetInstance
    //  Singleton protection
    //-----------------------------------------------------------------------------
    static CObjectManager* GetInstance( void );
    
    //-----------------------------------------------------------------------------
    //  CreateObject
    //  Creates a new object and returns its index
    //-----------------------------------------------------------------------------
    uint CreateObject( void );
    
    //-----------------------------------------------------------------------------
    //  GetObject
    //  Returns the object at the specified index
    //-----------------------------------------------------------------------------
    CObject* GetObject( uint nIndex );
    
    //-----------------------------------------------------------------------------
    //  AddComponent
    //  Adds a component to the specified object
    //-----------------------------------------------------------------------------
    void AddComponent( uint nIndex, eComponentType nType );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CObject     m_pObjects[MAX_OBJECTS];

    uint*       m_pFreeSlots;
    uint        m_nNumFreeSlots;

    uint        m_nNumObjects;
};


#endif // #ifndef _OBJECTMANAGER_H_
