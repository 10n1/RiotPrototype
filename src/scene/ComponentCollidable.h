/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/28/2011 10:36:18 PM
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
    public:
        struct Triangle
        {
            RVector3    vVerts[3];
            RVector3    vNormal;
        };

        //-----------------------------------------------------------------------------
        //  TSceneNode
        //  Basic interface for all other nodes (Terrain, Object and Parent)
        //-----------------------------------------------------------------------------
        struct TSceneNode : public RAABB
        {
            TSceneNode* m_pParent;

            void DrawNode( CRenderer* pRenderer, const RVector3& vColor)
            {
                pRenderer->DrawDebugBox( *this, vColor );
            }
        };
        
        //-----------------------------------------------------------------------------
        //  TSceneParentNode
        //  The non-object/terrain nodes. All this parent node does is hold them
        //-----------------------------------------------------------------------------
        struct TSceneParentNode : public TSceneNode
        {
            TSceneNode* m_pChildren[4];
            uint16      m_nNumChildren;
            uint16      m_nLowestParent;

            TSceneParentNode()
            {
                m_pChildren[0] = NULL;
                m_pChildren[1] = NULL;
                m_pChildren[2] = NULL;
                m_pChildren[3] = NULL;

                m_nNumChildren  = 0;
                m_nLowestParent = 0;
            }
        };
        
        //-----------------------------------------------------------------------------
        //  TTerrainLeafNode
        //  Holds the terrain triangle data
        //-----------------------------------------------------------------------------
        struct TTerrainLeafNode : public TSceneNode
        {
            Triangle*   m_pTri[2];
            uint        m_nNumTri;
            TTerrainLeafNode()
            {
                m_pTri[0] = NULL;
                m_pTri[1] = NULL;
                m_nNumTri = 0;
            }

            bool SphereTriangleCollision( const RSphere& s )
            {
                RVector3    fPosition = RVector3(s.position);
                float       fRadius   = s.radius;

                for( uint i = 0; i < 2; ++i )
                {
                    RPlane trianglePlane( m_pTri[i]->vVerts[0], m_pTri[i]->vVerts[1], m_pTri[i]->vVerts[2] );

                    float fDistance = DistanceFromPlane( trianglePlane, s.position );

                    if( fDistance > s.radius )
                    {
                        // The sphere doesn't interact the triangles plane
                        continue;
                    }
                
                    RVector3 planeCollisionPoint = fPosition - trianglePlane.normal;
                    if( CComponentCollidable::IsPointInTriangle( planeCollisionPoint, *m_pTri[i] ) )
                    {
                        // we collided, break
                        return true;
                    }
                }

                return false;
            }
        };
        
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

        //-----------------------------------------------------------------------------
        //  BuildParentNodes
        //  Constructs the top of the tree
        //-----------------------------------------------------------------------------
        void BuildParentNodes( TSceneParentNode* pNode, TSceneParentNode* pParent );

        //-----------------------------------------------------------------------------
        //  AddTriangleToGraph
        //  Adds a triangle to the graph
        //-----------------------------------------------------------------------------
        void AddTriangleToGraph( TSceneNode* pNode, Triangle* pTriangle, bool bLeaf = false );

        //-----------------------------------------------------------------------------
        //  RecomputeSceneGraphBounds
        //  Recomputes the bounds of the top of the tree
        //-----------------------------------------------------------------------------
        void RecomputeSceneGraphBounds( CComponentCollidable::TSceneNode* pNode );
        
        //-----------------------------------------------------------------------------
        //  SphereTerrainCollision
        //  Determines if a tree hits any triangles within the node
        //-----------------------------------------------------------------------------
        bool SphereTerrainCollision( TSceneNode* pNode, const RSphere& s );
        
        //-----------------------------------------------------------------------------
        //  DrawNodes
        //  Draws all nodes down to a specific depth
        //-----------------------------------------------------------------------------
        void DrawNodes( TSceneParentNode* pNode, uint nDepth );

    private:
        static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );
        
        //-----------------------------------------------------------------------------
        //  IsPointInTriangle
        //  Determines if a point is inside a particular triangle
        //-----------------------------------------------------------------------------
        // Reference: http://www.peroxide.dk/papers/collision/collision.pdf
        static bool IsPointInTriangle( const RVector3& point, const Triangle& triangle );
        

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentCollidable* m_pInstance;

        static const uint nNumTriangles = CTerrain::TERRAIN_HEIGHT * CTerrain::TERRAIN_WIDTH * 2;
        
        RSphere     m_Volume[MaxComponents];

        Triangle    m_pTerrainTriangles[nNumTriangles];

        TSceneParentNode*   m_pTerrainGraph;
        TSceneParentNode*   m_pParentNodes;
        TTerrainLeafNode*   m_pTerrainLeaves;

        uint                m_nNumParentNodes;
        uint                m_nNumTerrainLeaves;
     
    };

} // namespace Riot

#endif // #ifndef _COMPONENTCOLLIDABLE_H_
