/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/6/2011 11:50:16 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "IRefCounted.h"
#include "VertexFormats.h"
#include "Renderer.h"

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
        PerlinNoise(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed);

        // Get Height
        float GetHeight(float x, float y) const;

        // Get
        float Persistence() const { return persistence; }
        float Frequency()   const { return frequency;   }
        float Amplitude()   const { return amplitude;   }
        int    Octaves()     const { return octaves;     }
        int    RandomSeed()  const { return randomseed;  }

        // Set
        void Set(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed);

        void SetPersistence(float _persistence) { persistence = _persistence; }
        void SetFrequency(  float _frequency)   { frequency = _frequency;     }
        void SetAmplitude(  float _amplitude)   { amplitude = _amplitude;     }
        void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
        void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

    private:

        float Total(float i, float j) const;
        float GetValue(float x, float y) const;
        float PerlinInterpolate(float x, float y, float a) const;
        float Noise(int x, int y) const;

        float persistence, frequency, amplitude;
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
        
    private:

        /***************************************\
        | class members                         |
        \***************************************/
    public:

        static const sint   nTileDimensions = 16;
        static const sint   nTileHalfDimensions = nTileDimensions >> 1;
        static const sint   nVertsTotal = (nTileDimensions+1) * (nTileDimensions+1);
        static const sint   nPolysTotal = nTileDimensions * nTileDimensions;
        static const sint   nIndices = nPolysTotal * 6;

    private:

        RVector3        m_pVertexPositions[ nVertsTotal ];

        uint16          m_pIndices[ nIndices ];

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
        //  CenterTerrain
        //  Centers the active portion of the terrain around a point
        //-----------------------------------------------------------------------------
        void CenterTerrain( const RVector3& pos, float fRadius );
        
        //-----------------------------------------------------------------------------
        //  CalculateTileCenter
        //  Calculates a tiles center
        //-----------------------------------------------------------------------------
        void CalculateTileCenter( float& fX, float& fY );
        
        //-----------------------------------------------------------------------------
        //  SphereTerrainCollision
        //  Determines if a tree hits any triangles within the node
        //-----------------------------------------------------------------------------
        bool SphereTerrainCollision( const RSphere& s );

    private:

        /***************************************\
        | class members                         |
        \***************************************/
        enum { MAX_TERRAIN_TILES = 1024 };

        CTerrainTile    m_pTerrainTiles[MAX_TERRAIN_TILES];
        uint            m_nFreeTiles[MAX_TERRAIN_TILES];
        uint            m_pActiveTiles[MAX_TERRAIN_TILES];

        PerlinNoise     m_PerlinShape;
        PerlinNoise     m_PerlinDetail;

        atomic_t        m_nNumTiles;
        uint            m_nNumFreeTiles;
    };
    

} //namespace Riot

#endif // #ifndef _TERRAIN_H_
