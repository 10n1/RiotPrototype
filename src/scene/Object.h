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

    struct TObjectDefinition
    {
        DataType    nType[ 128 ];
        uint32      nTypeHash[ 128 ];
        uint32      nNameHash;
        uint32      nNumProperties;
    };

    class CObject
    {
    public:
        // CObject constructor
        CObject();

        // CObject destructor
        ~CObject();
        /***************************************\
        | class methods                         |
        \***************************************/

        static void CreateObjectTemplate( const char* szFilename );

        void CreateObjectFromFile( const char* szFilename );

        void CreateObjectOfType( const char* szName, const char* szType );

        void GetProperty( const char* szProp, void** pData );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static TObjectDefinition    m_pObjectTypes[ 128 ];
        static uint32               m_nNumObjectTypes;

        void*   m_pData;
        sint32  m_nNumProperties;
        sint32  m_nNameHash;
    };

    void IntegrateDynamics( RVector3* pos, RVector3* vel, RVector3* acc, float dt );
}

#endif // #ifndef _OBJECT_H_
