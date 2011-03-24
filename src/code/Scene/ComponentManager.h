/*********************************************************\
File:           ComponentManager.h
Purpose:        
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 10:40:32 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTMANAGER_H_
#define _COMPONENTMANAGER_H_
#include "common.h"
#include "irefcounted.h"
#include "Component.h"

class CComponentManager : public IRefCounted
{
    // CComponentManager constructor
    CComponentManager();

    // CComponentManager destructor
    ~CComponentManager();
public:
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  GetInstance
    //  Singleton protection
    //-----------------------------------------------------------------------------
    static CComponentManager* GetInstance( void );

    
    //-----------------------------------------------------------------------------
    //  AddComponent
    //  Adds a component of the specified type
    //-----------------------------------------------------------------------------
    uint AddComponent( eComponentType nType, CObject* pObject );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CComponent* m_ppComponents[eNUMCOMPONENTS];
};


#endif // #ifndef _COMPONENTMANAGER_H_
