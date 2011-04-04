/*********************************************************\
File:           Mesh.h
Purpose:        Interface for holding geometry
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/3/2011 9:02:09 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MESH_H_
#define _MESH_H_
#include "Common.h"
#include "RenderObject.h"
#include "Types.h"
#include <Windows.h>
#include <xnamath.h>
#include "RiotMath.h"


/********************* File Format ***********************\
float   fVertexSize
float   fVertexCount
uint    nIndexSize
uint    nIndexCount
float   fVerts[]
uint    nIndices[]
\*********************************************************/

class CRenderComponent;
class CObject;

class CMesh : public CRenderObject
{
    friend class CObject;
    friend class CRenderComponent;
public:
    // CMesh constructor
    CMesh();

    // CMesh destructor
    virtual ~CMesh();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  ProcessObject
    //  Passes the mesh to the GPU and renders it
    //-----------------------------------------------------------------------------
    virtual void ProcessObject( void ) = 0;

protected:
    /***************************************\
    | class members                         |
    \***************************************/
    uint        m_nVertexSize;
    uint        m_nIndexCount;
    uint        m_nIndexSize;
};


#endif // #ifndef _MESH_H_
