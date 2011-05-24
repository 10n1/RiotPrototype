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

        inline void SetUpdateFunc( ObjectFunc* pFunc );
        inline void SetRenderFunc( ObjectFunc* pFunc );
        inline void Update( float fDt );
        inline void Render( float fDt );
        
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
        ObjectFunc* m_pUpdateFunc;
        ObjectFunc* m_pRenderFunc;
        sint32  m_nNumProperties;
        sint32  m_nNameHash;
    };
    inline void CObject::SetUpdateFunc( ObjectFunc* pFunc ) { m_pUpdateFunc = pFunc; }
    inline void CObject::SetRenderFunc( ObjectFunc* pFunc ) { m_pRenderFunc = pFunc; }
    inline void CObject::Update( float fDt ) { m_pUpdateFunc( this, fDt ); }
    inline void CObject::Render( float fDt ) { m_pRenderFunc( this, fDt ); }

    //////////////////////////////////////////
    inline void NullUpdateFunc( CObject* pObject, float dt ) { }
    inline void NullRenderFunc( CObject* pObject, float dt ) { }
}

#endif // #ifndef _OBJECT_H_
