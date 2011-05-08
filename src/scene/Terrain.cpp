/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/7/2011 6:30:20 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Terrain.h"
#include "Engine.h"
#include "Renderer.h"
#include "Mesh.h"
#include "ComponentCollidable.h"

namespace Riot
{    


    VPosNormalTex   CTerrain::m_pVertices[ nHighVertsTotal ] = { RVector3Zero(), RVector3Zero(), RVector2Zero() };

    uint16          CTerrain::m_pHighIndices[ nHighIndices ] = { 0 };
    uint16          CTerrain::m_pMedIndices[ nMedIndices ] = { 0 };
    uint16          CTerrain::m_pLowIndices[ nLowIndices ] = { 0 };
    IGfxBuffer*     CTerrain::m_pLowIndexBuffer = NULL;
    IGfxBuffer*     CTerrain::m_pMedIndexBuffer = NULL;
    IGfxBuffer*     CTerrain::m_pHighIndexBuffer = NULL;


    static float fPersistance = 0.5f;
    static float fFrequency = 0.0625f / 16.0f;
    static float fAmplitude = 150.0f;
    static uint  nOctaves = 6;
    static uint  nSeed = 100;//RandShort();

    // CTerrain constructor
    CTerrain::CTerrain()
        : m_PerlinDetail( fPersistance, fFrequency, 150.0f, nOctaves, nSeed )
        , m_PerlinShape( fPersistance, fFrequency, 150.0f, nOctaves, nSeed << 1 )
        , m_nCurrX( 0 )
        , m_nCurrY( 0 )
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
        pDevice->SetPSTexture( 0, m_pTexture );

        // First all low
        pDevice->SetIndexBuffer( m_pLowIndexBuffer, sizeof( uint16 ) );
        for( uint i = 0; i < nNumLowTiles; ++i )
        {
            pDevice->SetVertexBuffer( m_pLowTiles[i].m_pVertexBuffer, VPosNormalTex::VertexStride );
            pDevice->DrawIndexedPrimitive( nLowIndices );
        }
        
        // Medium
        pDevice->SetIndexBuffer( m_pMedIndexBuffer, sizeof( uint16 ) );
        for( uint i = 0; i < nNumMedTiles; ++i )
        {
            pDevice->SetVertexBuffer( m_pMedTiles[i].m_pVertexBuffer, VPosNormalTex::VertexStride );
            pDevice->DrawIndexedPrimitive( nMedIndices );
        }

        // High
        pDevice->SetIndexBuffer( m_pHighIndexBuffer, sizeof( uint16 ) );
        for( uint i = 0; i < nNumHighTiles; ++i )
        {
            pDevice->SetVertexBuffer( m_pHighTiles[i].m_pVertexBuffer, VPosNormalTex::VertexStride );
            pDevice->DrawIndexedPrimitive( nHighIndices );
        }

    }

    //-----------------------------------------------------------------------------
    //  CalculateTileCenter
    //  Calculates a tiles center
    //-----------------------------------------------------------------------------
    void CTerrain::CalculateTileCenter( float fX, float fY, sint& nX, sint& nY )
    {
        float fTileDimensions = (float)nTileDimensions;
        float fTileHalfDimensions = (float)nTileHalfDimensions;

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

        nX = fX;

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

        nY = fY;
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
        for( nX = 0; nX < nLowTileDimensions; ++nX )
        {
            for( nY = 0; nY < nLowTileDimensions; ++nY )
            {
                uint nStart = nX * nLowTileDimensions;

                m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nLowTileDimensions );
                m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nLowTileDimensions );
                m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pLowIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nLowTileDimensions + 1 );
            }
        }
        m_pLowIndexBuffer = pDevice->CreateIndexBuffer( nLowIndices * sizeof(uint16), m_pLowIndices );

        //////////////////////////////////////////
        // Med
        nIndex = 0;
        for( nX = 0; nX < nMedTileDimensions; ++nX )
        {
            for( nY = 0; nY < nMedTileDimensions; ++nY )
            {
                uint nStart = nX * nMedTileDimensions;

                m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nMedTileDimensions );
                m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nMedTileDimensions );
                m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pMedIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nMedTileDimensions + 1 );
            }
        }
        m_pMedIndexBuffer = pDevice->CreateIndexBuffer( nMedIndices * sizeof(uint16), m_pMedIndices );

        //////////////////////////////////////////
        // High
        nIndex = 0;
        for( nX = 0; nX < nHighTileDimensions; ++nX )
        {
            for( nY = 0; nY < nHighTileDimensions; ++nY )
            {
                uint nStart = nX * nHighTileDimensions;

                m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nHighTileDimensions );
                m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nHighTileDimensions );
                m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pHighIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nHighTileDimensions + 1 );
            }
        }
        m_pHighIndexBuffer = pDevice->CreateIndexBuffer( nHighIndices * sizeof(uint16), m_pHighIndices );

        //////////////////////////////////////////
        // Low vertex buffers
        for( uint i = 0; i < nNumLowTiles; ++i )
        {
            m_pLowTiles[i].m_pVertexBuffer = pDevice->CreateVertexBuffer( VPosNormalTex::VertexStride*nLowVertsTotal, m_pVertices );
        }

        //////////////////////////////////////////
        // Med vertex buffers
        for( uint i = 0; i < nNumMedTiles; ++i )
        {
            m_pMedTiles[i].m_pVertexBuffer = pDevice->CreateVertexBuffer( VPosNormalTex::VertexStride*nMedVertsTotal, m_pVertices );
        }

        //////////////////////////////////////////
        // High vertex buffers
        for( uint i = 0; i < nNumHighTiles; ++i )
        {
            m_pHighTiles[i].m_pVertexBuffer = pDevice->CreateVertexBuffer( VPosNormalTex::VertexStride*nHighVertsTotal, m_pVertices );
        }
        
        // Load the terrain texture
        m_pTexture = pDevice->LoadTexture( "Assets/Textures/grass.png" );

        //////////////////////////////////////////
        // Initialize them
        sint nCenterX = 0.0f;
        sint nCenterY = 0.0f;

        sint nHigh = 0;
        sint nMed = 0;
        sint nLow = 0;

        CalculateTileCenter( 0.0f, 0.0f, nCenterX, nCenterY );

        for( sint nX = nCenterX - (nTerrainTileDimensionsPerSide*nTileDimensions); nX <= nCenterX + (nTerrainTileDimensionsPerSide*nTileDimensions); nX += nTileDimensions )
        {
            for( sint nY = nCenterY - (nTerrainTileDimensionsPerSide*nTileDimensions); nY <= nCenterY + (nTerrainTileDimensionsPerSide*nTileDimensions); nY += nTileDimensions )
            {
                if( nX == 64 )
                {
                    int x = 0;
                }
                if(    nX >= (nCenterX - nTileDimensions)
                    && nY >= (nCenterY - nTileDimensions)
                    && nX <= (nCenterX + nTileDimensions)
                    && nY <= (nCenterY + nTileDimensions) )
                {
                    BuildHighTile( nHigh++, nX, nY );
                }
                else if(   nX >= (nCenterX - (nTileDimensions * 2) )
                        && nY >= (nCenterY - (nTileDimensions * 2) )
                        && nX <= (nCenterX + (nTileDimensions * 2) )
                        && nY <= (nCenterY + (nTileDimensions * 2) ) )
                {
                    BuildMedTile( nMed++, nX, nY );
                }
                else
                {
                    BuildLowTile( nLow++, nX, nY );
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  BuildTile
    //  Converts tile index nIndex into the tile with the specified center
    //-----------------------------------------------------------------------------
    void CTerrain::BuildLowTile( sint nIndex, sint nX, sint nY )
    {
        static CRenderer*       pRenderer   = Engine::GetRenderer();
        static IGraphicsDevice* pDevice     = pRenderer->GetGraphicsDevice();
        static float fTileDimensions = (float)nTileDimensions;
        static float fTileHalfDimensions = (float)nTileHalfDimensions;

        CTerrainTile* pTile = &m_pLowTiles[ nIndex ];

        pTile->m_nXPos = nX;
        pTile->m_nYPos = nY;

        // Calculate the vertex data
        float fX;
        float fY;
        sint nVertex = 0;
        for( fX = pTile->m_nXPos - fTileHalfDimensions, nX = 0; fX <= pTile->m_nXPos + fTileHalfDimensions; fX += nLowGranularity, ++nX  )
        {
            for( fY = pTile->m_nYPos - fTileHalfDimensions, nY = 0; fY <= pTile->m_nYPos + fTileHalfDimensions; fY += nLowGranularity, ++nY )
            {
                float fHeight = m_PerlinDetail.GetHeight( fX, fY );
                //fHeight = 0.0f;

                VPosNormalTex vert = { RVector3( fX, fHeight, fY ), RVector3Zero(), RVector2Zero() };
                m_pVertices[ nVertex++ ] = vert;
            }
        }

        // Calculate the face normals
        for( int i = 0; i < nLowIndices; i += 3 )
        {   
            VPosNormalTex& v0 = m_pVertices[ m_pLowIndices[i + 0] ];
            VPosNormalTex& v1 = m_pVertices[ m_pLowIndices[i + 1] ];
            VPosNormalTex& v2 = m_pVertices[ m_pLowIndices[i + 2] ];

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
        pDevice->UpdateBuffer( pTile->m_pVertexBuffer, m_pVertices );
    }

    void CTerrain::BuildMedTile( sint nIndex, sint nX, sint nY )
    {
        static CRenderer*       pRenderer   = Engine::GetRenderer();
        static IGraphicsDevice* pDevice     = pRenderer->GetGraphicsDevice();
        static float fTileDimensions = (float)nTileDimensions;
        static float fTileHalfDimensions = (float)nTileHalfDimensions;

        CTerrainTile* pTile = &m_pMedTiles[ nIndex ];

        pTile->m_nXPos = nX;
        pTile->m_nYPos = nY;

        // Calculate the vertex data
        float fX;
        float fY;
        sint nVertex = 0;
        for( fX = pTile->m_nXPos - fTileHalfDimensions, nX = 0; fX <= pTile->m_nXPos + fTileHalfDimensions; fX += nMedGranularity, ++nX  )
        {
            for( fY = pTile->m_nYPos - fTileHalfDimensions, nY = 0; fY <= pTile->m_nYPos + fTileHalfDimensions; fY += nMedGranularity, ++nY )
            {
                float fHeight = m_PerlinDetail.GetHeight( fX, fY );
                //fHeight = 0.0f;

                VPosNormalTex vert = { RVector3( fX, fHeight, fY ), RVector3Zero(), RVector2Zero() };
                m_pVertices[ nVertex++ ] = vert;
            }
        }

        // Calculate the face normals
        for( int i = 0; i < nMedIndices; i += 3 )
        {   
            VPosNormalTex& v0 = m_pVertices[ m_pMedIndices[i + 0] ];
            VPosNormalTex& v1 = m_pVertices[ m_pMedIndices[i + 1] ];
            VPosNormalTex& v2 = m_pVertices[ m_pMedIndices[i + 2] ];

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
        pDevice->UpdateBuffer( pTile->m_pVertexBuffer, m_pVertices );
    }

    void CTerrain::BuildHighTile( sint nIndex, sint nX, sint nY )
    {
        static CRenderer*       pRenderer   = Engine::GetRenderer();
        static IGraphicsDevice* pDevice     = pRenderer->GetGraphicsDevice();
        static float fTileDimensions = (float)nTileDimensions;
        static float fTileHalfDimensions = (float)nTileHalfDimensions;

        CTerrainTile* pTile = &m_pHighTiles[ nIndex ];

        pTile->m_nXPos = nX;
        pTile->m_nYPos = nY;

        // Calculate the vertex data
        float fX;
        float fY;
        sint nVertex = 0;
        for( fX = pTile->m_nXPos - fTileHalfDimensions, nX = 0; fX <= pTile->m_nXPos + fTileHalfDimensions; fX += nHighGranularity, ++nX  )
        {
            for( fY = pTile->m_nYPos - fTileHalfDimensions, nY = 0; fY <= pTile->m_nYPos + fTileHalfDimensions; fY += nHighGranularity, ++nY )
            {
                float fHeight = m_PerlinDetail.GetHeight( fX, fY );
                //fHeight = 0.0f;

                VPosNormalTex vert = { RVector3( fX, fHeight, fY ), RVector3Zero(), RVector2Zero() };
                m_pVertices[ nVertex++ ] = vert;
            }
        }

        // Calculate the face normals
        for( int i = 0; i < nHighIndices; i += 3 )
        {   
            VPosNormalTex& v0 = m_pVertices[ m_pHighIndices[i + 0] ];
            VPosNormalTex& v1 = m_pVertices[ m_pHighIndices[i + 1] ];
            VPosNormalTex& v2 = m_pVertices[ m_pHighIndices[i + 2] ];

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
        pDevice->UpdateBuffer( pTile->m_pVertexBuffer, m_pVertices );
    }

    //-----------------------------------------------------------------------------
    //  CenterTerrain
    //  Centers the active portion of the terrain around a point
    //-----------------------------------------------------------------------------
    void CTerrain::CenterTerrain( const RVector3& pos, float fRadius )
    {
        // First see if we even have updating to do
        sint nX;
        sint nY;

        CalculateTileCenter( pos.x, pos.z, nX, nY );

        if(    nX == m_nCurrX
            && nY == m_nCurrY )
        {
            // We're still centered around the same tile, no updating required
            return;
        }

        // Calculate which direction we went in
        sint nXDir = 0;
        sint nYDir = 0;

        if( nX > m_nCurrX )
            nXDir = 1;
        else if( nX < m_nCurrX )
            nXDir = -1;

        if( nY > m_nCurrY )
            nYDir = 1;
        else if( nY < m_nCurrY )
            nYDir = -1;

        m_nCurrX = nX;
        m_nCurrY = nY;

        // Theres updating to do. Worst case is we just exactly crossed a corner and have a lot of tiles to update
        sint nLowNeedsUpdating[ nNumLowTiles ];
        sint nMedNeedsUpdating[ nNumMedTiles ];
        sint nHighNeedsUpdating[ nNumHighTiles ];

        sint nNumLow = 0;
        sint nNumMed = 0;
        sint nNumHigh = 0;
        sint nIndex = 0;

        //////////////////////////////////////////
        // Low
        sint nMinX = nX - ( nTileDimensions * nTerrainTileDimensionsPerSide );
        sint nMaxX = nX + ( nTileDimensions * nTerrainTileDimensionsPerSide );        
        sint nMinY = nY - ( nTileDimensions * nTerrainTileDimensionsPerSide );
        sint nMaxY = nY + ( nTileDimensions * nTerrainTileDimensionsPerSide );

        // First figure out which low needs updating
        for( sint i = 0; i < nNumLowTiles; ++i )
        {
            if(     m_pLowTiles[i].m_nXPos < nMinX
                ||  m_pLowTiles[i].m_nXPos > nMaxX
                ||  m_pLowTiles[i].m_nYPos < nMinY
                ||  m_pLowTiles[i].m_nYPos > nMaxY )
            {
                // This tile is outside the bounds
                nLowNeedsUpdating[ nNumLow++ ] = i;
            }
        }
        
        // Now build our new X tiles
        if( nXDir < 0 )
        {
            sint nNewX = nMinX;
            for( sint nNewY = nMinY; nNewY <= nMaxY; nNewY += nTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        else if( nXDir > 0 )
        {
            sint nNewX = nMaxX;
            for( sint nNewY = nMinY; nNewY <= nMaxY; nNewY += nTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        // Then the Y
        if( nYDir < 0 )
        {
            sint nNewY = nMinY;
            for( sint nNewX = nMinX; nNewX <= nMaxX; nNewX += nTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        else if( nYDir > 0 )
        {
            sint nNewY = nMaxY;
            for( sint nNewX = nMinX; nNewX <= nMaxX; nNewX += nTileDimensions )
            {
                BuildLowTile( nLowNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        
        //////////////////////////////////////////
        //// Medium
        //fMinX = fX - ( fTileDimensions * 2 );
        //fMaxX = fX + ( fTileDimensions * 2 );
        //fMinY = fY - ( fTileDimensions * 2 );
        //fMaxY = fY + ( fTileDimensions * 2 );
        //for( sint i = 0; i < nNumMedTiles; ++i )
        //{
        //    if(     m_pMedTiles[i].m_fXPos < fMinX
        //        ||  m_pMedTiles[i].m_fXPos > fMaxX
        //        ||  m_pMedTiles[i].m_fYPos < fMinY
        //        ||  m_pMedTiles[i].m_fYPos > fMaxY )
        //    {
        //        // This tile is outside the bounds
        //        nMedNeedsUpdating[ nNumMed++ ] = i;
        //    }
        //}
        // Now build our new X tiles
        //nIndex = 0;
        //if( fXDir < 0.0f )
        //{
        //    float fNewX = fMinX;
        //    for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
        //    {
        //        BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
        //    }
        //}
        //else if( fXDir > 0.0f )
        //{
        //    float fNewX = fMaxX;
        //    for( float fNewY = fMinY; fNewY <= fMaxY; fNewY += fTileDimensions )
        //    {
        //        BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
        //    }
        //}
        //// Then the Y
        //if( fYDir < 0.0f )
        //{
        //    float fNewY = fMinY;
        //    for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
        //    {
        //        BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
        //    }
        //}
        //else if( fYDir > 0.0f )
        //{
        //    float fNewY = fMaxY;
        //    for( float fNewX = fMinX; fNewX <= fMaxX; fNewX += fTileDimensions )
        //    {
        //        BuildMedTile( nMedNeedsUpdating[ nIndex++ ], fNewX, fNewY );
        //    }
        //}

        //////////////////////////////////////////
        // High
        nMinX = nX - ( nTileDimensions * 1 );
        nMaxX = nX + ( nTileDimensions * 1 );
        nMinY = nY - ( nTileDimensions * 1 );
        nMaxY = nY + ( nTileDimensions * 1 );
        for( sint i = 0; i < nNumHighTiles; ++i )
        {
            if(     m_pHighTiles[i].m_nXPos < nMinX
                ||  m_pHighTiles[i].m_nXPos > nMaxX
                ||  m_pHighTiles[i].m_nYPos < nMinY
                ||  m_pHighTiles[i].m_nYPos > nMaxY )
            {
                // This tile is outside the bounds
                nHighNeedsUpdating[ nNumHigh++ ] = i;
            }
        }
        // Now build our new X tiles
        nIndex = 0;
        if( nXDir < 0 )
        {
            sint nNewX = nMinX;
            for( sint nNewY = nMinY; nNewY <= nMaxY; nNewY += nTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        else if( nXDir > 0 )
        {
            sint nNewX = nMaxX;
            for( sint nNewY = nMinY; nNewY <= nMaxY; nNewY += nTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        // Then the Y
        if( nYDir < 0 )
        {
            sint nNewY = nMinY;
            for( sint nNewX = nMinX; nNewX <= nMaxX; nNewX += nTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], nNewX, nNewY );
            }
        }
        else if( nYDir > 0 )
        {
            sint nNewY = nMaxY;
            for( sint nNewX = nMinX; nNewX <= nMaxX; nNewX += nTileDimensions )
            {
                BuildHighTile( nHighNeedsUpdating[ nIndex++ ], nNewX, nNewY );
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
        : m_nXPos( -100000 )
        , m_nYPos( -100000 )
    {
    }

    // CTerrainTile destructor
    CTerrainTile::~CTerrainTile()
    {
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
