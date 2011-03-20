/*********************************************************\
File:           Mesh.h
Purpose:        Interface for holding geometry
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 9:52:02 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MESH_H_
#define _MESH_H_
#include "IRefCounted.h"
#include "Types.h"


/********************* File Format ***********************\
float   fVertexSize
float   fVertexCount
uint    nIndexSize
uint    nIndexCount
float   fVerts[]
uint    nIndices[]
\*********************************************************/

class CMesh : public IRefCounted
{
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
    uint    m_nVertexSize;
    uint    m_nIndexCount;
    uint    m_nIndexSize;
};


#endif // #ifndef _MESH_H_
