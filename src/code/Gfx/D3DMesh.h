/*********************************************************\
File:           D3DMesh.h
Purpose:        
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 7:53:12 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DMESH_H_
#define _D3DMESH_H_
#include "Mesh.h"

struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
class CD3DGraphics;

class CD3DMesh : public CMesh
{
    friend class CD3DGraphics;
public:
    // CD3DMesh constructor
    CD3DMesh();

    // CD3DMesh destructor
    ~CD3DMesh();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  DrawMesh
    //  Passes the mesh to the GPU and renders it
    //-----------------------------------------------------------------------------
    void DrawMesh( void );
private:
    /***************************************\
    | class members                         |
    \***************************************/
    ID3D11InputLayout*      m_pVertexLayout;
    ID3D11Buffer*           m_pVertexBuffer;
    ID3D11Buffer*           m_pIndexBuffer;

    ID3D11DeviceContext*    m_pDeviceContext;
};


#endif // #ifndef _D3DMESH_H_
