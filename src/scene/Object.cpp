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
