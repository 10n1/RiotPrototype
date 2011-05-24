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
        static const uint32 nFloat3Hash = StringHash32( "float3" );
        static const uint32 nBoolHash = StringHash32( "bool" );
        static const uint32 nIntHash = StringHash32( "int" );
        static const uint32 nMeshHash = StringHash32( "mesh" );
        static const uint32 nMaterialHash = StringHash32( "material" );
        static const uint32 nTextureHash = StringHash32( "texture" );
        static const uint32 nFloatHash = StringHash32( "float" );

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
        sint32      nNumProperties = 0;
        DataType    nDataTypes[128] = { eTypeNull };
        
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
            m_nNameHash = StringHash32( (char*)pRight );
        }

        while( *pReadPos != 0 )
        {
            pReadPos = GetLine( pReadPos, pLine );
            SplitLine( pLine, pLeft, pRight );
            
            nDataTypes[ nNumProperties++ ] = GetDataType( (char*)pLeft );
        }

        // Done!

    }


} // namespace Riot
