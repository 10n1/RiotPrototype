/*********************************************************\
File:           CollisionSystem.cpp
Author:         Kyle Weicht
Created:        5/24/2011
Modified:       5/24/2011 4:19:38 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "CollisionSystem.h"
#include "Object.h"
#include "Terrain.h"
#include <float.h>
#include "thread.h"

namespace Riot
{
    RSphere     CCollisionSystem::m_pBoundingSpheres[ MAX_OBJECTS ];
    CObject*    CCollisionSystem::m_pObjects[ MAX_OBJECTS ] = { NULL };
    uint        CCollisionSystem::m_nNumObjects = 0;
    CTerrain*   CCollisionSystem::m_pTerrain = NULL;
    CCollisionSystem::TSAPPair    CCollisionSystem::m_Pairs[ MAX_OBJECTS ];
    CCollisionSystem::TSAPBox     CCollisionSystem::m_Boxes[ MAX_OBJECTS ];
    CCollisionSystem::TEndPoint   CCollisionSystem::m_EndPointsX[ MAX_OBJECTS * 2 ];
    CCollisionSystem::TEndPoint   CCollisionSystem::m_EndPointsY[ MAX_OBJECTS * 2 ];
    CCollisionSystem::TEndPoint   CCollisionSystem::m_EndPointsZ[ MAX_OBJECTS * 2 ];
    uint        CCollisionSystem::m_nNumPairs = 0;
    uint        CCollisionSystem::m_nNumBoxes = 1;

    // CCollisionSystem constructor
    CCollisionSystem::CCollisionSystem()
    {
    }

    // CCollisionSystem destructor
    CCollisionSystem::~CCollisionSystem()
    {
    }

    void CCollisionSystem::Init( void )
    {
        // Make sure we don't have any leaves
        m_pTerrain = NULL;

        m_nNumPairs = 0;
        m_nNumBoxes = 1;

        Memset( m_EndPointsX, 0, sizeof( m_EndPointsX ) );
        Memset( m_EndPointsY, 0, sizeof( m_EndPointsY ) );
        Memset( m_EndPointsZ, 0, sizeof( m_EndPointsZ ) );
        Memset( m_Boxes, 0, sizeof( m_Boxes ) );

        // Initialize the end points with min and max values
        m_EndPointsX[0].fValue = -FLT_MAX;
        m_EndPointsX[0].nIndex |= 0x7FFFFFFF;
        m_EndPointsX[1].fValue =  FLT_MAX;
        m_EndPointsX[1].nIndex |= 0xFFFFFFFF;

        m_EndPointsY[0].fValue = -FLT_MAX;
        m_EndPointsY[0].nIndex |= 0x7FFFFFFF;
        m_EndPointsY[1].fValue =  FLT_MAX;
        m_EndPointsY[1].nIndex |= 0xFFFFFFFF;

        m_EndPointsZ[0].fValue = -FLT_MAX;
        m_EndPointsZ[0].nIndex |= 0x7FFFFFFF;
        m_EndPointsZ[1].fValue =  FLT_MAX;
        m_EndPointsZ[1].nIndex |= 0xFFFFFFFF;
    }
    
    uint CCollisionSystem::AddObject( const RAABB& box, uint nObject )
    {
        uint nArrayEnd = m_nNumBoxes * 2;

        //////////////////////////////////////////
        // Add the box to the axes
        m_EndPointsX[ nArrayEnd + 1 ] = m_EndPointsX[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsX[ nArrayEnd - 1 ].fValue = box.min.x;
        m_EndPointsX[ nArrayEnd - 1 ].nIndex = nObject | eSAPMinMask;
        m_EndPointsX[ nArrayEnd     ].fValue = box.max.x;
        m_EndPointsX[ nArrayEnd     ].nIndex = nObject;
        
        m_EndPointsY[ nArrayEnd + 1 ] = m_EndPointsY[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsY[ nArrayEnd - 1 ].fValue = box.min.y;
        m_EndPointsY[ nArrayEnd - 1 ].nIndex = nObject | eSAPMinMask;
        m_EndPointsY[ nArrayEnd     ].fValue = box.max.y;
        m_EndPointsY[ nArrayEnd     ].nIndex = nObject;
        
        m_EndPointsZ[ nArrayEnd + 1 ] = m_EndPointsZ[ nArrayEnd - 1]; // Bump the end two away
        m_EndPointsZ[ nArrayEnd - 1 ].fValue = box.min.z;
        m_EndPointsZ[ nArrayEnd - 1 ].nIndex = nObject | eSAPMinMask;
        m_EndPointsZ[ nArrayEnd     ].fValue = box.max.z;
        m_EndPointsZ[ nArrayEnd     ].nIndex = nObject;

        //////////////////////////////////////////
        // Now allocate a box
        TSAPBox& SAPbox = m_Boxes[ nObject ];
        SAPbox.nMin[0] = nArrayEnd-1;
        SAPbox.nMin[1] = nArrayEnd-1;
        SAPbox.nMin[2] = nArrayEnd-1;
        SAPbox.nMax[0] = nArrayEnd;
        SAPbox.nMax[1] = nArrayEnd;
        SAPbox.nMax[2] = nArrayEnd;

        SAPbox.nObject = nObject;

        //////////////////////////////////////////
        // Then force an update
        UpdateObject( box, nObject );

        return m_nNumBoxes++;
    }

    void CCollisionSystem::UpdateObject( const RAABB& box, uint nBox )
    {
        static CMutex m;
        CScopedMutex lock( &m );
        // Update the 3 axes
        UpdateXAxis( box.min.x, box.max.x, nBox );
        UpdateYAxis( box.min.y, box.max.y, nBox );
        UpdateZAxis( box.min.z, box.max.z, nBox );
    }

    void CCollisionSystem::UpdateObject( CObject* pObject )
    {
        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            if( m_pObjects[i] == pObject )
            {
                RVector3* v;

                pObject->GetProperty( "position", (void**)&v );
                m_pBoundingSpheres[i].position = *v;
                
                float fRadius = m_pBoundingSpheres[i].radius;

                RVector3 rad( fRadius, fRadius, fRadius );

                RVector3 min = *v - rad;
                RVector3 max = *v + rad;

                UpdateObject( RAABB( min, max ), i );
            }
        }
    }

    void CCollisionSystem::UpdateXAxis( float fMin, float fMax, uint nBox )
    {
        //////////////////////////////////////////
        // X Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[0];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[0];
        sint  nIndex;

        float fOldMin = m_EndPointsX[ nMinIndex ].fValue;
        float fOldMax = m_EndPointsX[ nMaxIndex ].fValue;

        m_EndPointsX[ nMinIndex ].fValue = fMin;
        m_EndPointsX[ nMaxIndex ].fValue = fMax;

        //////////////////////////////////////////
        // First move the min, left or right
        if( fMin < fOldMin )
        {
            sint nThat = nMinIndex-1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue < m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMax( m_EndPointsX[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }

                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                --nThis;
                --nThat;
            }

            m_Boxes[ nBox ].nMin[0] = nThis;
        }
        else
        {
            sint nThat = nMinIndex+1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue > m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMax( m_EndPointsX[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }


                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                ++nThis;
                ++nThat;
            }

            m_Boxes[ nBox ].nMin[0] = nThis;
        }
        
        //////////////////////////////////////////
        // Then the max, left or right
        if( fMax < fOldMax )
        {
            sint nThat = nMaxIndex-1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue < m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMin( m_EndPointsX[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }

                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                --nThis;
                --nThat;
            }
            m_Boxes[ nBox ].nMax[0] = nThis;
        }
        else
        {
            sint nThat = nMaxIndex+1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsX[ nThis ].fValue > m_EndPointsX[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsX[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMin( m_EndPointsX[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsX[ nThis ].nIndex & eSAPClearMask, m_EndPointsX[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[0] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[0] = nThis;
                }

                Swap( m_EndPointsX[ nThis ], m_EndPointsX[ nThat ] );

                ++nThis;
                ++nThat;
            }
            m_Boxes[ nBox ].nMax[0] = nThis;
        }
    }

    void CCollisionSystem::UpdateYAxis( float fMin, float fMax, uint nBox )
    {
        //////////////////////////////////////////
        // Y Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[1];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[1];
        sint  nIndex;

        float fOldMin = m_EndPointsY[ nMinIndex ].fValue;
        float fOldMax = m_EndPointsY[ nMaxIndex ].fValue;

        m_EndPointsY[ nMinIndex ].fValue = fMin;
        m_EndPointsY[ nMaxIndex ].fValue = fMax;

        //////////////////////////////////////////
        // First move the min, left or right
        if( fMin < fOldMin )
        {
            sint nThat = nMinIndex-1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue < m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMax( m_EndPointsY[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }

                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                --nThis;
                --nThat;
            }

            m_Boxes[ nBox ].nMin[1] = nThis;
        }
        else
        {
            sint nThat = nMinIndex+1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue > m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMax( m_EndPointsY[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }


                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                ++nThis;
                ++nThat;
            }

            m_Boxes[ nBox ].nMin[1] = nThis;
        }
        
        //////////////////////////////////////////
        // Then the max, left or right
        if( fMax < fOldMax )
        {
            sint nThat = nMaxIndex-1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue < m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMin( m_EndPointsY[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }

                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                --nThis;
                --nThat;
            }
            m_Boxes[ nBox ].nMax[1] = nThis;
        }
        else
        {
            sint nThat = nMaxIndex+1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsY[ nThis ].fValue > m_EndPointsY[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsY[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMin( m_EndPointsY[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsY[ nThis ].nIndex & eSAPClearMask, m_EndPointsY[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[1] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[1] = nThis;
                }

                Swap( m_EndPointsY[ nThis ], m_EndPointsY[ nThat ] );

                ++nThis;
                ++nThat;
            }
            m_Boxes[ nBox ].nMax[1] = nThis;
        }
    }

    void CCollisionSystem::UpdateZAxis( float fMin, float fMax, uint nBox )
    {
        //////////////////////////////////////////
        // Z Axis first
        sint  nMinIndex = m_Boxes[ nBox ].nMin[2];
        sint  nMaxIndex = m_Boxes[ nBox ].nMax[2];
        sint  nIndex;

        float fOldMin = m_EndPointsZ[ nMinIndex ].fValue;
        float fOldMax = m_EndPointsZ[ nMaxIndex ].fValue;

        m_EndPointsZ[ nMinIndex ].fValue = fMin;
        m_EndPointsZ[ nMaxIndex ].fValue = fMax;

        //////////////////////////////////////////
        // First move the min, left or right
        if( fMin < fOldMin )
        {
            sint nThat = nMinIndex-1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue < m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMax( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }

                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                --nThis;
                --nThat;
            }

            m_Boxes[ nBox ].nMin[2] = nThis;
        }
        else
        {
            sint nThat = nMinIndex+1;
            sint nThis = nMinIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue > m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMax( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }


                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                ++nThis;
                ++nThat;
            }

            m_Boxes[ nBox ].nMin[2] = nThis;
        }
        
        //////////////////////////////////////////
        // Then the max, left or right
        if( fMax < fOldMax )
        {
            sint nThat = nMaxIndex-1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue < m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, register the collision
                if( IsMin( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    RemovePair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }

                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                --nThis;
                --nThat;
            }
            m_Boxes[ nBox ].nMax[2] = nThis;
        }
        else
        {
            sint nThat = nMaxIndex+1;
            sint nThis = nMaxIndex;

            // Move it left until it's no longer less
            while( m_EndPointsZ[ nThis ].fValue > m_EndPointsZ[ nThat ].fValue )
            {
                sint nCmpObj = m_EndPointsZ[ nThat ].nIndex & eSAPClearMask;
                // A min passed a max, remove the collision
                if( IsMin( m_EndPointsZ[ nThat ].nIndex ) )
                {
                    AddPair( m_EndPointsZ[ nThis ].nIndex & eSAPClearMask, m_EndPointsZ[ nThat ].nIndex & eSAPClearMask );
                    m_Boxes[ nCmpObj ].nMin[2] = nThis;
                }
                else
                {
                    m_Boxes[ nCmpObj ].nMax[2] = nThis;
                }

                Swap( m_EndPointsZ[ nThis ], m_EndPointsZ[ nThat ] );

                ++nThis;
                ++nThat;
            }
            m_Boxes[ nBox ].nMax[2] = nThis;
        }
    }

    void CCollisionSystem::RemoveObject( uint nBox )
    {
        --m_nNumBoxes;
        uint nArrayEnd = m_nNumBoxes * 2;

        RAABB inf( RVector3( FLT_MAX - 2.0f, FLT_MAX - 2.0f, FLT_MAX - 2.0f ), RVector3( FLT_MAX - 1.0f, FLT_MAX - 1.0f, FLT_MAX - 1.0f ) );

        // Move the box to infinity, automatically removing all pairs
        UpdateObject( inf, nBox );

        // Now remove it from the array
        m_EndPointsX[ nArrayEnd ] = m_EndPointsX[ nArrayEnd+2 ];
        m_EndPointsY[ nArrayEnd ] = m_EndPointsY[ nArrayEnd+2 ];
        m_EndPointsZ[ nArrayEnd ] = m_EndPointsZ[ nArrayEnd+2 ];
    }

    void CCollisionSystem::AddPair( uint nObject0, uint nObject1 )
    {
        // See if they even overlap first
        if( !Overlap( nObject0, nObject1 ) )
        {
            return;
        }

        uint64 nPairA = nObject0;
        nPairA = ( nPairA << 32) | nObject1;
        uint64 nPairB = nObject1;
        nPairB = ( nPairB << 32) | nObject0;

        // XOR them together so that A colliding with B is the same as
        //  B colliding with A
        uint64 nPair = nPairA + nPairB;

        // See if the collision already exists
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( CheckPair( m_Pairs[i], nObject0, nObject1 ) )
            {
                return;
            }
        }

        // It doesn't, add it
        m_Pairs[ m_nNumPairs ].nPair = nPair;
        m_Pairs[ m_nNumPairs ].nObject0 = nObject0;
        m_Pairs[ m_nNumPairs ].nObject1 = nObject1;

        ++m_nNumPairs;
    }

    void CCollisionSystem::RemovePair( uint nObject0, uint nObject1 )
    {
        uint64 nPairA = nObject0;
        nPairA = ( nPairA << 32) | nObject1;
        uint64 nPairB = nObject1;
        nPairB = ( nPairB << 32) | nObject0;

        // XOR them together so that A colliding with B is the same as
        //  B colliding with A
        uint64 nPair = nPairA + nPairB;

        // Make sure the collision already exists
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            if( CheckPair( m_Pairs[i], nObject0, nObject1 ) )
            {
                Swap( m_Pairs[i], m_Pairs[ --m_nNumPairs ] );
                return;
            }
        }

        // It doesn't, assert for now
        //ASSERT( false );
    }

    /***************************************\
    | class methods                         |
    \***************************************/
    void CCollisionSystem::ProcessObjects( void )
    {
        for( uint i = 0; i < m_nNumPairs; ++i )
        {
            // TODO: something with m_Pairs[i]
                RVector3* v;

                m_pObjects[ m_Pairs[i].nObject0 ]->GetProperty( "velocity", (void**)&v );
                *v = RVector3( 0.0f, 20.0f, 0.0f );
        }

        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            if( m_pTerrain->SphereTerrainCollision( m_pBoundingSpheres[i] ) )
            {
                // TODO: something...
                RVector3* v;

                m_pObjects[i]->GetProperty( "velocity", (void**)&v );
                *v = RVector3Zero();
                m_pObjects[i]->GetProperty( "acceleration", (void**)&v );
                *v = RVector3Zero();
            }

            // TODO: An object just moved. How do we know?
        }
    }

    //-----------------------------------------------------------------------------
    //  AddObject
    //  Adds an object
    //-----------------------------------------------------------------------------
    void CCollisionSystem::AddObject( CObject* pObject )
    {
        RVector3* v;

        pObject->GetProperty( "position", (void**)&v );

        m_pBoundingSpheres[ m_nNumObjects ].position = *v;

        m_pObjects[ m_nNumObjects ] = pObject;

        static const RVector3 infmax( FLT_MAX - 10.0f, FLT_MAX - 10.0f, FLT_MAX - 10.0f );
        static const RVector3 infmin( -(FLT_MAX - 10.0f), -(FLT_MAX - 10.0f), -(FLT_MAX - 10.0f) );

        AddObject( RAABB( infmin, infmax ), m_nNumObjects );

        m_nNumObjects++;
    }

    //-----------------------------------------------------------------------------
    //  CalculateBoundingSphere
    //  Calculates a bounding sphere to surround the input vertices
    //-----------------------------------------------------------------------------
    void CCollisionSystem::CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, CObject* pObject )
    {
        float3 fExtents = {  0 };

        for( uint i = 0; i < nNumVerts; ++i )
        {
            for( uint j = 0; j < 3; ++j )
            {
                if( Abs(pVerts[i].Pos[j]) > fExtents[j] )
                {
                    fExtents[j] = Abs(pVerts[i].Pos[j]);
                }
            }
        }

        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            if( m_pObjects[i] == pObject )
            {
                m_pBoundingSpheres[i].radius = Magnitude( RVector3(fExtents) );
                return;
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  PickObject
    //  Returns the first object hit by the picking ray
    //-----------------------------------------------------------------------------
    CObject* CCollisionSystem::PickObject( RVector3 rayOrigin, RVector3 rayDir )
    {
        // TODO: Pick shit Omar!!!
        return 0;
    }

} // namespace Riot
