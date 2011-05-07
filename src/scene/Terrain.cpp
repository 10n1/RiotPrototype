/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/7/2011 9:47:38 AM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"
#include "ComponentCollidable.h"

namespace Riot
{    
    static float fPersistance = 0.5f;
    static float fFrequency = 0.0625f / 16.0f;
    static float fAmplitude = 150.0f;
    static uint  nOctaves = 6;
    static uint  nSeed = 100;//RandShort();

    // CTerrain constructor
    CTerrain::CTerrain()
        : m_PerlinDetail( fPersistance, fFrequency, 150.0f, nOctaves, nSeed )
        , m_PerlinShape( fPersistance, fFrequency, 150.0f, nOctaves, nSeed << 1 )
        , m_nNumTiles( 0 )
    {        
        for( sint i = MAX_TERRAIN_TILES - 1, j = 0; i >= 0; --i, ++j )
        {
            m_nFreeTiles[j] = i;
        }

        m_nNumFreeTiles = MAX_TERRAIN_TILES;
    }

    // CTerrain destructor
    CTerrain::~CTerrain()
    {
    }


    //-----------------------------------------------------------------------------
    //  Render
    //  Renders the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::Render( void )
    {
        for( uint i = 0; i < m_nNumTiles; ++i )
        {
            m_pTerrainTiles[ m_pActiveTiles[i] ].Render();
        }
    }

    //-----------------------------------------------------------------------------
    //  CalculateTileCenter
    //  Calculates a tiles center
    //-----------------------------------------------------------------------------
    void CTerrain::CalculateTileCenter( float& fX, float& fY )
    {
        float fTileDimensions = (float)CTerrainTile::nTileDimensions;
        float fTileHalfDimensions = (float)CTerrainTile::nTileHalfDimensions;

        // Calculate the tiles center
        bool bLessThanZero = false;
        if( fX < 0.0f )
            bLessThanZero = true;

        fX = fX / fTileDimensions;
        if( bLessThanZero )
        {
            fX = floorf( fX ) + 1.0f;
        }
        else
        {
            fX = ceilf( fX );
        }
        fX *= fTileDimensions;
        fX -= fTileHalfDimensions;


        if( fY < 0.0f )
            bLessThanZero = true;
        else
            bLessThanZero = false;

        fY = fY / fTileDimensions;
        if( bLessThanZero )
        {
            fY = floorf( fY ) + 1.0f;
        }
        else
        {
            fY = ceilf( fY );
        }
        fY *= fTileDimensions;
        fY -= fTileHalfDimensions;
    }

    //-----------------------------------------------------------------------------
    //  GenerateTerrain
    //  Generates the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::GenerateTerrain( void )
    {
        //float fTileDimensions = (float)CTerrainTile::nTileDimensions;
        //float fTileHalfDimensions = (float)CTerrainTile::nTileHalfDimensions;
        //
        //for( sint i = -4; i < 4; ++i )
        //{
        //    for( sint j = -4; j < 4; ++j )
        //    {
        //        GenerateTerrain( i*fTileDimensions, j*fTileDimensions );
        //    }
        //}

        //GenerateTerrain( 64.0f, 64.0f );
    }

    CTerrainTile* CTerrain::GenerateTerrain( float fX, float fY )
    {
        float fTileDimensions = (float)CTerrainTile::nTileDimensions;
        float fTileHalfDimensions = (float)CTerrainTile::nTileHalfDimensions;
        
        // Calculate the center
        CalculateTileCenter( fX, fY );

        // See if this tile already exists
        for( uint i = 0; i < m_nNumTiles; ++i )
        {
            if(    (m_pTerrainTiles[ m_pActiveTiles[i] ].m_fXPos == fX )
                && (m_pTerrainTiles[ m_pActiveTiles[i] ].m_fYPos == fY ) )
            {
                return &m_pTerrainTiles[ m_pActiveTiles[i] ];
            }
        }

        ASSERT( m_nNumFreeTiles );

        // Now grab a tile from the list
        uint nFreeIndex = --m_nNumFreeTiles;
        uint nIndex = AtomicIncrement( &m_nNumTiles ) - 1;

        CTerrainTile* pTile = &m_pTerrainTiles[ m_nFreeTiles[nFreeIndex] ];
        m_pActiveTiles[ nIndex ] = m_nFreeTiles[ nFreeIndex ];

        pTile->m_pParentTerrain = this;
        pTile->m_fXPos = fX;
        pTile->m_fYPos = fY;
        
        sint nX;
        sint nY;
        sint nVertex = 0;
        for( fX = pTile->m_fXPos-fTileHalfDimensions, nX = 0; fX <= pTile->m_fXPos+fTileHalfDimensions; fX += 1.0f, ++nX  )
        {
            for( fY = pTile->m_fYPos-fTileHalfDimensions, nY = 0; fY <= pTile->m_fYPos+fTileHalfDimensions; fY += 1.0f, ++nY )
            {
                pTile->m_pVertexPositions[ nVertex++ ] = RVector3( fX, m_PerlinDetail.GetHeight( fX, fY ), fY );
            }
        }

        pTile->CreateMesh();

        return pTile;
    }       

    //-----------------------------------------------------------------------------
    //  CenterTerrain
    //  Centers the active portion of the terrain around a point
    //-----------------------------------------------------------------------------
    void CTerrain::CenterTerrain( const RVector3& pos, float fRadius )
    {
        float fX = pos.x;
        float fY = pos.z;

        // First remove tiles that aren't in range.
        for( uint i = 0; i < m_nNumTiles; ++i )
        {
            if(    (m_pTerrainTiles[ m_pActiveTiles[i] ].m_fXPos + CTerrainTile::nTileDimensions) < (pos.x - fRadius)
                || (m_pTerrainTiles[ m_pActiveTiles[i] ].m_fYPos + CTerrainTile::nTileDimensions) < (pos.z - fRadius)
                || (m_pTerrainTiles[ m_pActiveTiles[i] ].m_fXPos - CTerrainTile::nTileDimensions) > (pos.x + fRadius)
                || (m_pTerrainTiles[ m_pActiveTiles[i] ].m_fYPos - CTerrainTile::nTileDimensions) > (pos.z + fRadius) )
            {
                // Remove it
                // TODO: This needs to happen here, but this tile is already queued in the
                //  renderers buffer....
                SAFE_RELEASE( m_pTerrainTiles[ m_pActiveTiles[i] ].m_pMesh );
                SAFE_RELEASE( m_pTerrainTiles[ m_pActiveTiles[i] ].m_pTexture );
        
                m_nFreeTiles[ m_nNumFreeTiles++ ] = m_pActiveTiles[i];
                m_pActiveTiles[i] = m_pActiveTiles[ --m_nNumTiles ];
                --i;
            }
        }
        
        // Now build the radius
        for( float fX = pos.x - fRadius; fX <= pos.x + fRadius; fX += CTerrainTile::nTileDimensions )
        {
            for( float fY = pos.z - fRadius; fY <= pos.z + fRadius; fY += CTerrainTile::nTileDimensions )
            {
                GenerateTerrain( fX, fY );
            }
        }
    }
    
    //-----------------------------------------------------------------------------
    //  SphereTerrainCollision
    //  Determines if a tree hits any triangles within the node
    //-----------------------------------------------------------------------------
    bool CTerrain::SphereTerrainCollision( const RSphere& s )
    {
        float fGround = m_PerlinDetail.GetHeight( s.position.x, s.position.z );

        if( s.position.y - fGround < s.radius )
        {
            return true;
        }
        return false;
    }

    /*************************************************************************\
    \*************************************************************************/
    // CTerrainTile constructor
    CTerrainTile::CTerrainTile()
        : m_pTexture( NULL )
        , m_pMesh( NULL )
        , m_fXPos( 0.0f )
        , m_fYPos( 0.0f )
    {
    }

    // CTerrainTile destructor
    CTerrainTile::~CTerrainTile()
    {
        SAFE_RELEASE( m_pMesh );
        SAFE_RELEASE( m_pTexture );
    }

    //-----------------------------------------------------------------------------
    //  Render
    //  Renders the terrain
    //-----------------------------------------------------------------------------
    void CTerrainTile::Render( void )
    {
        static CRenderer* pRender = Engine::GetRenderer();

        // Pass to the render engine
        TRenderCommand cmd = { m_pMesh, m_pTexture };
        RTransform t = RTransform();
        pRender->AddCommand( cmd, t );
    }

    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates the terrain mesh
    //-----------------------------------------------------------------------------
    void CTerrainTile::CreateMesh( void )
    {
        SAFE_RELEASE( m_pMesh );

        static CRenderer* pRender = Engine::GetRenderer();

        byte* pData = new byte[ sizeof( VPosNormalTex ) * nVertsTotal];

        VPosNormalTex* pVertices = (VPosNormalTex*)pData;

        // Create the vertices
        sint nVertex = 0;
        sint nIndex = 0;
        sint nX;
        sint nY;

        // Index them
        for( nX = 0; nX < nTileDimensions; ++nX )
        {
            for( nY = 0; nY < nTileDimensions; ++nY )
            {
                uint nStart = nX * nTileDimensions;
                
                m_pIndices[ nIndex++ ] = (uint32)(nX + nY + nStart + 0 );
                m_pIndices[ nIndex++ ] = (uint32)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint32)(nX + nY + nStart + 1 + nTileDimensions );

                m_pIndices[ nIndex++ ] = (uint32)(nX + nY + nStart + 1 + nTileDimensions );
                m_pIndices[ nIndex++ ] = (uint32)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint32)(nX + nY + nStart + 1 + nTileDimensions + 1 );
            }
        }

        // Calculate the face normals
        for( int i = 0; i < nIndices; i += 3 )
        {   
            VPosNormalTex& v0 = pVertices[ m_pIndices[i + 0] ];
            VPosNormalTex& v1 = pVertices[ m_pIndices[i + 1] ];
            VPosNormalTex& v2 = pVertices[ m_pIndices[i + 2] ];

            v0.Pos = m_pVertexPositions[ m_pIndices[i + 0] ];
            v1.Pos = m_pVertexPositions[ m_pIndices[i + 1] ];
            v2.Pos = m_pVertexPositions[ m_pIndices[i + 2] ];

            v0.Normal = RVector3Zero();
            v1.Normal = RVector3Zero();
            v2.Normal = RVector3Zero();

            v0.TexCoord = RVector2( 0.0f, 0.0f );
            v1.TexCoord = RVector2( 0.0f, 0.0f );
            v2.TexCoord = RVector2( 0.0f, 0.0f );

            RVector3 s0 = v0.Pos - v1.Pos;
            RVector3 s1 = v1.Pos - v2.Pos;

            RVector3 norm = Normalize( CrossProduct( s0, s1 ) );

            v0.Normal += norm;
            v1.Normal += norm;
            v2.Normal += norm;
        }

        // Then normalize them
        for( uint i = 0; i < nVertsTotal; ++i )
        {
            pVertices[i].Normal = Normalize( pVertices[i].Normal );
        }

        //////////////////////////////////////////
        // Create our mesh
        m_pMesh = pRender->CreateMesh( VPosNormalTex::VertexStride, nVertsTotal, sizeof( uint32 ), nIndices, pVertices, m_pIndices );

        
        //////////////////////////////////////////
        // Load the texture
        SAFE_RELEASE( m_pTexture );
        m_pTexture = Engine::GetRenderer()->LoadTexture2D( "Assets/Textures/grass.png" );

        SAFE_DELETE_ARRAY( pData );
    }

    /*************************************************************************\
    \*************************************************************************/
    PerlinNoise::PerlinNoise()
    {
        persistence = 0;
        frequency = 0;
        amplitude  = 0;
        octaves = 0;
        randomseed = 0;
    }

    PerlinNoise::PerlinNoise(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed)
    {
        persistence = _persistence;
        frequency = _frequency;
        amplitude  = _amplitude;
        octaves = _octaves;
        randomseed = 2 + _randomseed * _randomseed;
    }

    void PerlinNoise::Set(float _persistence, float _frequency, float _amplitude, int _octaves, int _randomseed)
    {
        persistence = _persistence;
        frequency = _frequency;
        amplitude  = _amplitude;
        octaves = _octaves;
        randomseed = 2 + _randomseed * _randomseed;
    }

    float PerlinNoise::GetHeight(float x, float y) const
    {
        return amplitude * Total(x, y);
    }

    float PerlinNoise::Total(float i, float j) const
    {
        //properties of one octave (changing each loop)
        float t = 0.0f;
        float _amplitude = 1;
        float freq = frequency;

        for(int k = 0; k < octaves; k++) 
        {
            t += GetValue(j * freq + randomseed, i * freq + randomseed) * _amplitude;
            _amplitude *= persistence;
            freq *= 2;
        }

        return t;
    }

    float PerlinNoise::GetValue(float x, float y) const
    {
        int Xint = (int)x;
        int Yint = (int)y;
        float Xfrac = x - Xint;
        float Yfrac = y - Yint;

        //noise values
        float n01 = Noise(Xint-1, Yint-1);
        float n02 = Noise(Xint+1, Yint-1);
        float n03 = Noise(Xint-1, Yint+1);
        float n04 = Noise(Xint+1, Yint+1);
        float n05 = Noise(Xint-1, Yint);
        float n06 = Noise(Xint+1, Yint);
        float n07 = Noise(Xint, Yint-1);
        float n08 = Noise(Xint, Yint+1);
        float n09 = Noise(Xint, Yint);

        float n12 = Noise(Xint+2, Yint-1);
        float n14 = Noise(Xint+2, Yint+1);
        float n16 = Noise(Xint+2, Yint);

        float n23 = Noise(Xint-1, Yint+2);
        float n24 = Noise(Xint+1, Yint+2);
        float n28 = Noise(Xint, Yint+2);

        float n34 = Noise(Xint+2, Yint+2);

        //find the noise values of the four corners
        float x0y0 = 0.0625*(n01+n02+n03+n04) + 0.125*(n05+n06+n07+n08) + 0.25*(n09);  
        float x1y0 = 0.0625*(n07+n12+n08+n14) + 0.125*(n09+n16+n02+n04) + 0.25*(n06);  
        float x0y1 = 0.0625*(n05+n06+n23+n24) + 0.125*(n03+n04+n09+n28) + 0.25*(n08);  
        float x1y1 = 0.0625*(n09+n16+n28+n34) + 0.125*(n08+n14+n06+n24) + 0.25*(n04);  

        //interpolate between those values according to the x and y fractions
        float v1 = PerlinInterpolate(x0y0, x1y0, Xfrac); //interpolate in x direction (y)
        float v2 = PerlinInterpolate(x0y1, x1y1, Xfrac); //interpolate in x direction (y+1)
        float fin = PerlinInterpolate(v1, v2, Yfrac);  //interpolate in y direction

        return fin;
    }

    float PerlinNoise::PerlinInterpolate(float x, float y, float a) const
    {
        float negA = 1.0 - a;
        float negASqr = negA * negA;
        float fac1 = 3.0 * (negASqr) - 2.0 * (negASqr * negA);
        float aSqr = a * a;
        float fac2 = 3.0 * aSqr - 2.0 * (aSqr * a);

        //return x * fac1 + y * fac2; //add the weighted factors

        return CosInterpolate( x, y, a );
    }

    float PerlinNoise::Noise(int x, int y) const
    {
        int n = x + y * 57;
        n = (n << 13) ^ n;
        int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return 1.0 - float(t) * 0.931322574615478515625e-9;/// 1073741824.0);
    }

} // namespace Riot
