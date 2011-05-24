/*********************************************************\
File:           Object.cpp
Author:         Kyle Weicht
Created:        5/23/2011
Modified:       5/23/2011 8:35:11 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include <fstream>
#include "object.h"

namespace Riot
{

    TObjectDefinition    CObject::m_pObjectTypes[ 128 ] = { eTypeNull };
    uint32               CObject::m_nNumObjectTypes = 0;
    uint32               CObject::m_pFuncNameHashs[ 128 ] = { 0 };
    ObjectFunc*          CObject::m_pFuncs[ 128 ] = { NULL };
    uint32               CObject::m_nNumFuncs = 0;

    inline byte* GetLine( byte* pIn, byte* pOut )
    {
        while( *pIn != 10 && *pIn != 0 )
        {
            *pOut++ = *pIn++;
        }

        if( *pIn == 10 )
            pIn++;

        *pOut = 0;

        return pIn;
    }

    inline void SplitLine( byte* pIn, byte* pLeft, byte* pRight )
    {
        //////////////////////////////////////////
        // First fill the left
        while( *pIn != ' ' && *pIn != '\t' && *pIn != 0 )
        {
            *pLeft++ = *pIn++;
        }

        *pLeft = 0;

        // Skip the spaces
        while( ( *pIn == ' ' || *pIn == '\t' ) && *pIn != 0 )
        {
            pIn++;
        }

        // Now the right
        while( *pIn != 0 && *pIn != 10 && *pIn != ' ' )
        {
            *pRight++ = *pIn++;
        }
        
        *pRight = 0;
    }

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
        

        //ASSERT( 0 );
        return eTypeNull;
    }

    /***************************************\
    | class members                         |
    \***************************************/

    // CObject constructor
    CObject::CObject()
    {
    }

    // CObject destructor
    CObject::~CObject()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/
    void CObject::CreateObjectTemplate( const char* szFilename )
    {
        static const uint32 nNameHash = StringHash32( "name" );

        byte pData[1024] = { 0 };
        byte pLine[256] = { 0 };
        byte pLeft[128] = { 0 };
        byte pRight[128] = { 0 };

        byte* pReadPos = pData;

        FILE* f = fopen( szFilename, "rt" );
        
        fread( pData, sizeof( byte ), 1024, f );

        fclose( f );

        //////////////////////////////////////////
        // Now start the parsing

        TObjectDefinition& def = m_pObjectTypes[ m_nNumObjectTypes++ ];
        
        // Get the name
        pReadPos = GetLine( pReadPos, pLine );
        SplitLine( pLine, pLeft, pRight );
        if( StringHash32( (char*)pLeft ) != nNameHash )
        {
            ASSERT( false );
            return;
        }
        else
        {
            def.nNameHash = StringHash32( (char*)pRight );
        }

        // Get the rest of the properties
        while( *pReadPos != 0 )
        {
            pReadPos = GetLine( pReadPos, pLine );
            SplitLine( pLine, pLeft, pRight );

            def.nType[ def.nNumProperties ] = GetDataType( (char*)pLeft );
            def.nTypeHash[ def.nNumProperties ] = StringHash32( (char*)pRight );
            def.nNumProperties++;
        }

        // Done!

    }

    void CObject::CreateObjectFromFile( const char* szFilename )
    {
        static const uint32 nNameHash = StringHash32( "name" );
        static const uint32 nTypeHash = StringHash32( "type" );

        byte pData[1024] = { 0 };
        byte pLine[256] = { 0 };
        byte pLeft[128] = { 0 };
        byte pRight[128] = { 0 };

        byte* pReadPos = pData;

        FILE* f = fopen( szFilename, "rt" );        
        fread( pData, sizeof( byte ), 1024, f );
        fclose( f );

        //////////////////////////////////////////
        // Get the objects name
        pReadPos = GetLine( pReadPos, pLine );
        SplitLine( pLine, pLeft, pRight );
        
        if( StringHash32( (char*)pLeft ) != nNameHash )
        {
            ASSERT( false );
            return;
        }
        else
        {
            m_nNameHash = StringHash32( (char*)pRight );
        }

        //////////////////////////////////////////
        // Get the objects type
        sint32 nType = 0;

        pReadPos = GetLine( pReadPos, pLine );
        SplitLine( pLine, pLeft, pRight );
        
        if( StringHash32( (char*)pLeft ) != nTypeHash )
        {
            ASSERT( false );
            return;
        }
        else
        {
            nType = StringHash32( (char*)pRight );
        }

        //////////////////////////////////////////
        // Find the correct type
        uint i;
        for( i = 0; i < m_nNumObjectTypes; ++i )
        {
            if( m_pObjectTypes[i].nNameHash == nType )
                break;
        }

        if( i == m_nNumObjectTypes )
        {
            // We're trying to use an undefined type
            ASSERT( 0 );
        }
    }

    void CObject::CreateObjectOfType( const char* szName, const char* szType )
    {
        static const RVector3   defaultVector3 = RVector3Zero();
        static const sint32     defaultInt = 0;
        static const float      defaultFloat = 0.0f;
        static const bool       defaultBool = false;
        static const sint32     defaultMesh = 0;
        static const sint32     defaultTexture = 0;
        static const sint32     defaultMaterial = 0;
        static const ObjectFunc*    defaultFunc = NULL;

        sint32 nTypeHash = StringHash32( szType );

        m_nNameHash = StringHash32( szName );
        //////////////////////////////////////////
        // Find the correct type
        uint nType;
        for( nType = 0; nType < m_nNumObjectTypes; ++nType )
        {
            if( m_pObjectTypes[nType].nNameHash == nTypeHash )
                break;
        }
        if( nType == m_nNumObjectTypes )
        {
            // We're trying to use an undefined type
            ASSERT( 0 );
        }

        //////////////////////////////////////////
        // Count up the size
        sint32 nSize = 0;
        for( uint i = 0; i < m_pObjectTypes[ nType ].nNumProperties; ++i )
        {
            switch( m_pObjectTypes[nType].nType[i] )
            {
            case eTypeVector3:
                nSize += sizeof( DataVector3 );
                break;
            case eTypeInt:
                nSize += sizeof( DataInt );
                break;
            case eTypeFloat:
                nSize += sizeof( DataFloat );
                break;
            case eTypeBool:
                nSize += sizeof( DataBool );
                break;
            case eTypeMesh:
                nSize += sizeof( DataMesh );
                break;
            case eTypeMaterial:
                nSize += sizeof( DataMaterial );
                break;
            case eTypeTexture:
                nSize += sizeof( DataTexture );
                break;
            case eTypeFunc:
                nSize += sizeof( DataFunc );
                break;
            case eTypeNull:
                continue;
            default:
                ASSERT( 0 );
                break;
            }
        }

        m_pData = new byte[ nSize ];
        m_nNumProperties = m_pObjectTypes[ nType ].nNumProperties;

        //////////////////////////////////////////
        // Now fill the data with the default values
        byte* pData = (byte*)m_pData;
        for( uint i = 0; i < m_nNumProperties; ++i )
        {
            switch( m_pObjectTypes[nType].nType[i] )
            {
            case eTypeVector3:
                {
                    DataVector3* p = (DataVector3*)pData;
                    p->nType = eTypeVector3;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultVector3;
                    
                    sint nSize = sizeof( DataVector3 );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeInt:
                {
                    DataInt* p = (DataInt*)pData;
                    p->nType = eTypeInt;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultInt;
                    sint nSize = sizeof( DataInt );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeFloat:
                {
                    DataFloat* p = (DataFloat*)pData;
                    p->nType = eTypeFloat;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultFloat;
                    sint nSize = sizeof( DataFloat );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeBool:
                {
                    DataBool* p = (DataBool*)pData;
                    p->nType = eTypeBool;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultBool;
                    sint nSize = sizeof( DataBool );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeMesh:
                {
                    DataMesh* p = (DataMesh*)pData;
                    p->nType = eTypeMesh;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultMesh;
                    sint nSize = sizeof( DataMesh );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeMaterial:
                {
                    DataMaterial* p = (DataMaterial*)pData;
                    p->nType = eTypeMaterial;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultMaterial;
                    sint nSize = sizeof( DataMaterial );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeTexture:
                {
                    DataTexture* p = (DataTexture*)pData;
                    p->nType = eTypeTexture;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultTexture;
                    sint nSize = sizeof( DataTexture );

                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeFunc:
                {
                    DataFunc* p = (DataFunc*)pData;
                    p->nType = eTypeFunc;
                    p->nNameHash = m_pObjectTypes[nType].nTypeHash[i];
                    p->data = defaultFunc;

                    sint nSize = sizeof( DataFunc );

                    pData += nSize;
                    p->nOffset = nSize;
                }
                break;
            case eTypeNull:
                continue;
            default:
                ASSERT( 0 );
                break;
            }
        }
    }

    void CObject::GetProperty( const char* szProp, void** pData )
    {
        const uint32 nPropHash = StringHash32( szProp );

        void* pTemp = m_pData;
        for( uint i = 0; i < m_nNumProperties; ++i )
        {
            DataInt prop = *(DataInt*)pTemp;

            uint32 nNameHash = *(uint32*)pTemp;
            if( nNameHash == nPropHash )
            {
                *pData = (byte*)pTemp + 8;
                return;
            }

            byte nOffset = *((byte*)pTemp + 7);

            pTemp = (byte*)pTemp + nOffset;
        }

        ASSERT( 0 );
    }

    void IntegrateDynamics( CObject* pObject, float dt )
    {
        RVector3* pos0;
        RVector3* vel0;
        RVector3* acc0;

        pObject->GetProperty( "position", (void**)&pos0 );
        pObject->GetProperty( "velocity", (void**)&vel0 );
        pObject->GetProperty( "acceleration", (void**)&acc0 );

        RVector3 vel05;
        RVector3 pos1;
        RVector3 vel1;
        RVector3 acc1;

        //  1. Calcualte new position
        pos1    = *pos0 + *vel0*dt + (0.5f * *acc0 * Square(dt));

        //  2. Calculate 1/2 of the new velocity
        vel05   = *vel0 + 0.5f * *acc0 * dt;

        //  3. Calculate the new acceleration
        acc1    = *acc0; // force / m;

        //  4. Calculate the other half of the new velocity
        vel1    = vel05 + 0.5 * acc1 * dt;

        *pos0 = pos1;
        *vel0 = vel1;
        *acc0 = acc1;
    }

    void CObject::RegisterFunc( const char* szFunc, ObjectFunc* pFunc )
    {
        uint32 nFuncHash = StringHash32( szFunc );

        sint32 nIndex = m_nNumFuncs++;

        m_pFuncNameHashs[ nIndex ] = nFuncHash;
        m_pFuncs[ nIndex ] = pFunc;
    }

    ObjectFunc* CObject::GetFunction( const char* szFunc )
    {
        uint32 nHash = StringHash32( szFunc );

        return GetFunction( nHash );
    }

    ObjectFunc* CObject::GetFunction( uint32 nHash )
    {
        for( sint i = 0; i < m_nNumFuncs; ++i )
        {
            if( m_pFuncNameHashs[i] == nHash )
                return m_pFuncs[i];
        }

        ASSERT( 0 );
        return 0;
    }

} // namespace Riot
