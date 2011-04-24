/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/24/2011 1:13:34 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"

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

    //-----------------------------------------------------------------------------
    //  GenerateTerrain
    //  Generates the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::GenerateTerrain( void )
    {
        for( uint x = 0; x < TERRAIN_WIDTH+1; ++x )
        {
            for( uint y = 0; y < TERRAIN_HEIGHT+1; ++y )
            {
                //float angleX = DegToRad( ((float)x/TERRAIN_WIDTH) * 360.0f );
                //float angleY = DegToRad( ((float)y/TERRAIN_WIDTH) * 360.0f );
                //m_fHeight[x][y] = sinf( angleX ) * 3.0f + cosf( angleY ) * 3.0f + RandFloat(0.1f);
                //m_fHeight[x][y] = rand()/(float)RAND_MAX * 0.5f;

                float angleX = DegToRad( ((float)x/TERRAIN_WIDTH) * 360.0f );
                float angleY = DegToRad( ((float)y/TERRAIN_WIDTH) * 360.0f );
                m_fHeight[x][y] = sinf( angleX ) * 1.0f + cosf( angleY ) * 1.0f + RandFloat(0.1f);
            }
        }

        m_fHeight[0][0] = 10.0f;

        CreateMesh();

        SAFE_RELEASE( m_pTexture );
        m_pTexture = Engine::GetRenderer()->LoadTexture2D( "Assets/Textures/grass.png" );
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
        //m_pMesh = pRender->CreateMesh( sizeof( SimpleVertex ), nVertsTotal, 16, nIndices, vertices, indices );
        m_pMesh = pRender->CreateMesh( VPosNormalTex::VertexStride, nVertsTotal, 2, nIndices, m_pVertices, m_pIndices );
    }

} // namespace Riot
