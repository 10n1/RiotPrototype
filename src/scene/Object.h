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
        DataType    nType;
        uint32      nNameHash;
        T           data;
    };

    typedef Data<RVector3>  DataVector3;
    typedef Data<sint32>    DataInt;
    typedef Data<float>     DataFloat;
    typedef Data<bool>      DataBool;
    typedef Data<sint32>    DataMesh;
    typedef Data<sint32>    DataTexture;
    typedef Data<sint32>    DataMaterial;

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

        void CreateObjectTemplate( const char* szFilename );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        void*   m_pData;
        sint32  m_nNumProperties;
        sint32  m_nNameHash;
    };


}

#endif // #ifndef _OBJECT_H_
