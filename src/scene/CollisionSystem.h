/*********************************************************\
File:           CollisionSystem.h
Purpose:        
Author:         Kyle Weicht
Created:        5/24/2011
Modified:       5/24/2011 4:17:04 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COLLISIONSYSTEM_H_
#define _COLLISIONSYSTEM_H_
#include "common.h"
#include "vertexformats.h"

namespace Riot
{
    class CObject;
    class CTerrain;

    class CCollisionSystem
    {
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

        static TSAPPair    m_Pairs[ MAX_OBJECTS ];
        static TSAPBox     m_Boxes[ MAX_OBJECTS ];
        static TEndPoint   m_EndPointsX[ MAX_OBJECTS * 2 ];
        static TEndPoint   m_EndPointsY[ MAX_OBJECTS * 2 ];
        static TEndPoint   m_EndPointsZ[ MAX_OBJECTS * 2 ];
        static uint        m_nNumPairs;
        static uint        m_nNumBoxes;

        uint AddObject( const RAABB& box, uint nObject );
        void UpdateObject( const RAABB& box, uint nBox );
        void UpdateObject( RAABB box, CObject* pObject );
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

        // CCollisionSystem constructor
        CCollisionSystem();

        // CCollisionSystem destructor
        ~CCollisionSystem();
        /***************************************\
        | class methods                         |
        \***************************************/
        
        //-----------------------------------------------------------------------------
        //  AddObject
        //  Adds an object
        //-----------------------------------------------------------------------------
        static void AddObject( CObject* pObject );

        //-----------------------------------------------------------------------------
        //  CalculateBoundingSphere
        //  Calculates a bounding sphere to surround the input vertices
        //-----------------------------------------------------------------------------
        static void CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, CObject* pObject );

        //-----------------------------------------------------------------------------
        //  PickObject
        //  Returns the first object hit by the picking ray
        //-----------------------------------------------------------------------------
        CObject* PickObject( RVector3 rayOrigin, RVector3 rayDir ); 
        
        //-----------------------------------------------------------------------------
        //  Accessors/mutators
        //-----------------------------------------------------------------------------
        static inline void SetTerrain( CTerrain* pTerrain );

        static void ProcessObjects( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CObject*     m_pObjects[ MAX_OBJECTS ];
        static RSphere      m_pBoundingSpheres[ MAX_OBJECTS ];
        static uint         m_nNumObjects;

        static CTerrain*    m_pTerrain;
    };
    
    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    void CCollisionSystem::SetTerrain( CTerrain* pTerrain )
    {
        m_pTerrain = pTerrain;
    }
} // namespace Riot

#endif // #ifndef _COLLISIONSYSTEM_H_
