/*********************************************************\
File:           Object.h
Purpose:        Objects
Author:         Kyle Weicht
Created:        5/23/2011
Modified:       5/23/2011 8:30:22 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "common.h"

namespace Riot
{
    class CObject;

    enum eDataType
    {
        eTypeNull = 0,
        eTypeFloat3,
        eTypeInt,
        eTypeFloat,
        eTypeBool,
        eTypeQuaternion,
        eTypeSystem,
    };

    enum eControllerSystems
    {
        eSystemRender    = 0x1,
        eSystemPhysics   = 0x2,
        eSystemCollision = 0x4,
    };

    template<class T>
    struct Data
    {
        uint32      nNameHash;
        eDataType    nType:24;
        uint32      nOffset:8;
        T           data;
    };

    struct TDataType
    {
        uint32      nNameHash;
        eDataType    nType;
    };

    typedef Data<RVector3>      DataFloat3;
    typedef Data<sint32>        DataInt;
    typedef Data<float>         DataFloat;
    typedef Data<bool>          DataBool;
    typedef Data<RQuaternion>   DataQuaternion;

    struct TObjectDefinition
    {
        uint32  nType[ 128 ];
        uint32  nTypeHash[ 128 ];
        uint32  nNumProperties;
        uint32  nNameHash;
        uint32  nSystems;
    };

    class CObject
    {
        friend class CObjectManager;
    public:
        // CObject constructor
        CObject();

        // CObject destructor
        ~CObject();
        /***************************************\
        | class methods                         |
        \***************************************/
        void CreateObjectFromFile( const char* szFilename );

        void GetProperty( const char* szProp, void** pData );

        void AddProperty( uint32 nTypeHash, uint32 nNameHash );
        
    private:
        /***************************************\
        | class members                         |
        \***************************************/
        void*   m_pData;
        sint32  m_nNumProperties;
        sint32  m_nNameHash;
    };


    inline eDataType GetDataType( uint32 nTypeHash )
    {
        static const uint32 nFloat3Hash = StringHash32( "float3" );
        static const uint32 nBoolHash = StringHash32( "bool" );
        static const uint32 nIntHash = StringHash32( "int" );
        static const uint32 nFloatHash = StringHash32( "float" );
        static const uint32 nQuaternionHash = StringHash32( "quaternion" );
        static const uint32 nSystemHash = StringHash32( "system" );

        if( nTypeHash == nFloat3Hash )
        {
            return eTypeFloat3;                
        }
        if( nTypeHash == nBoolHash )
        {
            return eTypeBool;                
        }
        if( nTypeHash == nIntHash )
        {
            return eTypeInt;                
        }
        if( nTypeHash == nFloatHash )
        {
            return eTypeFloat;                
        }
        if( nTypeHash == nQuaternionHash )
        {
            return eTypeQuaternion;
        }
        if( nTypeHash == nSystemHash )
        {
            return eTypeSystem;
        }

        //ASSERT( 0 );
        return eTypeNull;
    }

    inline eDataType GetDataType( const char* szString )
    {
        sint nTypeHash = StringHash32( szString );
        return GetDataType( nTypeHash );
    }
}

#endif // #ifndef _OBJECT_H_
