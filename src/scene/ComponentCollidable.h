/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/11/2011 10:26:53 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTCOLLIDABLE_H_
#define _COMPONENTCOLLIDABLE_H_
#include "IComponent.h"
#include "VertexFormats.h"
//#include "Terrain.h"
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
    class CTerrain;

    class CComponentCollidable : public IComponent
    {
        friend struct TBSPNode;
    public:
        enum
        {
            eSAPBit = 0x80000000,
            eSAPMinMask = 0x80000000,
            eSAPMaxMask = 0x7FFFFFFF,
            eSAPClearMask = 0x7FFFFFFF,
        };

        //-----------------------------------------------------------------------------
        //  TEndPoint
        //  An end point for the SAP algorithm
        //-----------------------------------------------------------------------------
        struct TEndPoint
        {
            uint nIndex;
            float fValue;
        };
                
        //-----------------------------------------------------------------------------
        //  TSAPBox
        //  A box for the SAP algorithm
        //-----------------------------------------------------------------------------
        struct TSAPBox
        {
            uint nMin[3];
            uint nMax[3];
            uint nObject;
        };
          
        //-----------------------------------------------------------------------------
        //  TSAPPair
        //  A box for the SAP algorithm
        //-----------------------------------------------------------------------------
        struct TSAPPair
        {
            uint64 nPair;   // Left 32 bits are object 1, right 32 are object 2
            uint   nObject0;
            uint   nObject1;
            //uint   nCount;  // Overlap count. Only actually overlapping when this is 3
        };

        TSAPPair    m_Pairs[ MaxComponents ];
        TSAPBox     m_Boxes[ MaxComponents ];
        TEndPoint   m_EndPointsX[ MaxComponents * 2 ];
        TEndPoint   m_EndPointsY[ MaxComponents * 2 ];
        TEndPoint   m_EndPointsZ[ MaxComponents * 2 ];

        uint        m_nNumPairs;
        uint        m_nNumBoxes;

        uint AddObject( const RAABB& box, uint nObject );
        void UpdateObject( const RAABB& box, uint nBox );
        void UpdateXAxis( float fMin, float fMax, uint nBox );
        void UpdateYAxis( float fMin, float fMax, uint nBox );
        void UpdateZAxis( float fMin, float fMax, uint nBox );
        void RemoveObject( uint nBox );

        void AddPair( uint nObject0, uint nObject1 );
        void RemovePair( uint nObject0, uint nObject1 );

        inline bool Overlap( uint nObject0, uint nObject1 )
        {
            //for( uint j = 0; j < 3; ++j )
            //{
            //    uint i = 1;
            //    if( m_Boxes[nObject0].nMax[i] < m_Boxes[nObject1].nMin[i] )
            //        return false;
            //    
            //    if( m_Boxes[nObject1].nMax[i] < m_Boxes[nObject0].nMin[i] )
            //        return false;
            //}
            if( m_EndPointsX[ m_Boxes[nObject0].nMax[0] ].fValue < m_EndPointsX[ m_Boxes[nObject1].nMin[0] ].fValue )
                return false;
            if( m_EndPointsX[ m_Boxes[nObject1].nMax[0] ].fValue < m_EndPointsX[ m_Boxes[nObject0].nMin[0] ].fValue )
                return false;

            if( m_EndPointsY[ m_Boxes[nObject0].nMax[1] ].fValue < m_EndPointsY[ m_Boxes[nObject1].nMin[1] ].fValue )
                return false;
            if( m_EndPointsY[ m_Boxes[nObject1].nMax[1] ].fValue < m_EndPointsY[ m_Boxes[nObject0].nMin[1] ].fValue )
                return false;

            if( m_EndPointsZ[ m_Boxes[nObject0].nMax[2] ].fValue < m_EndPointsZ[ m_Boxes[nObject1].nMin[2] ].fValue )
                return false;
            if( m_EndPointsZ[ m_Boxes[nObject1].nMax[2] ].fValue < m_EndPointsZ[ m_Boxes[nObject0].nMin[2] ].fValue )
                return false;

            return true;
        }

        inline uint IsMin( uint nIndex ) 
        {
            return nIndex & eSAPBit;
        }

        inline uint IsMax( uint nIndex )
        {
            return !IsMin( nIndex );
        }

        inline bool CheckPair( TSAPPair& pair, uint nObj0, uint nObj1 )
        {
            if( (  pair.nObject0 == nObj0 
                && pair.nObject1 == nObj1 ) ||
                (  pair.nObject1 == nObj0 
                && pair.nObject0 == nObj1 ) )
                return true;

            return false;
        }

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
                for( sint i = 0; i < m_nNumChildren; ++i )
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
                for( sint i = 0; i < m_nNumChildren; ++i )
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

                    for( sint i = 0; i < m_nNumChildren; ++i )
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
                        for( sint i = 0; i < m_nNumChildren; ++i )
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
                        AddObjectLeaf( pNode );
                        m_Mutex.Unlock();
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
                CScopedMutex lock( &m_Mutex );

                if( m_nNumChildren == 0 )
                {
                    // There should never be 0 children, but it has happened....
                    //  it technically shouldn't be a problem
                    ASSERT( m_nNumChildren );
                    return false;
                }

                uint nIndex = 0;
                while( m_pChildren[nIndex] != pNode )
                {
                    nIndex++;
                    if( nIndex == 8 )
                        break;
                }

                //ASSERT( nIndex < 8 );
                if( nIndex == 8 )
                {
                    // This node isn't one of our children,
                    //  so we can't remove it
                    return false;
                }

                m_pChildren[nIndex] = m_pChildren[--m_nNumChildren];

                // Make sure we're not still our child's parent
                pNode->m_pParent = NULL;

                return true;
            }
            
            void SplitNode( void )
            {
                // This node has 8 children and is going
                //  to be split into 8 sub parent nodes,
                //  redistrubting the objects to the subnodes
                TSceneNode* pChildren[] = 
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
        //  SphereTerrainCollision
        //  Determines if a tree hits any triangles within the node
        //-----------------------------------------------------------------------------
        bool SphereTerrainCollision( const RSphere& s );
        
        //-----------------------------------------------------------------------------
        //  ObjectObjectCollision
        //  Performs object-object collision
        //-----------------------------------------------------------------------------
        void ObjectObjectCollision( TSceneNode* pGraph, TSceneNode* pNode );

        //-----------------------------------------------------------------------------
        //  Accessors/mutators
        //-----------------------------------------------------------------------------
        static inline void SetTerrain( CTerrain* pTerrain );

    private:
        static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );        

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentCollidable* m_pInstance;

        RSphere             m_Volume[MaxComponents];
        TObjectLeafNode     m_ObjectSceneNodes[MaxComponents];
        TObjectParentNode*  m_pObjectGraph;     

        CTerrain*           m_pTerrain;
    };

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    void CComponentCollidable::SetTerrain( CTerrain* pTerrain )
    {
        m_pInstance->m_pTerrain = pTerrain;
    }

} // namespace Riot

#endif // #ifndef _COMPONENTCOLLIDABLE_H_
