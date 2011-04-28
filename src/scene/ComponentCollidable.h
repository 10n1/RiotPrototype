/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/28/2011 1:03:43 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTCOLLIDABLE_H_
#define _COMPONENTCOLLIDABLE_H_
#include "IComponent.h"
#include "VertexFormats.h"
#include "Terrain.h"
#include "Renderer.h"

/*
CComponentCollidable
0
2
BoundingVolume m_Volume
VolumeType m_nVolumeType
*/

namespace Riot
{

    class CComponentCollidable : public IComponent
    {
        union BoundingVolume
        {
            struct _sphere
            {
                float3  position;
                float   radius; // Radius is stored pre-squared
            } sphere;
            struct _AABB
            {
                float3  min;
                float3  max;
            } aabb;
        };

        class SceneNode;
        friend class CObjectManager;
    public:
        // CComponentCollidable constructor
        CComponentCollidable();

        // CComponentCollidable destructor
        ~CComponentCollidable();
        /***************************************\
        | class methods                         |
        \***************************************/

        void Attach( uint nObject );
        void Reattach( uint nObject );
        void Detach( uint nObject );
        void DetachAndSave( uint nObject );
        void ProcessComponent( void );
        void ReceiveMessage( uint nSlot, CComponentMessage& msg );
        void RemoveInactive( uint nObject );

        static const eComponentType ComponentType = eComponentCollidable;
        static const uint MaxComponents = IComponent::MaxComponents;
        static const eComponentMessageType MessagesReceived[];
        static const uint NumMessagesReceived;
        
        //-----------------------------------------------------------------------------
        //  CalculateBoundingSphere
        //  Calculates a bounding sphere to surround the input vertices
        //-----------------------------------------------------------------------------
        static void CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, uint nIndex );
        
        //-----------------------------------------------------------------------------
        //  SetTerrainData
        //  Sets the terrain data so objects can collide with it
        //-----------------------------------------------------------------------------
        static void SetTerrainData( const VPosNormalTex* pTerrainVerts, uint nNumVerts, const uint16* pIndices, uint nNumIndices );

        //-----------------------------------------------------------------------------
        //  BuildSceneGraph
        //  Builds the scene graph
        //-----------------------------------------------------------------------------
        static void BuildSceneGraph( void );

    private:
        static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );

        struct Triangle;
        static bool IsPointInTriangle( const RVector3& point, const Triangle& triangle );

        static bool DoesSphereHitTriangle( SceneNode* pNode, const BoundingVolume::_sphere& s );
        static bool AddTriangleToGraph( const Triangle& triangle );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentCollidable* m_pInstance;

        static const uint nNumTriangles = CTerrain::TERRAIN_HEIGHT * CTerrain::TERRAIN_WIDTH * 2;
        
        struct Triangle
        {
            RVector3    vVerts[3];
            RVector3    vNormal;
        };
        Triangle m_pTerrainTriangles[nNumTriangles];

        enum VolumeType
        {
            BoundingSphere,
            AABB,
        };

        BoundingVolume  m_Volume[MaxComponents];
        VolumeType      m_nVolumeType[MaxComponents];

    public:
        struct Plane
        {
            float4      fPlaneEquation;
            RVector3    vOrigin;
            RVector3    vNormal;

            Plane() { }

            Plane( Triangle& tri )
            {
                vNormal = tri.vNormal;
                vOrigin = tri.vVerts[0];

                fPlaneEquation[0] = vNormal.x;
                fPlaneEquation[1] = vNormal.y;
                fPlaneEquation[2] = vNormal.z;
                fPlaneEquation[3] = -(  vNormal.x * vOrigin.x + 
                                        vNormal.y * vOrigin.y + 
                                        vNormal.z * vOrigin.z );
            }

            Plane( const RVector3& norm, const RVector3& point )
            {
                vNormal = norm;
                vOrigin = point;

                fPlaneEquation[0] = vNormal.x;
                fPlaneEquation[1] = vNormal.y;
                fPlaneEquation[2] = vNormal.z;
                fPlaneEquation[3] = -(  vNormal.x * vOrigin.x + 
                                        vNormal.y * vOrigin.y + 
                                        vNormal.z * vOrigin.z );
            }

            float DistanceFrom( const float3& fPoint )
            {
                float fDot = DotProduct( RVector3( fPoint ), vNormal );
                return fDot + fPlaneEquation[3];
            }
        };

        static uint nTemp;

    public:
        struct SceneNode
        {
            RVector3    vMin;
            RVector3    vMax;

            SceneNode*  pChildren; 
            Triangle*   pTri[2];

            uint8       nTri;

            SceneNode()
            {
                pChildren = NULL;

                pTri[0] = NULL;
                pTri[1] = NULL;

                nTri = 0;
            }

            ~SceneNode()
            {
                SAFE_DELETE_ARRAY( pChildren );
            }

            bool DoesSphereHitBox(  const BoundingVolume::_sphere& s )
            {
                float fRadius = sqrtf( s.radius );

                // Test top
                Plane pTop( RVector3( 0.0f, 1.0f, 0.0f ),vMax );
                float fTop = pTop.DistanceFrom( s.position );  

                Plane pRight( RVector3( 1.0f, 0.0f, 0.0f ),vMax );
                float fRight = pRight.DistanceFrom( s.position );

                Plane pFar( RVector3( 0.0f, 0.0f, 1.0f ),vMax );
                float fFar = pFar.DistanceFrom( s.position );

                Plane pBottom( RVector3( 0.0f, -1.0f, 0.0f ),vMin );
                float fBottom = pBottom.DistanceFrom( s.position );  

                Plane pLeft( RVector3( -1.0f, 0.0f, 0.0f ),vMin );
                float fLeft = pLeft.DistanceFrom( s.position );

                Plane pNear( RVector3( 0.0f, 0.0f, -1.0f ),vMin );
                float fNear = pNear.DistanceFrom( s.position );


                // The box is too far away
                if(    fTop     > fRadius
                    || fBottom  > fRadius   
                    || fRight   > fRadius
                    || fLeft    > fRadius
                    || fNear    > fRadius
                    || fFar     > fRadius )
                {
                    return false;
                }

                return true;
            }

            void DrawNode( CRenderer* pRenderer, const RVector3& vColor, uint nDepth )
            {
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


                if( nDepth == 0 || pChildren == NULL )
                {
                    if( nTemp++ < 1000 )
                pRenderer->DrawDebugBox( vMin, vMax, vColors[nDepth] );
                    return;
                }

                for( uint i = 0; i < 4; ++i )
                {
                    pChildren[i].DrawNode( pRenderer, vColors[nDepth], nDepth-1 );
                }
            };

            bool IsPointInNode( const RVector3& vPoint )
            {
                if(    vPoint.x >= vMin.x
                    && vPoint.y >= vMin.y 
                    && vPoint.z >= vMin.z
                    && vPoint.x <= vMax.x
                    && vPoint.y <= vMax.y 
                    && vPoint.z <= vMax.z )
                {
                    return true;
                }

                return false;
            }

            bool DoesSphereHitTriangle( const BoundingVolume::_sphere& s )
            {
                if( pChildren )
                    return false;

                
                RVector3    fPosition = RVector3(s.position);
                float       fRadius   = s.radius;

                for( uint i = 0; i < 2; ++i )
                {
                    Plane trianglePlane( *pTri[i] );

                    float fDistance = trianglePlane.DistanceFrom( s.position );

                    if( fDistance > sqrtf(s.radius) )
                    {
                        // The sphere doesn't interact the triagnles plane
                        continue;
                    }
                
                    RVector3 planeCollisionPoint = fPosition - trianglePlane.vNormal;
                    if( CComponentCollidable::IsPointInTriangle( planeCollisionPoint, *pTri[i] ) )
                    {
                        // we collided, break
                        return true;
                    }
                }
            }

            void AddTriangleToNode( Triangle* tri )
            {
                for( int i = 0; i < 3; ++i )
                {
                    if(    tri->vVerts[i].x > vMax.x
                        || tri->vVerts[i].y > vMax.y
                        || tri->vVerts[i].z > vMax.z
                        || tri->vVerts[i].x < vMin.x
                        || tri->vVerts[i].y < vMin.y
                        || tri->vVerts[i].z < vMin.z )
                    {
                        return;
                    }
                }

                if( pChildren )
                {
                    for( int i = 0; i < 4; ++i )
                    {
                        pChildren[i].AddTriangleToNode( tri );
                    }
                }
                else
                {
                    pTri[nTri++] = tri;

                    if( nTri == 2 )
                    {
                        vMax = RVector3( -10000.0f, -10000.0f, -10000.0f );
                        vMin = RVector3( 10000.0f, 10000.0f, 10000.0f );
                        for( uint i = 0; i < 3; ++i )
                        {
                            //vMax.x = Max( tri->vVerts[i].x, vMax.x );
                            //vMax.y = Max( tri->vVerts[i].y, vMax.y );
                            //vMax.z = Max( tri->vVerts[i].z, vMax.z );
                            //vMin.x = Min( tri->vVerts[i].x, vMin.x );
                            //vMin.y = Min( tri->vVerts[i].y, vMin.y );
                            //vMin.z = Min( tri->vVerts[i].z, vMin.z );

                            vMax.x = Max( pTri[0]->vVerts[i].x, Max( pTri[1]->vVerts[i].x, vMax.x ) );
                            vMax.y = Max( pTri[0]->vVerts[i].y, Max( pTri[1]->vVerts[i].y, vMax.y ) );
                            vMax.z = Max( pTri[0]->vVerts[i].z, Max( pTri[1]->vVerts[i].z, vMax.z ) );
                            vMin.x = Min( pTri[0]->vVerts[i].x, Min( pTri[1]->vVerts[i].x, vMin.x ) );
                            vMin.y = Min( pTri[0]->vVerts[i].y, Min( pTri[1]->vVerts[i].y, vMin.y ) );
                            vMin.z = Min( pTri[0]->vVerts[i].z, Min( pTri[1]->vVerts[i].z, vMin.z ) );
                        }
                    }
                }
            }
        };

        SceneNode*  m_pGraph;

        static void BuildLeafNodes( SceneNode* pNode, Triangle* pTriangles );         
    };

} // namespace Riot

#endif // #ifndef _COMPONENTCOLLIDABLE_H_
