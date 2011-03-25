/*********************************************************\
File:           SceneGraph.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 5:54:34 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "SceneGraph.h"
#include "memory.h"
#include "Object.h"
#include "Gfx\View.h"
#include <memory> // for memcpy

static const uint MAX_OBJECTS = 16 * 1024;
static const uint MAX_VIEWS = 8;

// CSceneGraph constructor
CSceneGraph::CSceneGraph()
    : m_ppAllSceneObjects( NULL )
    , m_ppRenderObjects( NULL )
    , m_ppAllViews( NULL )
    , m_nNumTotalObjects( 0 )
    , m_nNumRenderObjects( 0 )
    , m_nNumViews( 0 )
    , m_pMainView( NULL )
{
    m_ppAllSceneObjects = new CObject*[MAX_OBJECTS];
    m_ppRenderObjects = new CObject*[MAX_OBJECTS];
    m_ppAllViews = new CView*[MAX_VIEWS];
    m_ppAllViews[ m_nNumViews++ ] = new CView(); // create default view
    m_pMainView = m_ppAllViews[0];
}

CSceneGraph::~CSceneGraph()
{
    // clean up objects
    for( uint i = 0; i < m_nNumTotalObjects; ++i )
    {
        SAFE_DELETE( m_ppAllSceneObjects[i] );
    }
    SAFE_DELETE_ARRAY( m_ppAllSceneObjects );
    SAFE_DELETE_ARRAY( m_ppRenderObjects );

    // clean up the views
    for( uint i = 0; i < m_nNumViews; ++i )
    {
        SAFE_DELETE( m_ppAllViews[i] );
    }
    SAFE_DELETE_ARRAY( m_ppAllViews );
}

//-----------------------------------------------------------------------------
//  GetInstance
//  Gets/creates the one instance of the class
//-----------------------------------------------------------------------------
CSceneGraph* CSceneGraph::GetInstance( void )
{
    static CSceneGraph pInstance;
    return &pInstance;
}


//-----------------------------------------------------------------------------
//  AddObject
//  Adds an object to the scene
//-----------------------------------------------------------------------------
void CSceneGraph::AddObject( CObject* pObject )
{
    m_ppAllSceneObjects[ m_nNumTotalObjects++ ] = pObject;
}


//-----------------------------------------------------------------------------
//  UpdateObjects
//  Updates all the objects
//-----------------------------------------------------------------------------
void CSceneGraph::UpdateObjects( float fDeltaTime )
{
    for( uint i = 0; i < m_nNumTotalObjects; ++i )
    {
        m_ppAllSceneObjects[i]->Update( ((i%2) == 0) ? fDeltaTime : -fDeltaTime );
    }

    m_pMainView->UpdateViewMatrix();
}

//-----------------------------------------------------------------------------
//  GetRenderObjects
//  Returns all objects in view that need to be rendered
//-----------------------------------------------------------------------------
CObject** CSceneGraph::GetRenderObjects( uint* nCount )
{
    memcpy( m_ppRenderObjects, m_ppAllSceneObjects, sizeof( CObject* ) * m_nNumTotalObjects );
    *nCount = m_nNumRenderObjects = m_nNumTotalObjects;

    return m_ppRenderObjects;
}

//-----------------------------------------------------------------------------
//  AddView
//  Adds a view to the scene
//-----------------------------------------------------------------------------
void CSceneGraph::AddView( CView* pView )
{
    m_ppAllViews[ m_nNumViews++ ] = pView;
}

//-----------------------------------------------------------------------------
//  GetMainView
//  Returns the current active view
//-----------------------------------------------------------------------------
CView* CSceneGraph::GetMainView( void )
{
    return m_pMainView;
}
