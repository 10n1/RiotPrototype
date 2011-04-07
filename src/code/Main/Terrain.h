/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/6/2011 10:42:01 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "Common.h"
#include "IRefCounted.h"

class CMesh;
class CMaterial;

enum
{
    TERRAIN_WIDTH = 64,
    TERRAIN_HEIGHT = 64,
};

class CTerrain : public IRefCounted
{
    // Allow the graphics engine to make the mesh
    friend class CGraphics;
    friend class CD3DGraphics;
    friend class COGLGraphics;

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
    /***************************************\
    | class members                         |
    \***************************************/
    float       m_fHeight[TERRAIN_WIDTH+1][TERRAIN_HEIGHT+1];

    CMesh*      m_pMesh;
    CMaterial*  m_pMaterial;
};


#endif // #ifndef _TERRAIN_H_
