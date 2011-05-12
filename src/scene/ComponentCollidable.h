/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       5/11/2011 10:33:22 PM
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
