/*********************************************************\
File:           SceneGraph.h
Purpose:        Controls the entire scene
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 5:40:01 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_
#include "Types.h"

class CObject;

class CSceneGraph
{
private:
    // CSceneGraph constructor
    CSceneGraph();
    CSceneGraph(CSceneGraph& ref){}
    CSceneGraph& operator=(CSceneGraph& ref){}
    // CSceneGraph destructor
    ~CSceneGraph();
public:
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  GetInstance
    //  Gets/creates the one instance of the class
    //-----------------------------------------------------------------------------
    static CSceneGraph* GetInstance( void );

    //-----------------------------------------------------------------------------
    //  AddObject
    //  Adds an object to the scene
    //-----------------------------------------------------------------------------
    void AddObject( CObject* pObject );
    // TODO: How do we remove an object?
    // TODO: Where are the objects created?

    
    //-----------------------------------------------------------------------------
    //  UpdateObjects
    //  Updates all the objects
    //-----------------------------------------------------------------------------
    void UpdateObjects( float fDeltaTime );
    
    //-----------------------------------------------------------------------------
    //  GetRenderObjects
    //  Returns all objects in view that need to be rendered
    //-----------------------------------------------------------------------------
    CObject** GetRenderObjects( uint* nCount );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CObject**   m_ppAllSceneObjects;
    CObject**   m_ppRenderObjects;

    uint        m_nNumTotalObjects;
    uint        m_nNumRenderObjects;
};


#endif // #ifndef _SCENEGRAPH_H_
