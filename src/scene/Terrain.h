/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/5/2011 2:33:10 PM
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
    class CTerrainTile;
    
    class PerlinNoise
    {
    public:

        // Constructor
        PerlinNoise();
        PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

        // Get Height
        double GetHeight(double x, double y) const;

        // Get
        double Persistence() const { return persistence; }
        double Frequency()   const { return frequency;   }
        double Amplitude()   const { return amplitude;   }
        int    Octaves()     const { return octaves;     }
        int    RandomSeed()  const { return randomseed;  }

        // Set
        void Set(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

        void SetPersistence(double _persistence) { persistence = _persistence; }
        void SetFrequency(  double _frequency)   { frequency = _frequency;     }
        void SetAmplitude(  double _amplitude)   { amplitude = _amplitude;     }
        void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
        void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

    private:

        double Total(double i, double j) const;
        double GetValue(double x, double y) const;
        double Interpolate(double x, double y, double a) const;
        double Noise(int x, int y) const;

        double persistence, frequency, amplitude;
        int octaves, randomseed;
    };
    
    class CTerrainTile : public IRefCounted
    {
        // Allow the graphics engine to make the mesh
        friend class CRenderer;
        friend class CTerrain;
    public:
        // CTerrainTile constructor
        CTerrainTile();

        // CTerrainTile destructor
        ~CTerrainTile();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Render
        //  Renders the terrain
        //-----------------------------------------------------------------------------
        void Render( void );

        //-----------------------------------------------------------------------------
        //  CreateMesh
        //  Creates the terrain mesh
        //-----------------------------------------------------------------------------
        void CreateMesh( void );

    public:
        enum
        {
            TERRAIN_WIDTH = 128,
            TERRAIN_HEIGHT = 128,
        };

    private:

        /***************************************\
        | class members                         |
        \***************************************/
        static const uint nPolysWidth = TERRAIN_WIDTH;
        static const uint nPolysHeight = TERRAIN_HEIGHT;
        static const uint nPolysTotal = nPolysWidth * nPolysHeight;
        static const uint nVertsTotal = (nPolysWidth+1) * (nPolysHeight+1);
        static const uint nIndices = nPolysTotal * 6;

        //float           m_fHeight[TERRAIN_WIDTH+1][TERRAIN_HEIGHT+1];
        VPosNormalTex   m_pVertices[ nVertsTotal ];
        RVector3        m_pVertexPositions[ nVertsTotal ];
        uint16          m_pIndices[nIndices];

        CTerrain*       m_pParentTerrain;

        float           m_fXPos;
        float           m_fYPos;

        CMesh*          m_pMesh;
        IGfxTexture2D*  m_pTexture;
    };

    class CTerrain : public IRefCounted
    {
        // Allow the graphics engine to make the mesh
        friend class CRenderer;
        friend class CTerrainTile;
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
        //  Generates or returns the tile holding the specified X and Y values
        //-----------------------------------------------------------------------------
        void GenerateTerrain( void );
        CTerrainTile* GenerateTerrain( float fX, float fY );

        //-----------------------------------------------------------------------------
        //  CreateMesh
        //  Creates the terrain mesh
        //-----------------------------------------------------------------------------
        void CreateMesh( void );



    public:
        enum
        {
            TERRAIN_WIDTH = 128,
            TERRAIN_HEIGHT = 128,
        };

    private:

        /***************************************\
        | class members                         |
        \***************************************/
        static const uint nPolysWidth = TERRAIN_WIDTH;
        static const uint nPolysHeight = TERRAIN_HEIGHT;
        static const uint nPolysTotal = nPolysWidth * nPolysHeight;
        static const uint nVertsTotal = (nPolysWidth+1) * (nPolysHeight+1);
        static const uint nIndices = nPolysTotal * 6;
        static const uint nTerrainTiles = 128;

        CTerrainTile    m_pTerrainTiles[nTerrainTiles];

        PerlinNoise     m_PerlinShape;
        PerlinNoise     m_PerlinDetail;

        uint            m_nNumTiles;
    };
    

} //namespace Riot

#endif // #ifndef _TERRAIN_H_
