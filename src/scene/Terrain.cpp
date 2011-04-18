/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/17/2011 5:34:59 PM
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
    {
    }

    // CTerrain destructor
    CTerrain::~CTerrain()
    {
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
        pRender->AddCommand( m_pMesh, RTransform() );
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
                float angleX = DegToRad( ((float)x/TERRAIN_WIDTH) * 360.0f );
                float angleY = DegToRad( ((float)y/TERRAIN_WIDTH) * 360.0f );
                m_fHeight[x][y] = sinf( angleX ) * 3.0f + cosf( angleY ) * 3.0f + RandFloat(0.5f);
                //m_fHeight[x][y] = rand()/(float)RAND_MAX * 0.5f;
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates the terrain mesh
    //-----------------------------------------------------------------------------
    void CTerrain::CreateMesh( void )
    {
        SAFE_RELEASE( m_pMesh );

        static CRenderer* pRender = Engine::GetRenderer();

        uint nPolysWidth = TERRAIN_WIDTH;
        uint nPolysHeight = TERRAIN_HEIGHT;
        uint nPolysTotal = nPolysWidth * nPolysHeight;

        // Initalize the buffers
        uint nVertsTotal = nPolysTotal * 6;
        VPosNormal* vertices = new VPosNormal[ nVertsTotal ];
        uint nIndices = nPolysTotal * 6;
        uint16* indices = new uint16[ nIndices ];

        // Create the vertices
        uint nVertex = 0;
        uint nIndex = 0;
        uint nX;
        uint nY;
        float fX;
        float fY;

        nVertex = 0;
        nIndex = 0;
        for( fX = -(nPolysWidth/2.0f), nX = 0; nX < nPolysWidth; fX += 1.0f, ++nX )
        {
            for( fY = -(nPolysHeight/2.0f), nY = 0; nY < nPolysHeight; fY += 1.0f, ++nY )
            {
                RVector3 s1;
                RVector3 s2;
                RVector3 norm;

                // Make the first triangles vertices
                uint nStart = nX * nPolysWidth;
                VPosNormal vert0 = { RVector3( fX,      m_fHeight[nX][nY],   fY      ), RVector3( 0.0f, 1.0f, 0.0f ) };
                VPosNormal vert1 = { RVector3( fX,      m_fHeight[nX][nY+1], fY+1.0f ), RVector3( 0.0f, 1.0f, 0.0f ) };
                VPosNormal vert2 = { RVector3( fX+1.0f, m_fHeight[nX+1][nY], fY      ), RVector3( 0.0f, 1.0f, 0.0f ) };

                // Calculate its normal
                s1 = vert0.Pos - vert1.Pos;
                s2 = vert1.Pos - vert2.Pos;
                //v1 = XMLoadFloat3( &s1 );
                //v2 = XMLoadFloat3( &s2 );
                //XMStoreFloat3( &norm, XMVector3Normalize( XMVector3Cross( v1, v2 ) ) );
                norm = Normalize( CrossProduct( s1, s2 ) );

                // Set the normal
                vert0.Normal = norm;
                vert1.Normal = norm;
                vert2.Normal = norm;

                // Now add the triangle to the buffer
                vertices[ nVertex++ ] = vert0;
                vertices[ nVertex++ ] = vert1;
                vertices[ nVertex++ ] = vert2;

                // Finish the last triangle
                VPosNormal vert3 = { RVector3( fX+1.0f, m_fHeight[nX+1][nY+1], fY+1.0f ), RVector3( 0.0f, 1.0f, 0.0f ) };   

                // Calculate its normal
                s1 = vert1.Pos - vert3.Pos;
                //XMStoreFloat3( &norm, XMVector3Normalize( XMVector3Cross( v1, v2 ) ) );
                norm = Normalize( CrossProduct( s1, s2 ) );
                // Set the normal
                vert3.Normal = norm;

                // Now add the triangle to the buffer
                vertices[ nVertex++ ] = vert3;

                // Now index that bitch
                indices[ nIndex++ ] = nVertex-1;
                indices[ nIndex++ ] = nVertex-2;
                indices[ nIndex++ ] = nVertex-3;

                indices[ nIndex++ ] = nVertex-2;
                indices[ nIndex++ ] = nVertex-4;
                indices[ nIndex++ ] = nVertex-3;
            }
        }

        // Create our mesh
        //m_pMesh = pRender->CreateMesh( sizeof( SimpleVertex ), nVertsTotal, 16, nIndices, vertices, indices );
        m_pMesh = pRender->CreateMesh( VPosNormal::VertexStride, nVertsTotal, 2, nIndices, vertices, indices );

        // Release the memory
        SAFE_DELETE_ARRAY( vertices );
        SAFE_DELETE_ARRAY( indices );
    }

} // namespace Riot
