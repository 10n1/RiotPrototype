/*********************************************************\
File:           Mesh.h
Purpose:        Interface for holding geometry
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/23/2011 7:07:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MESH_H_
#define _MESH_H_
#include "Common.h"
#include "IRefCounted.h"
#include "Types.h"
#include <Windows.h>
#include <xnamath.h>


/********************* File Format ***********************\
float   fVertexSize
float   fVertexCount
uint    nIndexSize
uint    nIndexCount
float   fVerts[]
uint    nIndices[]
\*********************************************************/

class CObject;

class CMesh : public IRefCounted
{
    friend class CObject;
public:
    // CMesh constructor
    CMesh();

    // CMesh destructor
    virtual ~CMesh();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  DrawMesh
    //  Passes the mesh to the GPU and renders it
    //-----------------------------------------------------------------------------
    virtual void DrawMesh( void ) = 0;

protected:
    /***************************************\
    | class members                         |
    \***************************************/
    XMVECTOR    m_vPosition;
    XMVECTOR    m_vOrientation;
    uint        m_nVertexSize;
    uint        m_nIndexCount;
    uint        m_nIndexSize;
};


#endif // #ifndef _MESH_H_
