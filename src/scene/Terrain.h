/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/17/2011 5:24:31 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "IRefCounted.h"

namespace Riot
{
    class CMesh;


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
        float       m_fHeight[TERRAIN_WIDTH+1][TERRAIN_HEIGHT+1];

        CMesh*      m_pMesh;
    };



} //namespace Riot

#endif // #ifndef _TERRAIN_H_
