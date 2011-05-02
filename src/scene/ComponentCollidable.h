/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/2/2011 1:27:05 PM
Modified by:    Kyle Weicht

210fps 4k objects
\*********************************************************/
#ifndef _COMPONENTCOLLIDABLE_H_
#define _COMPONENTCOLLIDABLE_H_
#include "IComponent.h"
#include "VertexFormats.h"
#include "Terrain.h"
#include "Renderer.h"
#include "Thread.h"

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
            CMutex      m_Mutex;

            virtual ~TSceneNode() { }

            virtual void DrawNode( CRenderer* pRenderer, const RVector3& vColor )
            {
                pRenderer->DrawDebugBox( *this, vColor );
            }
        };


        //-----------------------------------------------------------------------------
        //  TObjectParentNode
        //  The non-object nodes. All this parent node does is hold them
        //-----------------------------------------------------------------------------
        struct TObjectLeafNode;
        struct TObjectParentNode : public TSceneNode
        {
            TSceneNode* m_pChildren[8];
            atomic_t    m_nNumChildren;
            uint8       m_nLowestParent;
            uint8       m_nInvalid;

            void DrawNode(  CRenderer* pRenderer, const RVector3& vColor )
            {
                TSceneNode::DrawNode( pRenderer, vColor );
                for( uint i = 0; i < m_nNumChildren; ++i )
                {
                    m_pChildren[i]->DrawNode( pRenderer, vColor );
                }
            }

            TObjectParentNode()
            {
                m_pChildren[0] = NULL;
                m_pChildren[1] = NULL;
                m_pChildren[2] = NULL;
                m_pChildren[3] = NULL;
                m_pChildren[4] = NULL;
                m_pChildren[5] = NULL;
                m_pChildren[6] = NULL;
                m_pChildren[7] = NULL;

                m_nNumChildren  = 0;
                m_nLowestParent = 1;
                m_nInvalid      = 0;
            }

            ~TObjectParentNode()
            {
                for( uint i = 0; i < m_nNumChildren; ++i )
                {
                    SAFE_DELETE( m_pChildren[i] );
                }
            }

            void Prune()
            {
                // We have children and they're objects,
                //  so just return
                if( m_nLowestParent && m_nNumChildren )
                {
                    return;
                }

                // If we don't have children, don't do anything (the parent)
                if( m_nNumChildren )
                {
                    uint nLowestChildren = 0;

                    for( uint i = 0; i < m_nNumChildren; ++i )
                    {
                        TObjectParentNode* pChild = (TObjectParentNode*)m_pChildren[i];

                        // If its not a leaf, prune it
                        if( !pChild->m_nLowestParent )
                        {
                            pChild->Prune();
                        }
                        else if( pChild->m_nLowestParent == 1 && pChild->m_nNumChildren == 0 )
                        {
                            // This child is an empty leaf
                            ++nLowestChildren;
                        }
                    }

                    // All our children are empty and the lowest level
                    if( nLowestChildren == 8 )
                    {
                        for( uint i = 0; i < m_nNumChildren; ++i )
                        {
                            delete m_pChildren[i];
                            m_pChildren[i] = NULL;
                        }
                        
                        // We're now the lowest
                        m_nLowestParent = 1;
                        m_nNumChildren = 0;
                    }
                }
            }

            void AddObjectLeaf( TSceneNode* pNode )
            {
                if( !m_nLowestParent )
                {
                    RVector3 nodePos = ComputePos();
                    RVector3 leafPos = pNode->ComputePos();

                    uint nOffset = 0;
                    if( leafPos.y > nodePos.y )
                        nOffset += 4;
                    if( leafPos.x > nodePos.x )
                    {
                        nOffset += 2;
                        if( leafPos.z < nodePos.z )
                            nOffset += 1;
                    }
                    else
                    {
                        if( leafPos.z > nodePos.z )
                            nOffset += 1;
                    }

                    ((TObjectParentNode*)m_pChildren[ nOffset ])->AddObjectLeaf( pNode );
                }
                else
                {
                    m_Mutex.Lock();
                    if( !m_nLowestParent )
                    {
                        // We were split, add us to a child
                        m_Mutex.Unlock();
                        AddObjectLeaf( pNode );
                        return;
                    }
                    uint nIndex = m_nNumChildren++;
                    m_pChildren[ nIndex ] = pNode;
                    pNode->m_pParent = this;

                    if( m_nNumChildren == 8 )
                    {
                        SplitNode();
                    }
                    m_Mutex.Unlock();
                }
            }
            
            bool RemoveObject( TSceneNode* pNode )
            {
                m_Mutex.Lock();
                ASSERT( m_nNumChildren );

                uint nIndex = 0;
                while( m_pChildren[nIndex] != pNode )
                {
                    nIndex++;
                }

                if( nIndex == 8 )
                {
                    // This node isn't one of our children,
                    //  so we can't remove it
                    return false;
                }

                m_pChildren[nIndex] = m_pChildren[--m_nNumChildren];

                // Make sure we're not still our child's parent
                pNode->m_pParent = NULL;
                m_Mutex.Unlock();

                return true;
            }
            
            void SplitNode( void )
            {
                // This node has 8 children and is going
                //  to be split into 8 sub parent nodes,
                //  redistrubting the objects to the subnodes
                TSceneNode* pChildren[8] = 
                {
                    m_pChildren[0],
                    m_pChildren[1],
                    m_pChildren[2],
                    m_pChildren[3],
                    m_pChildren[4],
                    m_pChildren[5],
                    m_pChildren[6],
                    m_pChildren[7],
                };

                float fNewX = (max.x + min.x) / 2.0f;
                float fNewY = (max.y + min.y) / 2.0f;
                float fNewZ = (max.z + min.z) / 2.0f;

                TObjectParentNode* pNewNode = NULL;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->max.x = fNewX;
                pNewNode->max.y = fNewY;
                pNewNode->max.z = fNewZ;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[0] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->min.z = fNewZ;
                pNewNode->max.y = fNewY;
                pNewNode->max.x = fNewX;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[1] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->min.x = fNewX;
                pNewNode->max.y = fNewY;
                pNewNode->min.z = fNewZ;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[2] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->min.x = fNewX;
                pNewNode->max.y = fNewY;
                pNewNode->max.z = fNewZ;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[3] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->max.x = fNewX;
                pNewNode->min.y = fNewY;
                pNewNode->max.z = fNewZ;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[4] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->min.z = fNewZ;
                pNewNode->min.y = fNewY;
                pNewNode->max.x = fNewX;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[5] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->min.x = fNewX;
                pNewNode->min.y = fNewY;
                pNewNode->min.z = fNewZ;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[6] = pNewNode;

                pNewNode = new TObjectParentNode;
                pNewNode->m_pParent = this;
                pNewNode->min = min;
                pNewNode->max = max;
                pNewNode->min.x = fNewX;
                pNewNode->min.y = fNewY;
                pNewNode->max.z = fNewZ;
                pNewNode->m_nLowestParent = 1;
                m_pChildren[7] = pNewNode;

                m_nLowestParent = 0;

                //m_Mutex.Unlock();
                for( uint i = 0; i < 8; ++i )
                {
                    AddObjectLeaf( pChildren[i] );
                }
            }
        };

        
        //-----------------------------------------------------------------------------
        //  TObjectLeafNode
        //  Holds objects
        //-----------------------------------------------------------------------------
        struct TObjectLeafNode : public TSceneNode
        {
            uint    m_nObject;
        };
        
        //-----------------------------------------------------------------------------
        //  TTerrainParentNode
        //  The non-terrain nodes. All this parent node does is hold them
        //-----------------------------------------------------------------------------
        struct TTerrainParentNode : public TSceneNode
        {
            TSceneNode* m_pChildren[4];
            uint16      m_nNumChildren;
            uint16      m_nLowestParent;

            TTerrainParentNode()
                : TSceneNode()
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
                for( uint i = 0; i < 2; ++i )
                {
                    RPlane trianglePlane( m_pTri[i]->vVerts[0], m_pTri[i]->vVerts[1], m_pTri[i]->vVerts[2] );

                    float fDistance = DistanceFromPlane( trianglePlane, s.position );

                    if( fDistance > s.radius )
                    {
                        // The sphere doesn't interact the triangles plane
                        continue;
                    }
                
                    RVector3 planeCollisionPoint = s.position - trianglePlane.normal;
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
        void BuildParentNodes( TTerrainParentNode* pNode, TTerrainParentNode* pParent );

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
        //  ObjectObjectCollision
        //  Performs object-object collision
        //-----------------------------------------------------------------------------
        void ObjectObjectCollision( TSceneNode* pGraph, TSceneNode* pNode );
        
        //-----------------------------------------------------------------------------
        //  DrawNodes
        //  Draws all nodes down to a specific depth
        //-----------------------------------------------------------------------------
        void DrawNodes( TTerrainParentNode* pNode, uint nDepth );

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
        
        RSphere         m_Volume[MaxComponents];
        TObjectLeafNode m_ObjectSceneNodes[MaxComponents];

        Triangle        m_pTerrainTriangles[nNumTriangles];

        TTerrainParentNode* m_pTerrainGraph;
        TTerrainParentNode* m_pParentNodes;
        TTerrainLeafNode*   m_pTerrainLeaves;

        TObjectParentNode*  m_pObjectGraph;

        uint                m_nNumParentNodes;
        uint                m_nNumTerrainLeaves;
     
    };

} // namespace Riot

#endif // #ifndef _COMPONENTCOLLIDABLE_H_
