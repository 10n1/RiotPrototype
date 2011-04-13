/*********************************************************\
File:           Mesh.cpp
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 10:56:45 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Mesh.h"

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
    }
}
