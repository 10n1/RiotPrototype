/*********************************************************\
File:           Terrain.cpp
Author:         Kyle Weicht
Created:        4/6/2011
Modified:       5/5/2011 8:44:31 PM
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

        m_pTerrainTiles[1].RenderGraph( m_pTerrainTiles[1].m_pTerrainGraph, 4 );
    }

    //-----------------------------------------------------------------------------
    //  GenerateTerrain
    //  Generates the terrain
    //-----------------------------------------------------------------------------
    void CTerrain::GenerateTerrain( void )
    {
        float fTileDimensions = (float)CTerrainTile::nTileDimensions;
        float fTileHalfDimensions = (float)CTerrainTile::nTileHalfDimensions;

        for( sint i = -4; i < 4; ++i )
        {
            for( sint j = -4; j < 4; ++j )
            {
                GenerateTerrain( i*fTileDimensions, j*fTileDimensions );
            }
        }
    }

    CTerrainTile* CTerrain::GenerateTerrain( float fX, float fY )
    {
        float fTileDimensions = (float)CTerrainTile::nTileDimensions;
        float fTileHalfDimensions = (float)CTerrainTile::nTileHalfDimensions;

        // See if this tile already exists
        for( uint i = 0; i < m_nNumTiles; ++i )
        {
            if(    (fX > m_pTerrainTiles[i].m_fXPos - fTileHalfDimensions)
                && (fY > m_pTerrainTiles[i].m_fYPos - fTileHalfDimensions)
                && (fX < m_pTerrainTiles[i].m_fXPos + fTileHalfDimensions)
                && (fY < m_pTerrainTiles[i].m_fYPos + fTileHalfDimensions) )
            {
                return &m_pTerrainTiles[i];
            }
        }

        // It doesn't, calculate the tiles center
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

        // Now grab a tile from the list
        uint nIndex = AtomicIncrement( &m_nNumTiles ) - 1;
        CTerrainTile* pTile = &m_pTerrainTiles[ nIndex ];
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

        // Now create the scene graph
        pTile->BuildSceneGraph();

        // Add it to the component
        CComponentCollidable::AddTerrainTile( pTile );

        return pTile;
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
    //  BuildParentNodes
    //  Constructs the top of the tree
    //-----------------------------------------------------------------------------
    void CTerrainTile::BuildParentNodes( TTerrainParentNode* pNode, TTerrainParentNode* pParent )
    {
        pNode->m_pParent = pParent;
        pNode->m_pTile = this;
        float fNewX = (pNode->max.x + pNode->min.x) / 2.0f;
        float fNewZ = (pNode->max.z + pNode->min.z) / 2.0f;

        if( Abs(pNode->max.x - pNode->min.x) > 2.0f )
        {
            TTerrainParentNode* pNewNode = NULL;

            pNewNode = &m_pParentNodes[m_nNumParents++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->max.x = fNewX;
            pNewNode->max.z = fNewZ;
            pNode->m_pChildren[0] = pNewNode;

            BuildParentNodes( pNewNode, pNode );

            pNewNode = &m_pParentNodes[m_nNumParents++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->min.z = fNewZ;
            pNewNode->max.x = fNewX;
            pNode->m_pChildren[1] = pNewNode;

            BuildParentNodes( pNewNode, pNode );

            pNewNode = &m_pParentNodes[m_nNumParents++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->min.x = fNewX;
            pNewNode->min.z = fNewZ;
            pNode->m_pChildren[2] = pNewNode;

            BuildParentNodes( pNewNode, pNode );

            pNewNode = &m_pParentNodes[m_nNumParents++];
            pNewNode->min = pNode->min;
            pNewNode->max = pNode->max;
            pNewNode->min.x = fNewX;
            pNewNode->max.z = fNewZ;
            pNode->m_pChildren[3] = pNewNode;

            BuildParentNodes( pNewNode, pNode );
        }
        else
        {
            TTerrainLeafNode* pLeafNode = NULL;
            pNode->m_nLowestParent = 1;

            pLeafNode = &m_pLeafNodes[m_nNumLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->max.x = fNewX;
            pLeafNode->max.z = fNewZ;
            pLeafNode->m_pTile = this;
            pNode->m_pChildren[0] = pLeafNode;

            pLeafNode = &m_pLeafNodes[m_nNumLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->min.z = fNewZ;
            pLeafNode->max.x = fNewX;
            pLeafNode->m_pTile = this;
            pNode->m_pChildren[1] = pLeafNode;

            pLeafNode = &m_pLeafNodes[m_nNumLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->min.x = fNewX;
            pLeafNode->min.z = fNewZ;
            pLeafNode->m_pTile = this;
            pNode->m_pChildren[2] = pLeafNode;

            pLeafNode = &m_pLeafNodes[m_nNumLeaves++];
            pLeafNode->min = pNode->min;
            pLeafNode->max = pNode->max;
            pLeafNode->min.x = fNewX;
            pLeafNode->max.z = fNewZ;
            pLeafNode->m_pTile = this;
            pNode->m_pChildren[3] = pLeafNode;
        }
    }


    //-----------------------------------------------------------------------------
    //  BuildSceneGraph
    //  Builds the scene graph
    //-----------------------------------------------------------------------------
    void CTerrainTile::BuildSceneGraph( void )
    {
        // Initialize the graph
        m_nNumLeaves = 0;
        m_nNumParents = 1;

        m_pTerrainGraph = &m_pParentNodes[ 0 ];
        m_pTerrainGraph->min = RVector3( m_fXPos - nTileHalfDimensions, -300000.0f, m_fYPos - nTileHalfDimensions );
        m_pTerrainGraph->max = RVector3( m_fXPos + nTileHalfDimensions, 300000.0f, m_fYPos + nTileHalfDimensions );
        m_pTerrainGraph->m_pTile = this;

        // Build the parent nodes
        BuildParentNodes( m_pTerrainGraph, NULL );
        
        // Add the triangles
        for( uint i = 0; i < nIndices; i += 3 )
        {
            AddTriangleToGraph( m_pTerrainGraph, m_pIndices + i, false );
        }

        // Rebalance the top of the tree
        RecomputeSceneGraphBounds( m_pTerrainGraph );
    }

    //-----------------------------------------------------------------------------
    //  RecomputeSceneGraphBounds
    //  Recomputes the bounds of the top of the tree
    //-----------------------------------------------------------------------------
    void CTerrainTile::RecomputeSceneGraphBounds( TSceneNode* pNode )
    {
        TTerrainParentNode* pParentNode = (TTerrainParentNode*)pNode;

        pParentNode->max = RVector3( -10000.0f, -10000.0f, -10000.0f );
        pParentNode->min = RVector3( 10000.0f, 10000.0f, 10000.0f );
        for( uint i = 0; i < 4; ++i )
        {
            if( !pParentNode->m_nLowestParent )
            {
                // We're a grandparent, calculate the children
                RecomputeSceneGraphBounds( pParentNode->m_pChildren[i] );
            }

            pNode->max.x = Max( pParentNode->m_pChildren[i]->max.x, pNode->max.x );
            pNode->max.y = Max( pParentNode->m_pChildren[i]->max.y, pNode->max.y );
            pNode->max.z = Max( pParentNode->m_pChildren[i]->max.z, pNode->max.z );

            pNode->min.x = Min( pParentNode->m_pChildren[i]->min.x, pNode->min.x );
            pNode->min.y = Min( pParentNode->m_pChildren[i]->min.y, pNode->min.y );
            pNode->min.z = Min( pParentNode->m_pChildren[i]->min.z, pNode->min.z );
        }
    }

    //-----------------------------------------------------------------------------
    //  AddTriangleToGraph
    //  Adds a triangle to the graph
    //-----------------------------------------------------------------------------
    void CTerrainTile::AddTriangleToGraph( TSceneNode* pNode, uint16* pIndices, bool bLeaf )
    {        
        for( int i = 0; i < 3; ++i )
        {
            if(    m_pVertexPositions[ pIndices[i] ].x > pNode->max.x
                || m_pVertexPositions[ pIndices[i] ].y > pNode->max.y
                || m_pVertexPositions[ pIndices[i] ].z > pNode->max.z
                || m_pVertexPositions[ pIndices[i] ].x < pNode->min.x
                || m_pVertexPositions[ pIndices[i] ].y < pNode->min.y
                || m_pVertexPositions[ pIndices[i] ].z < pNode->min.z )
            {
                return;
            }
        }

        if( !bLeaf )
        {
            // There are still children below us, add it
            TTerrainParentNode* pParentNode = (TTerrainParentNode*)pNode;
            if( !pParentNode->m_nLowestParent )
            {
                for( int i = 0; i < 4; ++i )
                {
                    AddTriangleToGraph( pParentNode->m_pChildren[i], pIndices, false );
                }
                return;
            }
            else
            {
                for( int i = 0; i < 4; ++i )
                {
                    AddTriangleToGraph( pParentNode->m_pChildren[i], pIndices, true );
                }
                return;
            }
        }

        // This is a leaf, see which child the triangle goes in
        TTerrainLeafNode* pLeaf = (TTerrainLeafNode*)pNode;
        if( pLeaf->m_nNumAdded == 0 )
        {
            pLeaf->m_pCornerIndices[0] = pIndices[0];
            pLeaf->m_pCornerIndices[1] = pIndices[1];
            pLeaf->m_pCornerIndices[2] = pIndices[2];

            pLeaf->m_nNumAdded++;
        }
        else
        {
            // Add the missing index
            for( uint i = 0; i < 3; ++i )
            {
                bool bFound = false;
                for( uint j = 0; j < 3; ++j )
                {
                    if( pLeaf->m_pCornerIndices[j] == pIndices[i] )
                    {
                        bFound = true;
                        break;
                    }
                }

                if( !bFound )
                {
                    pLeaf->m_pCornerIndices[3] = pIndices[i];
                }
            }

            // Now recalculate the leaf bounds
            pLeaf->max = RVector3( -10000.0f, -10000.0f, -10000.0f );
            pLeaf->min = RVector3( 10000.0f, 10000.0f, 10000.0f );
            for( uint i = 0; i < 4; ++i )
            {
                pLeaf->max.x = Max( m_pVertexPositions[ pLeaf->m_pCornerIndices[i] ].x, pLeaf->max.x );
                pLeaf->max.y = Max( m_pVertexPositions[ pLeaf->m_pCornerIndices[i] ].y, pLeaf->max.y );
                pLeaf->max.z = Max( m_pVertexPositions[ pLeaf->m_pCornerIndices[i] ].z, pLeaf->max.z );
                pLeaf->min.x = Min( m_pVertexPositions[ pLeaf->m_pCornerIndices[i] ].x, pLeaf->min.x );
                pLeaf->min.y = Min( m_pVertexPositions[ pLeaf->m_pCornerIndices[i] ].y, pLeaf->min.y );
                pLeaf->min.z = Min( m_pVertexPositions[ pLeaf->m_pCornerIndices[i] ].z, pLeaf->min.z );
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  SphereTerrainCollision
    //  Determines if a tree hits any triangles within the node
    //-----------------------------------------------------------------------------
    bool CTerrainTile::SphereTerrainCollision( const RSphere& s )
    {
        return SphereTerrainCollision( m_pTerrainGraph, s );
    }
    bool CTerrainTile::SphereTerrainCollision( TSceneNode* pNode, const RSphere& s )
    {
        TTerrainParentNode* pParentNode = (TTerrainParentNode*)pNode;
        if( SphereAABBCollision( *pNode, s ) )
        {
            if( !pParentNode->m_nLowestParent )
            {
                for( uint i = 0; i < 4; ++i )
                {
                    if( SphereTerrainCollision( pParentNode->m_pChildren[i], s ) )
                        return true;
                }
            }
            else
            {
                for( uint i = 0; i < 4; ++i )
                {
                    TTerrainLeafNode* pLeaf = (TTerrainLeafNode*)pParentNode->m_pChildren[i];
                    if( pLeaf->SphereTriangleCollision( s ) )
                        return true;
                }
            }
        }

        return false;
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
    //  RenderGraph
    //  Renders the terrain
    //-----------------------------------------------------------------------------
    void CTerrainTile::RenderGraph( TTerrainParentNode* pNode, uint nDepth )
    {
        CRenderer*  pRenderer = Engine::GetRenderer();
        static const RVector3    vColors[] =
        {
            RVector3( 1.0f, 1.0f, 1.0f ),
            RVector3( 0.0f, 1.0f, 1.0f ),
            RVector3( 1.0f, 0.0f, 1.0f ),
            RVector3( 1.0f, 1.0f, 0.0f ),
            RVector3( 0.0f, 1.0f, 0.0f ),
            RVector3( 0.0f, 0.0f, 1.0f ),
            RVector3( 1.0f, 0.0f, 0.0f ),
            RVector3( 0.0f, 0.0f, 0.0f ),
            RVector3( 0.5f, 0.5f, 0.5f ),
            RVector3( 1.0f, 0.5f, 0.0f ),
            RVector3( 0.0f, 1.0f, 0.5f ),
            RVector3( 0.5f, 0.0f, 1.0f ),
        };

        if( nDepth )
        {
            ((TSceneNode*)pNode)->DrawNode( pRenderer, vColors[nDepth] );

            if( !pNode->m_nLowestParent )
            {
                for( uint i = 0; i < 4; ++i )
                {
                    RenderGraph( (TTerrainParentNode*)pNode->m_pChildren[i], nDepth-1 );
                }
            }
            else
            {
                for( uint i = 0; i < 4; ++i )
                {
                    pNode->m_pChildren[i]->DrawNode( pRenderer, vColors[nDepth-1] );
                }
            }
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
                
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 0 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nTileDimensions );

                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nTileDimensions );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 );
                m_pIndices[ nIndex++ ] = (uint16)(nX + nY + nStart + 1 + nTileDimensions + 1 );
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
        m_pMesh = pRender->CreateMesh( VPosNormalTex::VertexStride, nVertsTotal, 2, nIndices, pVertices, m_pIndices );

        
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
        double v1 = PerlinInterpolate(x0y0, x1y0, Xfrac); //interpolate in x direction (y)
        double v2 = PerlinInterpolate(x0y1, x1y1, Xfrac); //interpolate in x direction (y+1)
        double fin = PerlinInterpolate(v1, v2, Yfrac);  //interpolate in y direction

        return fin;
    }

    double PerlinNoise::PerlinInterpolate(double x, double y, double a) const
    {
        double negA = 1.0 - a;
        double negASqr = negA * negA;
        double fac1 = 3.0 * (negASqr) - 2.0 * (negASqr * negA);
        double aSqr = a * a;
        double fac2 = 3.0 * aSqr - 2.0 * (aSqr * a);

        //return x * fac1 + y * fac2; //add the weighted factors

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
