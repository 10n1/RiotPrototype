/*********************************************************\
File:           Terrain.h
Purpose:        The terrain
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/5/2011 9:42:30 PM
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
        double PerlinInterpolate(double x, double y, double a) const;
        double Noise(int x, int y) const;

        double persistence, frequency, amplitude;
        int octaves, randomseed;
    };
    
    class CTerrainTile : public IRefCounted
    {
        // Allow the graphics engine to make the mesh
        friend class CRenderer;
        friend class CTerrain;
        
        //-----------------------------------------------------------------------------
        //  TSceneNode
        //  Basic interface for all other nodes (Terrain, Object and Parent)
        //-----------------------------------------------------------------------------
        struct TSceneNode : public RAABB
        {
            TSceneNode*     m_pParent;
            CTerrainTile*   m_pTile;

            void DrawNode( CRenderer* pRenderer, const RVector3& vColor )
            {
                pRenderer->DrawDebugBox( *this, vColor );
            }
        };

        
        //-----------------------------------------------------------------------------
        //  TTerrainParentNode
        //  The non-terrain nodes. All this parent node does is hold them
        //-----------------------------------------------------------------------------
        struct TTerrainParentNode : public TSceneNode
        {
            TSceneNode* m_pChildren[4];
            uint16      m_nNumChildren;
            uint16      m_nLowestParent;

            TTerrainParentNode()
                : TSceneNode()
            {
                m_pChildren[0] = NULL;
                m_pChildren[1] = NULL;
                m_pChildren[2] = NULL;
                m_pChildren[3] = NULL;

                m_nNumChildren  = 0;
                m_nLowestParent = 0;
            }
        };
        
        //-----------------------------------------------------------------------------
        //  TTerrainLeafNode
        //  Holds the terrain triangle data
        //-----------------------------------------------------------------------------
        struct TTerrainLeafNode : public TSceneNode
        {
            uint16  m_pCornerIndices[4];
            uint    m_nNumAdded;

            TTerrainLeafNode()
            {
                m_pCornerIndices[0] = -1;
                m_pCornerIndices[1] = -1;
                m_pCornerIndices[2] = -1;
                m_pCornerIndices[3] = -1;

                m_nNumAdded = 0;
            }

            bool SphereTriangleCollision( const RSphere& s )
            {
                for( uint i = 0; i < 2; ++i )
                {
                    RPlane trianglePlane( m_pTile->m_pVertexPositions[ m_pCornerIndices[0+i] ], m_pTile->m_pVertexPositions[ m_pCornerIndices[1+i] ], m_pTile->m_pVertexPositions[ m_pCornerIndices[2+i] ] );

                    float fDistance = DistanceFromPlane( trianglePlane, s.position );

                    if( fDistance > s.radius )
                    {
                        // The sphere doesn't interact the triangles plane
                        continue;
                    }
                
                    RVector3 planeCollisionPoint = s.position - trianglePlane.normal;
                    if( m_pTile->IsPointInTriangle( planeCollisionPoint, m_pCornerIndices[0+i], m_pCornerIndices[1+i], m_pCornerIndices[2+i] ) )
                    {
                        // we collided, break
                        return true;
                    }
                }

                return false;
            }
        };

    public:
        // CTerrainTile constructor
        CTerrainTile();

        // CTerrainTile destructor
        ~CTerrainTile();
        /***************************************\
        | class methods                         |
        \***************************************/        

        //-----------------------------------------------------------------------------
        //  BuildParentNodes
        //  Constructs the top of the tree
        //-----------------------------------------------------------------------------
        void BuildParentNodes( TTerrainParentNode* pNode, TTerrainParentNode* pParent );

        //-----------------------------------------------------------------------------
        //  BuildSceneGraph
        //  Builds the scene graph
        //-----------------------------------------------------------------------------
        void BuildSceneGraph( void );

        //-----------------------------------------------------------------------------
        //  RecomputeSceneGraphBounds
        //  Recomputes the bounds of the top of the tree
        //-----------------------------------------------------------------------------
        void RecomputeSceneGraphBounds( TSceneNode* pNode );

        //-----------------------------------------------------------------------------
        //  AddTriangleToGraph
        //  Adds a triangle to the graph
        //-----------------------------------------------------------------------------
        void AddTriangleToGraph( TSceneNode* pNode, uint16* pIndices, bool bLeaf );

        //-----------------------------------------------------------------------------
        //  Render
        //  Renders the terrain
        //-----------------------------------------------------------------------------
        void Render( void );
        
        //-----------------------------------------------------------------------------
        //  RenderGraph
        //  Renders the terrain
        //-----------------------------------------------------------------------------
        void RenderGraph( TTerrainParentNode* pNode, uint nDepth );

        //-----------------------------------------------------------------------------
        //  CreateMesh
        //  Creates the terrain mesh
        //-----------------------------------------------------------------------------
        void CreateMesh( void );
        
        //-----------------------------------------------------------------------------
        //  IsPointInTriangle
        //  Determines if a point is inside a particular triangle
        //-----------------------------------------------------------------------------
        // Reference: http://www.peroxide.dk/papers/collision/collision.pdf
#define FloatBitwiseToInt(a) ((uint32&) a)
        bool IsPointInTriangle( const RVector3& point, uint16 nVert0, uint16 nVert1, uint16 nVert2 )
        {
            RVector3& vVert0 = m_pVertexPositions[ nVert0 ];
            RVector3& vVert1 = m_pVertexPositions[ nVert1 ];
            RVector3& vVert2 = m_pVertexPositions[ nVert2 ];


            RVector3 vSide1 = vVert1 - vVert0;
            RVector3 vSide2 = vVert2 - vVert0;

            //Plane

            float a = DotProduct( vSide1, vSide1 );
            float b = DotProduct( vSide1, vSide2 );
            float c = DotProduct( vSide2, vSide2 );

            float ac_bb = (a*c) - (b*b);

            RVector3 vp( point.x - vVert0.x, point.y - vVert0.y, point.z - vVert0.z );

            float d = DotProduct( vp, vSide1 );
            float e = DotProduct( vp, vSide2 );

            float x = (d*c) - (e*b);
            float y = (e*a) - (d*b);
            float z = x+y-ac_bb;


            return (( FloatBitwiseToInt(z) & ~(FloatBitwiseToInt(x)|FloatBitwiseToInt(y)) ) & 0x80000000);
        }
        
        //-----------------------------------------------------------------------------
        //  SphereTerrainCollision
        //  Determines if a tree hits any triangles within the node
        //-----------------------------------------------------------------------------
        bool SphereTerrainCollision( const RSphere& s );
        bool SphereTerrainCollision( TSceneNode* pNode, const RSphere& s );

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
        static const sint   nNumParentNodes = 85; // 1 + 4 + 16 + 64 + 256
        static const sint   nNumLeafNodes = nTileDimensions * nTileDimensions; 

    private:

        RVector3        m_pVertexPositions[ nVertsTotal ];

        uint16          m_pIndices[ nIndices ];

        TTerrainParentNode  m_pParentNodes[ nNumParentNodes ];
        TTerrainLeafNode    m_pLeafNodes[ nNumLeafNodes ];

        TTerrainParentNode* m_pTerrainGraph;

        uint            m_nNumParents;
        uint            m_nNumLeaves;

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
