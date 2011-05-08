/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/7/2011 5:20:36 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"
#include "ComponentCollidable.h"

namespace Riot
{    


    VPosNormalTex   CTerrainTile::m_pVertices[ nHighVertsTotal ] = { RVector3Zero(), RVector3Zero(), RVector2Zero() };

    uint16          CTerrainTile::m_pHighIndices[ nHighIndices ] = { 0 };
    uint16          CTerrainTile::m_pMedIndices[ nMedIndices ] = { 0 };
    uint16          CTerrainTile::m_pLowIndices[ nLowIndices ] = { 0 };
    IGfxBuffer*     CTerrainTile::m_pLowIndexBuffer = NULL;
    IGfxBuffer*     CTerrainTile::m_pMedIndexBuffer = NULL;
    IGfxBuffer*     CTerrainTile::m_pHighIndexBuffer = NULL;


    static float fPersistance = 0.5f;
    static float fFrequency = 0.0625f / 16.0f;
    static float fAmplitude = 150.0f;
    static uint  nOctaves = 6;
    static uint  nSeed = 100;//RandShort();

    // CTerrain constructor
    CTerrain::CTerrain()
        : m_PerlinDetail( fPersistance, fFrequency, 150.0f, nOctaves, nSeed )
        , m_PerlinShape( fPersistance, fFrequency, 150.0f, nOctaves, nSeed << 1 )
        , m_fCurrX( 0.0f )
        , m_fCurrY( 0.0f )
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
        static CRenderer*       pRenderer   = Engine::GetRenderer();
        static IGraphicsDevice* pDevice     = pRenderer->GetGraphicsDevice();

        pRenderer->SetWorldMatrix( RMatrix4Identity() );

        // First all low
        pDevice->SetIndexBuffer( CTerrainTile::m_pLowIndexBuffer, sizeof( uint16 ) );
        for( uint i = 0; i < nNumLowTiles; ++i )
        {
            pDevice->SetVertexBuffer( m_pLowTiles[i].m_pVertexBuffer, VPosNormalTex::VertexStride );
            pDevice->DrawIndexedPrimitive( CTerrainTile::nLowIndices );
        }

        // Medium
        pDevice->SetIndexBuffer( CTerrainTile::m_pMedIndexBuffer, sizeof( uint16 ) );
        for( uint i = 0; i < nNumMedTiles; ++i )
        {
            pDevice->SetVertexBuffer( m_pMedTiles[i].m_pVertexBuffer, VPosNormalTex::VertexStride );
            pDevice->DrawIndexedPrimitive( CTerrainTile::nMedIndices );
        }

        // High
        pDevice->SetIndexBuffer( CTerrainTile::m_pHighIndexBuffer, sizeof( uint16 ) );
        for( uint i = 0; i < nNumHighTiles; ++i )
        {
            pDevice->SetVertexBuffer( m_pHighTiles[i].m_pVertexBuffer, VPosNormalTex::VertexStride );
            pDevice->DrawIndexedPrimitive( CTerrainTile::nHighIndices );
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
        static CRenderer*       pRenderer   = Engine::GetRenderer();
        static IGraphicsDevice* pDevice     = pRenderer->GetGraphicsDevice();

        //////////////////////////////////////////
        // Index buffers first

        //////////////////////////////////////////
        // Low
        sint nIndex = 0;
        sint nX;
        sint nY;
        for( nX = 0; nX < CTerrainTile::nLowTileDimensions; ++nX )
        {
            for( nY = 0; nY < CTerrainTile::nLowTileDimensions; ++nY )
            {
                uint nStart = nX * CTerrainTile::nLowTileDimensions;
                
                CTerrainTile::m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                CTerrainTile::m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                CTerrainTile::m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nLowTileDimensions );
                CTerrainTile::m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nLowTileDimensions );
                CTerrainTile::m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                CTerrainTile::m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nLowTileDimensions + 1 );
            }
        }
        CTerrainTile::m_pLowIndexBuffer = pDevice->CreateIndexBuffer( CTerrainTile::nLowIndices, CTerrainTile::m_pLowIndices );
        
        //////////////////////////////////////////
        // Med
        nIndex = 0;
        for( nX = 0; nX < CTerrainTile::nMedTileDimensions; ++nX )
        {
            for( nY = 0; nY < CTerrainTile::nMedTileDimensions; ++nY )
            {
                uint nStart = nX * CTerrainTile::nMedTileDimensions;
                
                CTerrainTile::m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                CTerrainTile::m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                CTerrainTile::m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nMedTileDimensions );
                CTerrainTile::m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nMedTileDimensions );
                CTerrainTile::m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                CTerrainTile::m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nMedTileDimensions + 1 );
            }
        }
        CTerrainTile::m_pMedIndexBuffer = pDevice->CreateIndexBuffer( CTerrainTile::nMedIndices, CTerrainTile::m_pMedIndices );
        
        //////////////////////////////////////////
        // High
        nIndex = 0;
        for( nX = 0; nX < CTerrainTile::nHighTileDimensions; ++nX )
        {
            for( nY = 0; nY < CTerrainTile::nHighTileDimensions; ++nY )
            {
                uint nStart = nX * CTerrainTile::nHighTileDimensions;
                
                CTerrainTile::m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                CTerrainTile::m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                CTerrainTile::m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nHighTileDimensions );
                CTerrainTile::m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nHighTileDimensions );
                CTerrainTile::m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                CTerrainTile::m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + CTerrainTile::nHighTileDimensions + 1 );
            }
        }
        CTerrainTile::m_pHighIndexBuffer = pDevice->CreateIndexBuffer( CTerrainTile::nHighIndices, CTerrainTile::m_pHighIndices );

        //////////////////////////////////////////
        // Low vertex buffers
        for( uint i = 0; i < nNumLowTiles; ++i )
        {
            m_pLowTiles[i].m_pVertexBuffer = pDevice->CreateVertexBuffer( VPosNormalTex::VertexStride*CTerrainTile::nLowVertsTotal, CTerrainTile::m_pVertices );
            m_pLowTiles[i].m_pTexture = pRenderer->LoadTexture2D( "Assets/Textures/grass.png" );
        }
        
        //////////////////////////////////////////
        // Med vertex buffers
        for( uint i = 0; i < nNumMedTiles; ++i )
        {
            m_pMedTiles[i].m_pVertexBuffer = pDevice->CreateVertexBuffer( VPosNormalTex::VertexStride*CTerrainTile::nMedVertsTotal, CTerrainTile::m_pVertices );
            m_pMedTiles[i].m_pTexture = pRenderer->LoadTexture2D( "Assets/Textures/grass.png" );
        }
        
        //////////////////////////////////////////
        // High vertex buffers
        for( uint i = 0; i < nNumHighTiles; ++i )
        {
            m_pHighTiles[i].m_pVertexBuffer = pDevice->CreateVertexBuffer( VPosNormalTex::VertexStride*CTerrainTile::nHighVertsTotal, CTerrainTile::m_pVertices );
            m_pHighTiles[i].m_pTexture = pRenderer->LoadTexture2D( "Assets/Textures/grass.png" );
        }
    }

    //-----------------------------------------------------------------------------
    //  BuildTile
    //  Converts tile index nIndex into the tile with the specified center
    //-----------------------------------------------------------------------------
    void CTerrain::BuildLowTile( sint nIndex, float fX, float fY )
    {
        CTerrainTile* pTile = &m_pLowTiles[ nIndex ];

        pTile->m_fXPos = fX;
        pTile->m_fYPos = fY;
        
        // Calculate the vertex data
        sint nX;
        sint nY;
        sint nVertex = 0;
        for( fX = pTile->m_fXPos - fTileHalfDimensions, nX = 0; fX <= pTile->m_fXPos + fTileHalfDimensions; fX += nLowGranularity, ++nX  )
        {
            for( fY = pTile->m_fYPos - fTileHalfDimensions, nY = 0; fY <= pTile->m_fYPos + fTileHalfDimensions; fY += nLowGranularity, ++nY )
            {
                //pTile->m_pVertexPositions[ nVertex++ ] = RVector3( fX, m_PerlinDetail.GetHeight( fX, fY ), fY );
                VPosNormalTex vert = { RVector3( fX, 0.0f, fY ), RVector3Zero(), RVector2Zero() };
                CTerrainTile::m_pVertices[ nVertex++ ] = vert;
            }
        }
        
        // Calculate the face normals
        for( int i = 0; i < nLowIndices; i += 3 )
        {   
            VPosNormalTex& v0 = CTerrainTile::m_pVertices[ m_pLowIndices[i + 0] ];
            VPosNormalTex& v1 = CTerrainTile::m_pVertices[ m_pLowIndices[i + 1] ];
            VPosNormalTex& v2 = CTerrainTile::m_pVertices[ m_pLowIndices[i + 2] ];

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

        // Now update the mesh
        Engine::GetRenderer()->UpdateMesh( pTile->m_pMesh, CTerrainTile::m_pVertices );
    }

    void CTerrain::BuildMedTile( sint nIndex, float fX, float fY )
    {
        CTerrainTile* pTile = &m_pMedTiles[ nIndex ];

        pTile->m_fXPos = fX;
        pTile->m_fYPos = fY;
        
        // Calculate the vertex data
        sint nX;
        sint nY;
        sint nVertex = 0;
        for( fX = pTile->m_fXPos - fTileHalfDimensions, nX = 0; fX <= pTile->m_fXPos + fTileHalfDimensions; fX += nMedGranularity, ++nX  )
        {
            for( fY = pTile->m_fYPos - fTileHalfDimensions, nY = 0; fY <= pTile->m_fYPos + fTileHalfDimensions; fY += nMedGranularity, ++nY )
            {
                //pTile->m_pVertexPositions[ nVertex++ ] = RVector3( fX, m_PerlinDetail.GetHeight( fX, fY ), fY );
                VPosNormalTex vert = { RVector3( fX, 0.0f, fY ), RVector3Zero(), RVector2Zero() };
                CTerrainTile::m_pVertices[ nVertex++ ] = vert;
            }
        }
        
        // Calculate the face normals
        for( int i = 0; i < nMedIndices; i += 3 )
        {   
            VPosNormalTex& v0 = CTerrainTile::m_pVertices[ m_pMedIndices[i + 0] ];
            VPosNormalTex& v1 = CTerrainTile::m_pVertices[ m_pMedIndices[i + 1] ];
            VPosNormalTex& v2 = CTerrainTile::m_pVertices[ m_pMedIndices[i + 2] ];

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

        // Now update the mesh
        Engine::GetRenderer()->UpdateMesh( pTile->m_pMesh, CTerrainTile::m_pVertices );
    }

    void CTerrain::BuildHighTile( sint nIndex, float fX, float fY )
    {
        CTerrainTile* pTile = &m_pHighTiles[ nIndex ];

        pTile->m_fXPos = fX;
        pTile->m_fYPos = fY;
        
        // Calculate the vertex data
        sint nX;
        sint nY;
        sint nVertex = 0;
        for( fX = pTile->m_fXPos - fTileHalfDimensions, nX = 0; fX <= pTile->m_fXPos + fTileHalfDimensions; fX += nHighGranularity, ++nX  )
        {
            for( fY = pTile->m_fYPos - fTileHalfDimensions, nY = 0; fY <= pTile->m_fYPos + fTileHalfDimensions; fY += nHighGranularity, ++nY )
            {
                //pTile->m_pVertexPositions[ nVertex++ ] = RVector3( fX, m_PerlinDetail.GetHeight( fX, fY ), fY );
                VPosNormalTex vert = { RVector3( fX, 0.0f, fY ), RVector3Zero(), RVector2Zero() };
                CTerrainTile::m_pVertices[ nVertex++ ] = vert;
            }
        }
        
        // Calculate the face normals
        for( int i = 0; i < nHighIndices; i += 3 )
        {   
            VPosNormalTex& v0 = CTerrainTile::m_pVertices[ m_pHighIndices[i + 0] ];
            VPosNormalTex& v1 = CTerrainTile::m_pVertices[ m_pHighIndices[i + 1] ];
            VPosNormalTex& v2 = CTerrainTile::m_pVertices[ m_pHighIndices[i + 2] ];

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

        // Now update the mesh
        Engine::GetRenderer()->UpdateMesh( pTile->m_pMesh, CTerrainTile::m_pVertices );
    }

    //-----------------------------------------------------------------------------
    //  CenterTerrain
    //  Centers the active portion of the terrain around a point
    //-----------------------------------------------------------------------------
    void CTerrain::CenterTerrain( const RVector3& pos, float fRadius )
    {
        // First see if we even have updating to do
        float fX = pos.x;
        float fY = pos.z;

        CalculateTileCenter( fX, fY );

        if(    fX == m_fCurrX
            && fY == m_fCurrY )
        {
            // We're still centered around the same tile, no updating required
            return;
        }

        // Calculate which direction we went in
        float fXDir = 0.0f;
        float fYDir = 0.0f;

        if( fX > m_fCurrX )
            fXDir = 1.0f;
        else if( fX < m_fCurrX )
            fXDir = -1.0f;

        if( fY > m_fCurrY )
            fYDir = 1.0f;
        else if( fY < m_fCurrY )
            fYDir = -1.0f;


        // Theres updating to do. Worst case is we just exactly crossed a corner and have a lot of tiles to update
        sint nLowNeedsUpdating[ nNumLowTiles ];
        sint nMedNeedsUpdating[ nNumMedTiles ];
        sint nHighNeedsUpdating[ nNumHighTiles ];

        sint nNumLow = 0;
        sint nNumMed = 0;
        sint nNumHigh = 0;


        float fTileDimensions = (float)CTerrainTile::nTileDimensions;
        float fTileHalfDimensions = (float)CTerrainTile::nTileHalfDimensions;

        //////////////////////////////////////////
        // Low
        float fMinX = fX - ( fTileDimensions * nTerrainTileDimensionsPerSide );
        float fMaxX = fX + ( fTileDimensions * nTerrainTileDimensionsPerSide );        
        float fMinY = fY - ( fTileDimensions * nTerrainTileDimensionsPerSide );
        float fMaxY = fY + ( fTileDimensions * nTerrainTileDimensionsPerSide );

        // First figure out which low needs updating
        for( sint i = 0; i < nNumLowTiles; ++i )
        {
            if(     m_pLowTiles[i].m_fXPos < fMinX
                ||  m_pLowTiles[i].m_fXPos > fMaxX
                ||  m_pLowTiles[i].m_fYPos < fMinY
                ||  m_pLowTiles[i].m_fYPos > fMaxY )
            {
                // This tile is outside the bounds
                nLowNeedsUpdating[ nNumLow++ ] = i;
            }
        }

        // Now build our new X tiles
        if( fXDir < 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMinX;
            for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        else if( fXDir > 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMaxX;
            for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        // Then the Y
        if( fYDir < 0.0f )
        {
            sint nIndex = 0;
            float fNewY = fMinY;
            for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        else if( fYDir > 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMaxY;
            for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }

        //////////////////////////////////////////
        // Medium
        fMinX = fX - ( fTileDimensions * 2 );
        fMaxX = fX + ( fTileDimensions * 2 );
        fMinY = fY - ( fTileDimensions * 2 );
        fMaxY = fY + ( fTileDimensions * 2 );
        for( sint i = 0; i < nNumMedTiles; ++i )
        {
            if(     m_pMedTiles[i].m_fXPos < fMinX
                ||  m_pMedTiles[i].m_fXPos > fMaxX
                ||  m_pMedTiles[i].m_fYPos < fMinY
                ||  m_pMedTiles[i].m_fYPos > fMaxY )
            {
                // This tile is outside the bounds
                nMedNeedsUpdating[ nNumMed++ ] = i;
            }
        }
        // Now build our new X tiles
        if( fXDir < 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMinX;
            for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
            {
                BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        else if( fXDir > 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMaxX;
            for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
            {
                BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        // Then the Y
        if( fYDir < 0.0f )
        {
            sint nIndex = 0;
            float fNewY = fMinY;
            for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
            {
                BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        else if( fYDir > 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMaxY;
            for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
            {
                BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }

        //////////////////////////////////////////
        // High
        fMinX = fX - ( fTileDimensions * 1 );
        fMaxX = fX + ( fTileDimensions * 1 );
        fMinY = fY - ( fTileDimensions * 1 );
        fMaxY = fY + ( fTileDimensions * 1 );
        for( sint i = 0; i < nNumHighTiles; ++i )
        {
            if(     m_pHighTiles[i].m_fXPos < fMinX
                ||  m_pHighTiles[i].m_fXPos > fMaxX
                ||  m_pHighTiles[i].m_fYPos < fMinY
                ||  m_pHighTiles[i].m_fYPos > fMaxY )
            {
                // This tile is outside the bounds
                nHighNeedsUpdating[ nNumHigh++ ] = i;
            }
        }
        // Now build our new X tiles
        if( fXDir < 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMinX;
            for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        else if( fXDir > 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMaxX;
            for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        // Then the Y
        if( fYDir < 0.0f )
        {
            sint nIndex = 0;
            float fNewY = fMinY;
            for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], fNewX, fNewY );
            }
        }
        else if( fYDir > 0.0f )
        {
            sint nIndex = 0;
            float fNewX = fMaxY;
            for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], fNewX, fNewY );
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
    //  CreateMesh
    //  Creates the terrain mesh
    //-----------------------------------------------------------------------------
    void CTerrainTile::CreateMesh( void )
    {
        SAFE_RELEASE( m_pMesh );

        static CRenderer* pRender = Engine::GetRenderer();

        sint nSize = sizeof( VPosNormalTex ) * nHighVertsTotal;

        byte* pData = new byte[ sizeof( VPosNormalTex ) * nHighVertsTotal];

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
                
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nTileDimensions );

                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nTileDimensions );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nTileDimensions + 1 );
            }
        }

        // Calculate the face normals
        for( int i = 0; i < nHighIndices; i += 3 )
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
        for( uint i = 0; i < nHighVertsTotal; ++i )
        {
            pVertices[i].Normal = Normalize( pVertices[i].Normal );
        }

        //////////////////////////////////////////
        // Create our mesh
        m_pMesh = pRender->CreateMesh( VPosNormalTex::VertexStride, nHighVertsTotal, sizeof( uint16 ), nHighIndices, pVertices, m_pIndices );

        
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
