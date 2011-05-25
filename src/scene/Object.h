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

    enum DataType
    {
        eTypeNull = 0,
        eTypeVector3,
        eTypeInt,
        eTypeFloat,
        eTypeBool,
        eTypeMesh,
        eTypeMaterial,
        eTypeTexture,
        eTypeFunc,
        eTypeTransform,
    };

    template<class T>
    struct Data
    {
        uint32      nNameHash;
        DataType    nType:24;
        uint32      nOffset:8;
        T           data;
    };

    typedef Data<RVector3>  DataVector3;
    typedef Data<sint32>    DataInt;
    typedef Data<float>     DataFloat;
    typedef Data<bool>      DataBool;
    typedef Data<sint32>    DataMesh;
    typedef Data<sint32>    DataTexture;
    typedef Data<sint32>    DataMaterial;
    typedef Data<RTransform>    DataTransform;

    typedef void (ObjectFunc)(CObject*,float);
    typedef Data<ObjectFunc*>    DataFunc;

    struct TObjectDefinition
    {
        DataType    nType[ 128 ];
        uint32      nTypeHash[ 128 ];
        uint32      nNameHash;
        uint32      nNumProperties;
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
        
    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static TObjectDefinition    m_pObjectTypes[ 128 ];
        static uint32               m_nNumObjectTypes;
        static uint32               m_pFuncNameHashs[ 128 ];
        static ObjectFunc*          m_pFuncs[ 128 ];
        static uint32               m_nNumFuncs;

        void*   m_pData;
        sint32  m_nNumProperties;
        sint32  m_nNameHash;
    };
    
    inline DataType GetDataType( const char* szString )
    {
        const uint32 nFloat3Hash = StringHash32( "float3" );
        const uint32 nBoolHash = StringHash32( "bool" );
        const uint32 nIntHash = StringHash32( "int" );
        const uint32 nMeshHash = StringHash32( "mesh" );
        const uint32 nMaterialHash = StringHash32( "material" );
        const uint32 nTextureHash = StringHash32( "texture" );
        const uint32 nFloatHash = StringHash32( "float" );
        const uint32 nFuncHash = StringHash32( "func" );
        const uint32 nTransformHash = StringHash32( "transform" );

        sint nTypeHash = StringHash32( szString );

        if( nTypeHash ==  nFloat3Hash )
        {
            return eTypeVector3;                
        }
        if( nTypeHash ==  nBoolHash )
        {
            return eTypeBool;                
        }
        if( nTypeHash ==  nIntHash )
        {
            return eTypeInt;                
        }
        if( nTypeHash ==  nMeshHash )
        {
            return eTypeMesh;                
        }
        if( nTypeHash ==  nMaterialHash )
        {
            return eTypeMaterial;                
        }
        if( nTypeHash ==  nTextureHash )
        {
            return eTypeTexture;                
        }
        if( nTypeHash ==  nFloatHash )
        {
            return eTypeFloat;                
        }
        if( nTypeHash == nFuncHash )
        {
            return eTypeFunc;
        }
        if( nTypeHash == nTransformHash )
        {
            return eTypeTransform;
        }

        //ASSERT( 0 );
        return eTypeNull;
    }
}

#endif // #ifndef _OBJECT_H_
