/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/26/2011 10:21:02 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTCOLLIDABLE_H_
#define _COMPONENTCOLLIDABLE_H_
#include "IComponent.h"
#include "VertexFormats.h"
#include "Terrain.h"

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
    private:
        static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );

        struct Triangle;
        bool IsPointInTriangle( const RVector3& point, const Triangle& triangle );

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

        BoundingVolume  m_Volume[MaxComponents];
        VolumeType      m_nVolumeType[MaxComponents];

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

            float DistanceFrom( const float3& fPoint )
            {
                float fDot = DotProduct( RVector3( fPoint ), vNormal );
                return fDot + fPlaneEquation[3];
            }
        };
    };

} // namespace Riot

#endif // #ifndef _COMPONENTCOLLIDABLE_H_
