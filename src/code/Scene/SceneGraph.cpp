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
#include <memory> // for memcpy

#define MAX_OBJECTS (16*1024)

// CSceneGraph constructor
CSceneGraph::CSceneGraph()
    : m_ppAllSceneObjects( NULL )
    , m_ppRenderObjects( NULL )
    , m_nNumTotalObjects( 0 )
    , m_nNumRenderObjects( 0 )
{
    m_ppAllSceneObjects = new CObject*[MAX_OBJECTS];
    m_ppRenderObjects = new CObject*[MAX_OBJECTS];
}

CSceneGraph::~CSceneGraph()
{
    SAFE_DELETE_ARRAY( m_ppAllSceneObjects );
    SAFE_DELETE_ARRAY( m_ppRenderObjects );
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
        m_ppAllSceneObjects[i]->Update( fDeltaTime );
    }
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
