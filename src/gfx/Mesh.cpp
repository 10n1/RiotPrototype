/*********************************************************\
File:           Mesh.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       5/21/2011 2:46:03 PM
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
        m_pDevice->SetVertexBuffer( 0, m_pVertexBuffer, m_nVertexStride );
        m_pDevice->SetIndexBuffer( m_pIndexBuffer, m_nIndexSize );
        m_pDevice->DrawIndexedPrimitive( m_nIndexCount );
    }
}
