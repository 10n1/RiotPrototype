/*********************************************************\
File:           Mesh.h
Purpose:        Holds geometry data
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 9:24:00 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MESH_H_
#define _MESH_H_
#include "common.h"
#include "IRefCounted.h"
#include "GraphicsObjects.h"

namespace Riot
{
    class IGraphicsDevice;

    class CMesh : public IRefCounted
    {
        friend class CRenderer;
    public:
        // CMesh constructor
        CMesh();

        // CMesh destructor
        ~CMesh();
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
        IGraphicsDevice*    m_pDevice;

        IGfxBuffer*         m_pVertexBuffer;
        IGfxBuffer*         m_pIndexBuffer;
        uint                m_nIndexSize;
        uint                m_nVertexStride;
        uint                m_nIndexCount;
    };


} // namespace Riot

#endif // #ifndef _MESH_H_
