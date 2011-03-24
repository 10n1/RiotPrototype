/*********************************************************\
File:           SceneGraph.h
Purpose:        Controls the entire scene
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/23/2011 9:22:31 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_
#include "Common.h"
#include "Types.h"
#include "Component.h"

class CObject;
class CView;

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
    //  AddView
    //  Adds a view to the scene
    //-----------------------------------------------------------------------------
    void AddView( CView* pView );
    
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
    CView*      m_ppViews[8];
    CView*      m_pActiveView;

    uint        m_nNumTotalObjects;
    uint        m_nNumRenderObjects;
    uint        m_nNumViews;
};


#endif // #ifndef _SCENEGRAPH_H_
