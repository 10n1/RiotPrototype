/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       4/6/2011 10:49:19 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Riot.h"
#include "Gfx\Graphics.h"
#include "Gfx\Mesh.h"
#include "Gfx\Material.h"
#include <stdlib.h>

// CTerrain constructor
CTerrain::CTerrain()
    : m_pMesh( NULL )
    , m_pMaterial( NULL )
{
}

// CTerrain destructor
CTerrain::~CTerrain()
{
    SAFE_RELEASE( m_pMesh );
    SAFE_RELEASE( m_pMaterial );
}


//-----------------------------------------------------------------------------
//  Render
//  Renders the terrain
//-----------------------------------------------------------------------------
void CTerrain::Render( void )
{
    static CGraphics* pGfx = Riot::GetGraphics();
    static XMMATRIX mWorld = XMMatrixIdentity();

    // Build the command and matrix
    CRenderCommand command = { m_pMaterial, m_pMesh };

    // Pass to the render engine
    pGfx->AddCommand( command );
    pGfx->AddMatrix( &mWorld );
}

//-----------------------------------------------------------------------------
//  GenerateTerrain
//  Generates the terrain
//-----------------------------------------------------------------------------
void CTerrain::GenerateTerrain( void )
{
    for( uint x = 0; x < TERRAIN_WIDTH; ++x )
    {
        for( uint y = 0; y < TERRAIN_HEIGHT; ++y )
        {
            m_fHeight[x][y] = rand()/(float)RAND_MAX;
        }
    }
}

//-----------------------------------------------------------------------------
//  CreateMesh
//  Creates the terrain mesh
//-----------------------------------------------------------------------------
void CTerrain::CreateMesh( void )
{
    CGraphics* pGfx = Riot::GetGraphics();

    uint nPolysWidth = TERRAIN_WIDTH;
    uint nPolysHeight = TERRAIN_HEIGHT;
    uint nPolysTotal = nPolysWidth * nPolysHeight;

    // Initalize the buffers
    uint nVertsTotal = (nPolysWidth+1) * (nPolysHeight+1);
    SimpleVertex* vertices = new SimpleVertex[ nVertsTotal ];
    uint nIndices = nPolysTotal * 6;
    WORD* indices = new WORD[ nIndices ];

    // Create the vertices
    uint nVertex = 0;
    uint nX;
    uint nY;
    float fX;
    float fY;
    for( fX = -(nPolysWidth/2.0f), nX = 0; fX <= (nPolysWidth/2.0f); fX += 1.0f, ++nX  )
    {
        for( fY = -(nPolysHeight/2.0f), nY = 0; fY <= (nPolysHeight/2.0f); fY += 1.0f, ++nY )
        {
            SimpleVertex vert = { XMFLOAT3( fX, m_fHeight[nX][nY], fY ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) };
            vertices[ nVertex++ ] = vert;
        }
    }

    // Index them
    uint nIndex = 0;
    for( nX = 0; nX < nPolysWidth; ++nX )
    {
        for( nY = 0; nY < nPolysHeight; ++nY )
        {
            uint nStart = nX * nPolysWidth;
            indices[ nIndex++ ] = nX + nY + nStart + 0;
            indices[ nIndex++ ] = nX + nY + nStart + 1;
            indices[ nIndex++ ] = nX + nY + nStart + 1 + nPolysWidth;

            indices[ nIndex++ ] = nX + nY + nStart + 1 + nPolysWidth;
            indices[ nIndex++ ] = nX + nY + nStart + 1;
            indices[ nIndex++ ] = nX + nY + nStart + 1 + nPolysWidth + 1;
        }
    }

    // Create our mesh
    m_pMesh = pGfx->CreateMesh( sizeof( SimpleVertex ), nVertsTotal, 16, nIndices, vertices, indices );
    m_pMaterial = pGfx->GetMaterial( eMaterialStandard );

    // Release the memory
    SAFE_DELETE_ARRAY( vertices );
    SAFE_DELETE_ARRAY( indices );
}
