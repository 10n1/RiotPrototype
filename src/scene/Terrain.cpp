/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/10/2011 6:21:59 PM
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
        SAFE_RELEASE( m_pLowIndexBuffer );
        SAFE_RELEASE( m_pMedIndexBuffer );
        SAFE_RELEASE( m_pHighIndexBuffer );

        SAFE_RELEASE( m_pTexture );

        // Low
        for( uint i = 0; i < nNumLowTiles; ++i )
        {
            SAFE_RELEASE( m_pLowTiles[i].m_pVertexBuffer );
        }
        
        // Medium
        for( uint i = 0; i < nNumMedTiles; ++i )
        {
            SAFE_RELEASE( m_pMedTiles[i].m_pVertexBuffer );
        }

        // High
        for( uint i = 0; i < nNumHighTiles; ++i )
        {
            SAFE_RELEASE( m_pHighTiles[i].m_pVertexBuffer );
        }
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

        CalculateTileCenter( 10.0f, 10.0f, nCenterX, nCenterY );

        m_nCurrX = nCenterX;
        m_nCurrY = nCenterY;

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

        // Update the terrain in each direction
        if( nXDir > 0 )
            UpdatePosX();
        else if( nXDir < 0 )
            UpdateNegX();
        
        if( nYDir > 0 )
            UpdatePosY();
        else if( nYDir < 0 )
            UpdateNegY();
    }


    //-----------------------------------------------------------------------------
    //  Update*
    //  Updates the terrain in either direction
    //-----------------------------------------------------------------------------
    void CTerrain::UpdatePosX( void )
    {
        sint nOldX;
        sint nNewX;
        
        sint nOldFrontX;
        sint nOldBackX;
        sint nNewFrontX;
        sint nNewBackX;

        sint nTopY;
        sint nBottomY;

        //////////////////////////////////////////
        // First update the High detail
        nOldX = m_nCurrX - nTileDimensions * 2;
        nNewX = m_nCurrX + nTileDimensions;

        for( sint i = 0; i < nNumHighTiles; ++i )
        {
            if( m_pHighTiles[i].m_nXPos == nOldX )
            {
                // This tile is outside the bounds
                BuildHighTile( i, nNewX, m_pHighTiles[i].m_nYPos );
            }
        }
        
        //////////////////////////////////////////
        // Then medium
        nOldBackX = nOldX - nTileDimensions;
        nNewBackX = nOldX;
        
        nOldFrontX = nNewX;
        nNewFrontX = nNewX + nTileDimensions;

        nTopY = m_nCurrY + nTileDimensions * 2;
        nBottomY = m_nCurrY - nTileDimensions * 2;

        for( sint i = 0; i < nNumMedTiles; ++i )
        {
            if(    m_pMedTiles[i].m_nXPos == nOldBackX )
            {
                if(    m_pMedTiles[i].m_nYPos == nTopY 
                    || m_pMedTiles[i].m_nYPos == nBottomY)
                {
                    // These are the corner tiles
                    BuildMedTile( i, nNewFrontX, m_pMedTiles[i].m_nYPos );
                }
                else
                {
                    // The middle tiles
                    BuildMedTile( i, nNewBackX, m_pMedTiles[i].m_nYPos );
                }
            }
            else if( m_pMedTiles[i].m_nXPos == nOldFrontX )
            {
                if(    m_pMedTiles[i].m_nYPos == nTopY 
                    || m_pMedTiles[i].m_nYPos == nBottomY)
                {
                    // Ignore these corners
                    continue;
                }

                BuildMedTile( i, nNewFrontX, m_pMedTiles[i].m_nYPos );
            }
        }

        //////////////////////////////////////////
        // Then low
        nOldX = (m_nCurrX - nTileDimensions) - (nTerrainTileDimensionsPerSide*nTileDimensions);
        nNewX = m_nCurrX + (nTerrainTileDimensionsPerSide*nTileDimensions);

        nOldBackX = nOldBackX - nTileDimensions;
        nNewBackX = nNewBackX - nTileDimensions;
        
        nOldFrontX = nOldFrontX + nTileDimensions;
        nNewFrontX = nNewFrontX + nTileDimensions;

        nTopY = m_nCurrY + nTileDimensions * 3;
        nBottomY = m_nCurrY - nTileDimensions * 3;
        
        for( sint i = 0; i < nNumLowTiles; ++i )
        {
            // First the inside edges
            if(    m_pLowTiles[i].m_nXPos == nOldFrontX 
                && m_pLowTiles[i].m_nYPos < nTopY
                && m_pLowTiles[i].m_nYPos > nBottomY )
            {
                // The middle tiles
                BuildLowTile( i, nNewBackX, m_pLowTiles[i].m_nYPos );
            }
            // then the outsides
            else if( m_pLowTiles[i].m_nXPos == nOldX )
            {
                // This tile is outside the bounds
                BuildLowTile( i, nNewX, m_pLowTiles[i].m_nYPos );
            }
        }
    }
    void CTerrain::UpdateNegX( void )
    {
        sint nOldX;
        sint nNewX;

        sint nOldFrontX;
        sint nOldBackX;
        sint nNewFrontX;
        sint nNewBackX;

        sint nTopY;
        sint nBottomY;

        //////////////////////////////////////////
        // First update the High detail
        nOldX = m_nCurrX + nTileDimensions * 2;
        nNewX = m_nCurrX - nTileDimensions;

        for( sint i = 0; i < nNumHighTiles; ++i )
        {
            if( m_pHighTiles[i].m_nXPos == nOldX )
            {
                // This tile is outside the bounds
                BuildHighTile( i, nNewX, m_pHighTiles[i].m_nYPos );
            }
        }

        //////////////////////////////////////////
        // Then medium
        nOldBackX = nOldX + nTileDimensions;
        nNewBackX = nOldX;
        
        nOldFrontX = nNewX;
        nNewFrontX = nNewX - nTileDimensions;

        nTopY = m_nCurrY + nTileDimensions * 2;
        nBottomY = m_nCurrY - nTileDimensions * 2;

        for( sint i = 0; i < nNumMedTiles; ++i )
        {
            if(    m_pMedTiles[i].m_nXPos == nOldBackX )
            {
                if(    m_pMedTiles[i].m_nYPos == nTopY 
                    || m_pMedTiles[i].m_nYPos == nBottomY)
                {
                    // These are the corner tiles
                    BuildMedTile( i, nNewFrontX, m_pMedTiles[i].m_nYPos );
                }
                else
                {
                    // The middle tiles
                    BuildMedTile( i, nNewBackX, m_pMedTiles[i].m_nYPos );
                }
            }
            else if( m_pMedTiles[i].m_nXPos == nOldFrontX )
            {
                if(    m_pMedTiles[i].m_nYPos == nTopY 
                    || m_pMedTiles[i].m_nYPos == nBottomY)
                {
                    // Ignore these corners
                    continue;
                }

                BuildMedTile( i, nNewFrontX, m_pMedTiles[i].m_nYPos );
            }
        }
        
        //////////////////////////////////////////
        // Then low        
        nOldX = (m_nCurrX + nTileDimensions) + (nTerrainTileDimensionsPerSide*nTileDimensions);
        nNewX = m_nCurrX - (nTerrainTileDimensionsPerSide*nTileDimensions);
        
        nOldBackX = nOldBackX + nTileDimensions;
        nNewBackX = nNewBackX + nTileDimensions;
        
        nOldFrontX = nOldFrontX - nTileDimensions;
        nNewFrontX = nNewFrontX - nTileDimensions;

        nTopY = m_nCurrY + nTileDimensions * 3;
        nBottomY = m_nCurrY - nTileDimensions * 3;
        
        for( sint i = 0; i < nNumLowTiles; ++i )
        {
            // First the inside edges
            if(    m_pLowTiles[i].m_nXPos == nOldFrontX 
                && m_pLowTiles[i].m_nYPos < nTopY
                && m_pLowTiles[i].m_nYPos > nBottomY )
            {
                // The middle tiles
                BuildLowTile( i, nNewBackX, m_pLowTiles[i].m_nYPos );
            }
            // then the outsides
            else if( m_pLowTiles[i].m_nXPos == nOldX )
            {
                // This tile is outside the bounds
                BuildLowTile( i, nNewX, m_pLowTiles[i].m_nYPos );
            }
        }
    }
    void CTerrain::UpdatePosY( void )
    {
        sint nOldY;
        sint nNewY;

        sint nOldFrontY;
        sint nOldBackY;
        sint nNewFrontY;
        sint nNewBackY;

        sint nTopX;
        sint nBottomX;

        //////////////////////////////////////////
        // First update the High detail
        nOldY = m_nCurrY - nTileDimensions * 2;
        nNewY = m_nCurrY + nTileDimensions;

        for( sint i = 0; i < nNumHighTiles; ++i )
        {
            if( m_pHighTiles[i].m_nYPos == nOldY )
            {
                // This tile is outside the bounds
                BuildHighTile( i, m_pHighTiles[i].m_nXPos, nNewY );
            }
        }

        //////////////////////////////////////////
        // Then medium
        nOldBackY = nOldY - nTileDimensions;
        nNewBackY = nOldY;
        
        nOldFrontY = nNewY;
        nNewFrontY = nNewY + nTileDimensions;

        nTopX = m_nCurrX + nTileDimensions * 2;
        nBottomX = m_nCurrX - nTileDimensions * 2;

        for( sint i = 0; i < nNumMedTiles; ++i )
        {
            if(    m_pMedTiles[i].m_nYPos == nOldBackY )
            {
                if(    m_pMedTiles[i].m_nXPos == nTopX 
                    || m_pMedTiles[i].m_nXPos == nBottomX)
                {
                    // These are the corner tiles
                    BuildMedTile( i, m_pMedTiles[i].m_nXPos, nNewFrontY );
                }
                else
                {
                    // The middle tiles
                    BuildMedTile( i, m_pMedTiles[i].m_nXPos, nNewBackY );
                }
            }
            else if( m_pMedTiles[i].m_nYPos == nOldFrontY )
            {
                if(    m_pMedTiles[i].m_nXPos == nTopX 
                    || m_pMedTiles[i].m_nXPos == nBottomX)
                {
                    // Ignore these corners
                    continue;
                }

                BuildMedTile( i, m_pMedTiles[i].m_nXPos, nNewFrontY );
            }
        }

        //////////////////////////////////////////
        // Then low
        nOldY = (m_nCurrY - nTileDimensions) - (nTerrainTileDimensionsPerSide*nTileDimensions);
        nNewY = m_nCurrY + (nTerrainTileDimensionsPerSide*nTileDimensions);

        nOldBackY = nOldBackY - nTileDimensions;
        nNewBackY = nNewBackY - nTileDimensions;
        
        nOldFrontY = nOldFrontY + nTileDimensions;
        nNewFrontY = nNewFrontY + nTileDimensions;

        nTopX = m_nCurrX + nTileDimensions * 3;
        nBottomX = m_nCurrX - nTileDimensions * 3;
        
        for( sint i = 0; i < nNumLowTiles; ++i )
        {
            // First the inside edges
            if(    m_pLowTiles[i].m_nYPos == nOldFrontY 
                && m_pLowTiles[i].m_nXPos < nTopX
                && m_pLowTiles[i].m_nXPos > nBottomX )
            {
                // The middle tiles
                BuildLowTile( i, m_pLowTiles[i].m_nXPos, nNewBackY );
            }
            // then the outsides
            else if( m_pLowTiles[i].m_nYPos == nOldY )
            {
                // This tile is outside the bounds
                BuildLowTile( i, m_pLowTiles[i].m_nXPos, nNewY );
            }
        }
    }
    void CTerrain::UpdateNegY( void )
    {
        sint nOldY;
        sint nNewY;

        sint nOldFrontY;
        sint nOldBackY;
        sint nNewFrontY;
        sint nNewBackY;

        sint nTopX;
        sint nBottomX;

        //////////////////////////////////////////
        // First update the High detail
        nOldY = m_nCurrY + nTileDimensions * 2;
        nNewY = m_nCurrY - nTileDimensions;

        for( sint i = 0; i < nNumHighTiles; ++i )
        {
            if( m_pHighTiles[i].m_nYPos == nOldY )
            {
                // This tile is outside the bounds
                BuildHighTile( i, m_pHighTiles[i].m_nXPos, nNewY );
            }
        }

        
        //////////////////////////////////////////
        // Then medium
        nOldBackY = nOldY + nTileDimensions;
        nNewBackY = nOldY;
        
        nOldFrontY = nNewY;
        nNewFrontY = nNewY - nTileDimensions;

        nTopX = m_nCurrX + nTileDimensions * 2;
        nBottomX = m_nCurrX - nTileDimensions * 2;

        for( sint i = 0; i < nNumMedTiles; ++i )
        {
            if(    m_pMedTiles[i].m_nYPos == nOldBackY )
            {
                if(    m_pMedTiles[i].m_nXPos == nTopX 
                    || m_pMedTiles[i].m_nXPos == nBottomX)
                {
                    // These are the corner tiles
                    BuildMedTile( i, m_pMedTiles[i].m_nXPos, nNewFrontY );
                }
                else
                {
                    // The middle tiles
                    BuildMedTile( i, m_pMedTiles[i].m_nXPos, nNewBackY );
                }
            }
            else if( m_pMedTiles[i].m_nYPos == nOldFrontY )
            {
                if(    m_pMedTiles[i].m_nXPos == nTopX 
                    || m_pMedTiles[i].m_nXPos == nBottomX)
                {
                    // Ignore these corners
                    continue;
                }

                BuildMedTile( i, m_pMedTiles[i].m_nXPos, nNewFrontY );
            }
        }
        
        //////////////////////////////////////////
        // Then low
        nOldY = (m_nCurrY + nTileDimensions) + (nTerrainTileDimensionsPerSide*nTileDimensions);
        nNewY = m_nCurrY - (nTerrainTileDimensionsPerSide*nTileDimensions);

        nOldBackY = nOldBackY + nTileDimensions;
        nNewBackY = nNewBackY + nTileDimensions;
        
        nOldFrontY = nOldFrontY - nTileDimensions;
        nNewFrontY = nNewFrontY - nTileDimensions;

        nTopX = m_nCurrX + nTileDimensions * 3;
        nBottomX = m_nCurrX - nTileDimensions * 3;
        
        for( sint i = 0; i < nNumLowTiles; ++i )
        {
            // First the inside edges
            if(    m_pLowTiles[i].m_nYPos == nOldFrontY 
                && m_pLowTiles[i].m_nXPos < nTopX
                && m_pLowTiles[i].m_nXPos > nBottomX )
            {
                // The middle tiles
                BuildLowTile( i, m_pLowTiles[i].m_nXPos, nNewBackY );
            }
            // then the outsides
            else if( m_pLowTiles[i].m_nYPos == nOldY )
            {
                // This tile is outside the bounds
                BuildLowTile( i, m_pLowTiles[i].m_nXPos, nNewY );
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
