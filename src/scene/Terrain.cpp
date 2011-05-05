/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/4/2011 10:39:06 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"
#include "ComponentCollidable.h"

namespace Riot
{    
    static float fPersistance = 0.25f;
    static float fFrequency = 0.0625f / 2.0f;
    static float fAmplitude = 150.0f;
    static uint  nOctaves = 6;
    static uint  nSeed = 10000;//RandShort();

    // CTerrain constructor
    CTerrain::CTerrain()
        : m_PerlinDetail( fPersistance, fFrequency, 150.0f, nOctaves, nSeed )
        , m_PerlinShape( fPersistance, fFrequency, 150.0f, nOctaves, nSeed << 1 )
        , m_nNumTiles( 0 )
    {
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
            m_pTerrainTiles[i].Render();
        }
    }

    //-----------------------------------------------------------------------------
    //  GenerateTerrain
    //  Generates the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::GenerateTerrain( void )
    {
        GenerateTerrain( 0.0f, 0.0f );
    }

    CTerrainTile* CTerrain::GenerateTerrain( float fX, float fY )
    {
        // See if this tile already exists
        for( uint i = 0; i < m_nNumTiles; ++i )
        {
            if(    (fX > m_pTerrainTiles[i].m_fXPos - 64.0f)
                && (fY > m_pTerrainTiles[i].m_fYPos - 64.0f)
                && (fX < m_pTerrainTiles[i].m_fXPos + 64.0f)
                && (fY < m_pTerrainTiles[i].m_fYPos + 64.0f) )
            {
                return &m_pTerrainTiles[i];
            }
        }

        // It doesn't
        CTerrainTile* pTile = &m_pTerrainTiles[ m_nNumTiles++ ];

        fX = fX / 128.0f;
        fX = floorf( fX + 0.5f );
        fX = (float)((int)fX);
        fX *= 128.0f;
        fX -= 64.0f;

        
        fY = fY / 128.0f;
        fY = floorf( fY + 0.5f );
        fY = (float)((int)fY);
        fY *= 128.0f;
        fY -= 64.0f;

        pTile->m_fXPos = fX;
        pTile->m_fYPos = fY;
        
        sint nX;
        sint nY;
        for( fX = pTile->m_fXPos-64.0f, nX = 0; fX <= pTile->m_fXPos+64.0f; fX += 1.0f, ++nX  )
        {
            for( fY = pTile->m_fYPos-64.0f, nY = 0; fY <= pTile->m_fXPos+64.0f; fY += 1.0f, ++nY )
            {
                float fAbsoluteX = pTile->m_fXPos + fX;
                float fAbsoluteY = pTile->m_fYPos + fY;

                //pTile->m_fHeight[nX][nY] = m_PerlinDetail.GetHeight( fAbsoluteX, fAbsoluteY );// + m_PerlinShape.GetHeight( i, j );
                pTile->m_fHeight[nX][nY] = m_PerlinDetail.GetHeight( fAbsoluteX, fAbsoluteY );// + m_PerlinShape.GetHeight( i, j );

                VPosNormalTex vert = { RVector3( fX, m_PerlinDetail.GetHeight( fAbsoluteX, fAbsoluteY ), fY ), RVector3( 0.0f, 0.0f, 0.0f ), RVector2( 0.0f, 0.0f ) };
                pTile->m_pVertices[ nX + nY * TERRAIN_WIDTH ] = vert;
            }
        }

        pTile->CreateMesh();

        return pTile;
    }


    /*************************************************************************\
    \*************************************************************************/
    // CTerrainTile constructor
    CTerrainTile::CTerrainTile()
        : m_pTexture( NULL )
        , m_pMesh( NULL )
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

        for( int i = 0; i < 32*32; ++i )
        {
            pRender->DrawDebugRay( m_pVertices[i].Pos, m_pVertices[i].Normal );
        }
    }

    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates the terrain mesh
    //-----------------------------------------------------------------------------
    void CTerrainTile::CreateMesh( void )
    {
        SAFE_RELEASE( m_pMesh );

        static CRenderer* pRender = Engine::GetRenderer();

        // Create the vertices
        sint nVertex = 0;
        sint nIndex = 0;
        sint nX;
        sint nY;
        float fX;
        float fY;
        for( fX = m_fXPos-64.0f, nX = 0; fX <= m_fXPos+64.0f; fX += 1.0f, ++nX  )
        {
            for( fY = m_fYPos-64.0f, nY = 0; fY <= m_fXPos+64.0f; fY += 1.0f, ++nY )
            {
                VPosNormalTex vert = { RVector3( fX, m_fHeight[nX][nY], fY ), RVector3( 0.0f, 0.0f, 0.0f ), RVector2( 0.0f, 0.0f ) };
                m_pVertices[ nVertex++ ] = vert;
            }
        }

        // Index them
        for( nX = 0; nX < nPolysWidth; ++nX )
        {
            for( nY = 0; nY < nPolysHeight; ++nY )
            {
                uint nStart = nX * nPolysWidth;
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nPolysWidth );

                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nPolysWidth );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nPolysWidth + 1 );
            }
        }

        // Calculate the face normals
        for( int i = 0; i < nIndices; i += 3 )
        {
            VPosNormalTex& v0 = m_pVertices[ m_pIndices[i + 0] ];
            VPosNormalTex& v1 = m_pVertices[ m_pIndices[i + 1] ];
            VPosNormalTex& v2 = m_pVertices[ m_pIndices[i + 2] ];

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
            m_pVertices[i].Normal = Normalize( m_pVertices[i].Normal );
        }

        //////////////////////////////////////////
        // Create our mesh
        m_pMesh = pRender->CreateMesh( VPosNormalTex::VertexStride, nVertsTotal, 2, nIndices, m_pVertices, m_pIndices );

        
        //////////////////////////////////////////
        // Load the texture
        SAFE_RELEASE( m_pTexture );
        m_pTexture = Engine::GetRenderer()->LoadTexture2D( "Assets/Textures/grass.png" );

        //m_pMesh->m_nIndexCount /= 2;
        CComponentCollidable::SetTerrainData( m_pVertices, nVertsTotal, m_pIndices, nIndices );
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

    PerlinNoise::PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed)
    {
        persistence = _persistence;
        frequency = _frequency;
        amplitude  = _amplitude;
        octaves = _octaves;
        randomseed = 2 + _randomseed * _randomseed;
    }

    void PerlinNoise::Set(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed)
    {
        persistence = _persistence;
        frequency = _frequency;
        amplitude  = _amplitude;
        octaves = _octaves;
        randomseed = 2 + _randomseed * _randomseed;
    }

    double PerlinNoise::GetHeight(double x, double y) const
    {
        return amplitude * Total(x, y);
    }

    double PerlinNoise::Total(double i, double j) const
    {
        //properties of one octave (changing each loop)
        double t = 0.0f;
        double _amplitude = 1;
        double freq = frequency;

        for(int k = 0; k < octaves; k++) 
        {
            t += GetValue(j * freq + randomseed, i * freq + randomseed) * _amplitude;
            _amplitude *= persistence;
            freq *= 2;
        }

        return t;
    }

    double PerlinNoise::GetValue(double x, double y) const
    {
        int Xint = (int)x;
        int Yint = (int)y;
        double Xfrac = x - Xint;
        double Yfrac = y - Yint;

        //noise values
        double n01 = Noise(Xint-1, Yint-1);
        double n02 = Noise(Xint+1, Yint-1);
        double n03 = Noise(Xint-1, Yint+1);
        double n04 = Noise(Xint+1, Yint+1);
        double n05 = Noise(Xint-1, Yint);
        double n06 = Noise(Xint+1, Yint);
        double n07 = Noise(Xint, Yint-1);
        double n08 = Noise(Xint, Yint+1);
        double n09 = Noise(Xint, Yint);

        double n12 = Noise(Xint+2, Yint-1);
        double n14 = Noise(Xint+2, Yint+1);
        double n16 = Noise(Xint+2, Yint);

        double n23 = Noise(Xint-1, Yint+2);
        double n24 = Noise(Xint+1, Yint+2);
        double n28 = Noise(Xint, Yint+2);

        double n34 = Noise(Xint+2, Yint+2);

        //find the noise values of the four corners
        double x0y0 = 0.0625*(n01+n02+n03+n04) + 0.125*(n05+n06+n07+n08) + 0.25*(n09);  
        double x1y0 = 0.0625*(n07+n12+n08+n14) + 0.125*(n09+n16+n02+n04) + 0.25*(n06);  
        double x0y1 = 0.0625*(n05+n06+n23+n24) + 0.125*(n03+n04+n09+n28) + 0.25*(n08);  
        double x1y1 = 0.0625*(n09+n16+n28+n34) + 0.125*(n08+n14+n06+n24) + 0.25*(n04);  

        //interpolate between those values according to the x and y fractions
        double v1 = Interpolate(x0y0, x1y0, Xfrac); //interpolate in x direction (y)
        double v2 = Interpolate(x0y1, x1y1, Xfrac); //interpolate in x direction (y+1)
        double fin = Interpolate(v1, v2, Yfrac);  //interpolate in y direction

        return fin;
    }

    double PerlinNoise::Interpolate(double x, double y, double a) const
    {
        double negA = 1.0 - a;
        double negASqr = negA * negA;
        double fac1 = 3.0 * (negASqr) - 2.0 * (negASqr * negA);
        double aSqr = a * a;
        double fac2 = 3.0 * aSqr - 2.0 * (aSqr * a);

        return x * fac1 + y * fac2; //add the weighted factors

        return CosInterpolate( x, y, a );
    }

    double PerlinNoise::Noise(int x, int y) const
    {
        int n = x + y * 57;
        n = (n << 13) ^ n;
        int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
    }

} // namespace Riot
