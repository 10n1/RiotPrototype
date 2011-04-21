/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/20/2011 9:09:40 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "IRefCounted.h"
#include "VertexFormats.h"

namespace Riot
{
    class CMesh;
    class IGfxTexture2D;

    class CTerrain : public IRefCounted
    {
        // Allow the graphics engine to make the mesh
        friend class CRenderer;

    public:
        // CTerrain constructor
        CTerrain();

        // CTerrain destructor
        ~CTerrain();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Render
        //  Renders the terrain
        //-----------------------------------------------------------------------------
        void Render( void );


        //-----------------------------------------------------------------------------
        //  GenerateTerrain
        //  Generates the terrain
        //-----------------------------------------------------------------------------
        void GenerateTerrain( void );

        //-----------------------------------------------------------------------------
        //  CreateMesh
        //  Creates the terrain mesh
        //-----------------------------------------------------------------------------
        void CreateMesh( void );

    private:
        enum
        {
            TERRAIN_WIDTH = 128,
            TERRAIN_HEIGHT = 128,
        };

        /***************************************\
        | class members                         |
        \***************************************/        
        static const uint nPolysWidth = TERRAIN_WIDTH;
        static const uint nPolysHeight = TERRAIN_HEIGHT;
        static const uint nPolysTotal = nPolysWidth * nPolysHeight;
        static const uint nVertsTotal = (nPolysWidth+1) * (nPolysHeight+1);
        static const uint nIndices = nPolysTotal * 6;

        float       m_fHeight[TERRAIN_WIDTH+1][TERRAIN_HEIGHT+1];
        VPosNormalTex  m_pVertices[ nVertsTotal ];
        uint16      m_pIndices[nIndices];

        CMesh*          m_pMesh;
        IGfxTexture2D*  m_pTexture;
    };



} //namespace Riot

#endif // #ifndef _TERRAIN_H_
