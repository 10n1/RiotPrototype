/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/19/2011 5:47:56 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _TERRAIN_H_
#define _TERRAIN_H_
#include "IRefCounted.h"
#include "VertexFormats.h"
#include "Renderer.h"

/*********************************************************\



                   ---------------------
                   | M | M | M | M | M |
                   ---------------------
                   | M | H | H | H | M |
                   ---------------------
                   | M | H | P | H | M |
                   ---------------------
                   | M | H | H | H | M |
                   ---------------------
                   | M | M | M | M | M |
                   ---------------------

                   9 High
                   16 Medium


\*********************************************************/

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
        //  CreateMesh
        //  Creates the terrain mesh
        //-----------------------------------------------------------------------------
        void CreateMesh( void );
        
    private:
        /***************************************\
        | class members                         |
        \***************************************/
        IGfxBuffer*     m_pVertexBuffer;

        sint    m_nXPos;
        sint    m_nYPos;
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
        void CalculateTileCenter( float fX, float fY, sint& nX, sint& nY );
        
        //-----------------------------------------------------------------------------
        //  SphereTerrainCollision
        //  Determines if a tree hits any triangles within the node
        //-----------------------------------------------------------------------------
        bool SphereTerrainCollision( const RSphere& s );
        
        //-----------------------------------------------------------------------------
        //  Update*
        //  Updates the terrain in either direction
        //-----------------------------------------------------------------------------
        void UpdatePosX( void );
        void UpdateNegX( void );
        void UpdatePosY( void );
        void UpdateNegY( void );
        
        //-----------------------------------------------------------------------------
        //  BuildTile
        //  Converts tile index nIndex into the tile with the specified center
        //-----------------------------------------------------------------------------
        void BuildLowTile( sint nIndex,  sint nX, sint nY );
        void BuildMedTile( sint nIndex,  sint nX, sint nY );
        void BuildHighTile( sint nIndex, sint nX, sint nY );

    public:

        /***************************************\
        | class members                         |
        \***************************************/
        
        static const sint   nTileDimensions = 128;
        static const sint   nTileHalfDimensions = nTileDimensions >> 1;

        static const sint   nHighGranularity = 1;
        static const sint   nHighTileDimensions = nTileDimensions / nHighGranularity;
        static const sint   nHighVertsTotal = (nTileDimensions+1) * (nTileDimensions+1);
        static const sint   nHighPolysTotal = nTileDimensions * nTileDimensions;
        static const sint   nHighIndices = nHighPolysTotal * 6;
        
        static const sint   nMedGranularity = 2;
        static const sint   nMedTileDimensions = nTileDimensions / nMedGranularity;
        static const sint   nMedVertsTotal = (nMedTileDimensions+1) * (nMedTileDimensions+1);
        static const sint   nMedPolysTotal = nMedTileDimensions * nMedTileDimensions;
        static const sint   nMedIndices = nMedPolysTotal * 6;
        
        static const sint   nLowGranularity = 8;
        static const sint   nLowTileDimensions = nTileDimensions / nLowGranularity;
        static const sint   nLowVertsTotal = (nLowTileDimensions+1) * (nLowTileDimensions+1);
        static const sint   nLowPolysTotal = nLowTileDimensions * nLowTileDimensions;
        static const sint   nLowIndices = nLowPolysTotal * 6;

        static const sint   nMaxTerrainDistance = 2 * 1024;
        static const sint   nTerrainTileDimensions = (nMaxTerrainDistance / nTileDimensions) + 1;
        static const sint   nTerrainTileDimensionsPerSide = (nMaxTerrainDistance / nTileDimensions) / 2;
        // There is an odd number of tiles because the center is one tile                                                                                                                

        static const sint   nNumHighTiles = 9;
        static const sint   nNumMedTiles = 16;
        static const sint   nNumLowTiles = (nTerrainTileDimensions*nTerrainTileDimensions) - 25;

    private:
        static VPosNormalTex    m_pVertices[ nHighVertsTotal ];
        static uint16           m_pHighIndices[ nHighIndices ];
        static uint16           m_pMedIndices[ nMedIndices ];
        static uint16           m_pLowIndices[ nLowIndices ];

        static IGfxBuffer*      m_pLowIndexBuffer;
        static IGfxBuffer*      m_pMedIndexBuffer;
        static IGfxBuffer*      m_pHighIndexBuffer;

        CTerrainTile    m_pLowTiles[ nNumLowTiles ];
        CTerrainTile    m_pMedTiles[ nNumMedTiles ];
        CTerrainTile    m_pHighTiles[ nNumMedTiles ];

        PerlinNoise     m_PerlinDetail;
        PerlinNoise     m_PerlinShape;

        sint            m_nCurrX;
        sint            m_nCurrY;

        IGfxTexture2D*  m_pTexture;
    };
    

} //namespace Riot

#endif // #ifndef _TERRAIN_H_
