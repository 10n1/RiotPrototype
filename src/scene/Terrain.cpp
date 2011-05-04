/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/28/2011 6:42:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"
#include "ComponentCollidable.h"

namespace Riot
{
    // CTerrain constructor
    CTerrain::CTerrain()
        : m_pMesh( NULL )
        , m_pTexture( NULL )
    {
    }

    // CTerrain destructor
    CTerrain::~CTerrain()
    {
        SAFE_RELEASE( m_pTexture );
        SAFE_RELEASE( m_pMesh );
    }


    //-----------------------------------------------------------------------------
    //  Render
    //  Renders the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::Render( void )
    {
        static CRenderer* pRender = Engine::GetRenderer();

        // Pass to the render engine
        TRenderCommand cmd = { m_pMesh, m_pTexture };
        RTransform t = RTransform();
        pRender->AddCommand( cmd, t );
    }

    void GenerateSmoothNoise( uint nWidth, uint nHeight, float* fBase, float* fBlend, uint nOctave )
    {
        sint    nSamplePeriod = 1 << nOctave;
        float   fSampleFrequency = 1.0f / nSamplePeriod;
            
        for( sint x = 0; x < nWidth; ++x )
        {
            // Calculate horizontal sampling
            sint nSamplex0 = (1 / nSamplePeriod) * nSamplePeriod;
            sint nSamplex1 = (nSamplex0 + nSamplePeriod) % nWidth;

            float fHorizBlend = (x - nSamplex0) * fSampleFrequency;

            for( sint y = 0; y < nHeight; ++y )
            {
                sint nSampley0 = (1 / nSamplePeriod) * nSamplePeriod;
                sint nSampley1 = (nSampley0 + nSamplePeriod) % nHeight;
                
                float fVertBlend = (y - nSampley0) * fSampleFrequency;

                float fTop = Interpolate( fBase[nSamplex0 + nSampley0*nWidth ], fBase[nSamplex1 + nSampley0*nWidth], fHorizBlend );
                float fBottom = Interpolate( fBase[nSamplex0 + nSampley1*nWidth], fBase[nSamplex1 + nSampley1*nWidth], fHorizBlend );

                fBlend[x + y*nWidth] = Interpolate( fTop, fBottom, fVertBlend );
            }
        }
    }

    void GenerateRandomNoise( uint nWidth, uint nHeight, float* f )
    {        
        for( uint x = 0; x < nWidth; ++x )
        {
            for( uint y = 0; y < nHeight; ++y )
            {
                f[x + y*nWidth]  = RandFloat( 1.0f );
            }
        }
    }

    //void PerlinNoise( void )
    //{
    //    const uint nWidth = 64;
    //    const uint nHeight = 64;
    //    const uint nOctaveCount = 6;

    //    float fPersistance = 0.5f;

    //    float fBase[nWidth][nHeight] = { 0.0f };

    //    float fSmooth[nOctaveCount][nWidth][nHeight] = { 0.0f };

    //    // Generate the base pass
    //    GenerateRandomNoise( nWidth, nHeight, &fBase[0][0] );

    //    // Generate each octave
    //    for( uint i = 0; i < nOctaveCount; ++i )
    //    {
    //        GenerateSmoothNoise( nWidth, nHeight, &fBase[0][0], &fSmooth[i][0][0], i );
    //    }

    //    // the final
    //    float fFinal[nWidth][nHeight] = { 0.0f };

    //    float fAmplitude = 1.0f;
    //    float fTotalAmplitude = 0.0f;
    //    float fWeight = 0.5f;

    //    for( uint i = 0; i < nOctaveCount; ++i )
    //    {
    //        fAmplitude *= fPersistance;
    //        fTotalAmplitude += fAmplitude;

    //        for( uint x = 0; x < nWidth; ++x )
    //        {
    //            for( uint y = 0; y < nHeight; ++y )
    //            {
    //                fFinal[x][y] += fSmooth[i][x][y] * fWeight;
    //            }
    //        }

    //        fWeight *= 0.5f;
    //    }

    //    // Normalize it
    //    //for( uint i = 0; i < nOctaveCount; ++i )
    //    //{
    //    //    
    //    //    for( uint x = 0; x < nWidth; ++x )
    //    //    {
    //    //        for( uint y = 0; y < nHeight; ++y )
    //    //        {
    //    //            fFinal[x][y] /= fTotalAmplitude;
    //    //        }
    //    //    }
    //    //}

    //    int x = 0;
    //}

    float GetNoise( float x, float y )
    {
        int n=(int)x+(int)y*57;
        n=(n<<13)^n;
        int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
        return 1.0f-((float)nn/1073741824.0f);
    }

    float Noise( float x, float y )
    {
        float fFloorX = (float)((int)x);
        float fFloorY = (float)((int)y);

        float s, t, u, v;
        s = GetNoise( fFloorX, fFloorY );
        t = GetNoise( fFloorX+1, fFloorY );
        u = GetNoise( fFloorX, fFloorY+1 );
        v = GetNoise( fFloorX+1, fFloorY+1 );

        float f1 = CosInterpolate( s, t, x - fFloorX );
        float f2 = CosInterpolate( u, v, x - fFloorX );

        return CosInterpolate( f1, f2, y - fFloorY );
    }

    //-----------------------------------------------------------------------------
    //  GenerateTerrain
    //  Generates the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::GenerateTerrain( void )
    {
        //PerlinNoise();
        //for( uint x = 0; x < TERRAIN_WIDTH+1; ++x )
        //{
        //    for( uint y = 0; y < TERRAIN_HEIGHT+1; ++y )
        //    {
        //        //float angleX = DegToRad( ((float)x/TERRAIN_WIDTH) * 360.0f );
        //        //float angleY = DegToRad( ((float)y/TERRAIN_WIDTH) * 360.0f );
        //        //m_fHeight[x][y] = sinf( angleX ) * 3.0f + cosf( angleY ) * 3.0f + RandFloat(0.1f);
        //        //m_fHeight[x][y] = rand()/(float)RAND_MAX * 0.5f;
        //
        //        //float angleX = DegToRad( ((float)x/TERRAIN_WIDTH) * 360.0f * 1.5f );
        //        //float angleY = DegToRad( ((float)y/TERRAIN_WIDTH) * 360.0f * 1.5f );
        //        //m_fHeight[x][y] = sinf( angleX ) * 6.0f + cosf( angleY ) * 6.0f + RandFloat(0.1f);
        //        m_fHeight[x][y] = RandFloat( 1.0f );
        //    }
        //}
        //m_fHeight[0][0] = 10.0f;
        
        const uint nWidth = TERRAIN_WIDTH+1;
        const uint nHeight = TERRAIN_HEIGHT+1;
        const uint nOctaveCount = 3;
        //
        float fPersistance = 0.5f;
        //
        //float fBase[nWidth][nHeight] = { 0.0f };
        //
        //float fSmooth[nOctaveCount][nWidth][nHeight] = { 0.0f };
        //
        //// Generate the base pass
        //GenerateRandomNoise( nWidth, nHeight, &fBase[0][0] );
        //
        //// Generate each octave
        //for( uint i = 0; i < nOctaveCount; ++i )
        //{
        //    GenerateSmoothNoise( nWidth, nHeight, &fBase[0][0], &fSmooth[i][0][0], i );
        //}
        //
        //// the final
        //float fFinal[nWidth][nHeight] = { 0.0f };
        //
        //float fAmplitude = 1.0f;
        //float fTotalAmplitude = 0.0f;
        //float fWeight = 0.5f;
        //
        //for( uint i = 0; i < nOctaveCount; ++i )
        //{
        //    fAmplitude *= fPersistance;
        //    fTotalAmplitude += fAmplitude;
        //
        //    for( uint x = 0; x < nWidth; ++x )
        //    {
        //        for( uint y = 0; y < nHeight; ++y )
        //        {
        //            m_fHeight[x][y] += fSmooth[i][x][y] * fWeight;
        //        }
        //    }
        //
        //    fWeight *= 0.5f;
        //}


        //fAmplitude *= fPersistance;
        //fTotalAmplitude += fAmplitude;
        PerlinNoise p( 0.5f, 0.0625f, 30.0f, 6, 100 );

        for( uint x = 0; x < nWidth; ++x )
        {
            for( uint y = 0; y < nHeight; ++y )
            {
                m_fHeight[x][y] = p.GetHeight( x, y );
            }
        }

        //fWeight *= 0.5f;
        

        //////////////////////////////////////////
        // Create a mesh from the new data
        CreateMesh();

        //////////////////////////////////////////
        // Load the texture
        SAFE_RELEASE( m_pTexture );
        m_pTexture = Engine::GetRenderer()->LoadTexture2D( "Assets/Textures/grass.png" );

        
        //////////////////////////////////////////
        // Notify the collidable about the new Terrain data
        CComponentCollidable::SetTerrainData( m_pVertices, nVertsTotal, m_pIndices, nIndices );
    }

    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates the terrain mesh
    //-----------------------------------------------------------------------------
    void CTerrain::CreateMesh( void )
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
        for( fX = -(nPolysWidth/2.0f), nX = 0; fX <= (nPolysWidth/2.0f); fX += 1.0f, ++nX  )
        {
            for( fY = -(nPolysHeight/2.0f), nY = 0; fY <= (nPolysHeight/2.0f); fY += 1.0f, ++nY )
            {
                VPosNormalTex vert = { RVector3( fX, m_fHeight[nX][nY], fY ), RVector3( 0.0f, 1.0f, 0.0f ) };
                m_pVertices[ nVertex++ ] = vert;
            }
        }

        nVertex = 0;
        for( fX = -(nPolysWidth/2.0f), nX = 0; fX <= (nPolysWidth/2.0f); fX += 1.0f, ++nX  )
        {
            for( fY = -(nPolysHeight/2.0f), nY = 0; fY <= (nPolysHeight/2.0f); fY += 1.0f, ++nY )
            {
                if( nX == 0 || nY == 0 || nX == TERRAIN_WIDTH || nY == TERRAIN_HEIGHT )
                {
                    // skip the edges
                    continue;
                }

                RVector3 top =      RVector3( fX,   m_fHeight[nX][nY+1], fY+1 );
                RVector3 bottom =   RVector3( fX,   m_fHeight[nX][nY-1], fY-1 );
                RVector3 left =     RVector3( fX-1, m_fHeight[nX-1][nY], fY   );
                RVector3 right =    RVector3( fX+1, m_fHeight[nX+1][nY], fY   );
                RVector3 me =       RVector3( fX,   m_fHeight[nX][nY],   fY   );

                RVector3 norm0 = CrossProduct( (top-me),    (me-left)    );
                RVector3 norm1 = CrossProduct( (right-me),  (me-top)     );
                RVector3 norm2 = CrossProduct( (bottom-me), (me-right)   );
                RVector3 norm3 = CrossProduct( (left-me),   (me-bottom)  );

                RVector3 avgNorm = norm0 + norm1 + norm2 + norm3;
                avgNorm = Normalize( avgNorm );

                m_pVertices[ nVertex++ ].Normal = avgNorm;
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

        // Create our mesh
        m_pMesh = pRender->CreateMesh( VPosNormalTex::VertexStride, nVertsTotal, 2, nIndices, m_pVertices, m_pIndices );
    }

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
    }

    double PerlinNoise::Noise(int x, int y) const
    {
        int n = x + y * 57;
        n = (n << 13) ^ n;
        int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
    }

} // namespace Riot
