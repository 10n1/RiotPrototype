/*********************************************************\
File:           ComponentManager.h
Purpose:        
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/23/2011 11:40:55 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTMANAGER_H_
#define _COMPONENTMANAGER_H_
#include "common.h"
#include "irefcounted.h"
#include "Component.h"

#define MAX_COMPONENT_MESSAGES (1024)

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
    
    //-----------------------------------------------------------------------------
    //  ProcessComponents
    //  Updates all the components, then resolves issues
    //-----------------------------------------------------------------------------
    void ProcessComponents( void );

    
    //-----------------------------------------------------------------------------
    //  PostMessage
    //  Posts a message to be processed
    //-----------------------------------------------------------------------------
    void PostMessage( eComponentMessageType nType, CObject* pObject, pvoid pData );
    void PostMessage( eComponentMessageType nType, CObject* pObject, nativeuint nData );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CComponent* m_ppComponents[eNUMCOMPONENTS];

    CComponentMessage   m_pMessages[MAX_COMPONENT_MESSAGES];
    uint                m_nNumMessages;
};


#endif // #ifndef _COMPONENTMANAGER_H_
