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

    inline byte* GetLine( byte* pIn, byte* pOut )
    {
        while( *pIn != 10 && *pIn != 0 )
        {
            *pOut++ = *pIn++;
        }

        if( *pIn == 10 )
            pIn++;

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


} // namespace Riot
