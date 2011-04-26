/*********************************************************\
File:           ComponentCollidable.h
Purpose:        Allows an object to collide with others or
                be collided with
Author:         Kyle Weicht
Created:        4/25/2011
Modified:       4/25/2011 9:50:41 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _COMPONENTCOLLIDABLE_H_
#define _COMPONENTCOLLIDABLE_H_
#include "IComponent.h"
#include "VertexFormats.h"

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

        static void CalculateBoundingSphere( const VPosNormalTex* pVerts, uint nNumVerts, uint nIndex );
    private:
        static void ProcessBatch( void* pData, uint nThreadId, uint nStart, uint nCount );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CComponentCollidable* m_pInstance;


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
        VolumeType  m_nVolumeType[MaxComponents];

    };

} // namespace Riot

#endif // #ifndef _COMPONENTCOLLIDABLE_H_
