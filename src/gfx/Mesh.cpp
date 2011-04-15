/*********************************************************\
File:           Mesh.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 10:35:24 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Mesh.h"
#include "Graphics.h"

namespace Riot
{
    /***************************************\
    | class members                         |
    \***************************************/

    // CMesh constructor
    CMesh::CMesh()
        : m_pDevice( NULL )
        , m_pIndexBuffer( NULL )
        , m_pVertexBuffer( NULL )
    {
    }

    // CMesh destructor
    CMesh::~CMesh()
    {
        SAFE_RELEASE( m_pIndexBuffer );
        SAFE_RELEASE( m_pVertexBuffer );
    }

    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  DrawMesh
    //  Passes the mesh to the GPU and renders it
    //-----------------------------------------------------------------------------
    void CMesh::DrawMesh( void )
    {
        m_pDevice->SetVertexBuffer( m_pVertexBuffer, m_nVertexStride );
        m_pDevice->SetIndexBuffer( m_pIndexBuffer, m_nIndexSize );
        m_pDevice->DrawIndexedPrimitive( m_nIndexCount );
    }
}
