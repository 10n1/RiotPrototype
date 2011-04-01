/*********************************************************\
File:           ComponentManager.h
Purpose:        
Author:         Kyle Weicht
Created:        3/23/2011
Modified:       3/31/2011 1:03:39 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTMANAGER_H_
#define _COMPONENTMANAGER_H_
#include "common.h"
#include "Component.h"

#define MAX_COMPONENT_MESSAGES (1024*1024)

class CComponentManager
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
    //  RemoveComponent
    //  Removes a component of the specified type
    //-----------------------------------------------------------------------------
    void RemoveComponent( eComponentType nType, uint nIndex );
    
    
    //-----------------------------------------------------------------------------
    //  ProcessComponents
    //  Updates all the components, then resolves issues
    //-----------------------------------------------------------------------------
    void ProcessComponents( void );

    
    //-----------------------------------------------------------------------------
    //  PostMessage
    //  Posts a message to be processed
    //-----------------------------------------------------------------------------
    void PostMessage( CComponentMessage& msg );
    void PostMessage( eComponentMessageType nType, CObject* pObject, pvoid pData, eComponentType nOrigin = eNULLCOMPONENT);
    void PostMessage( eComponentMessageType nType, CObject* pObject, nativeuint nData, eComponentType nOrigin = eNULLCOMPONENT );

    
    //-----------------------------------------------------------------------------
    //  SendMessage
    //  Sends the message
    //-----------------------------------------------------------------------------
    void SendMessage( CComponentMessage& msg );
    void SendMessage( eComponentMessageType nType, CObject* pObject, pvoid pData, eComponentType nOrigin = eNULLCOMPONENT );
    void SendMessage( eComponentMessageType nType, CObject* pObject, nativeuint nData, eComponentType nOrigin = eNULLCOMPONENT );

#ifdef DEBUG    
    //-----------------------------------------------------------------------------
    //  LoadComponent
    //  This function is for debugging creation of components. In release it becomes
    //      a simple, non-templated macro
    //-----------------------------------------------------------------------------
    template< class TheComponent>
    void LoadComponent( void );
#endif

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CComponent* m_ppComponents[eNUMCOMPONENTS];

    bool        m_bRegistered[eNUMCOMPONENTMESSAGES][eNUMCOMPONENTS];

    CComponentMessage   m_pMessages[MAX_COMPONENT_MESSAGES];
    uint                m_nNumMessages;
};


#endif // #ifndef _COMPONENTMANAGER_H_
