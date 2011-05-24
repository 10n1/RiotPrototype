/*********************************************************\
File:           ObjectManager.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/3/2011 4:28:28 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include <fstream>
#include "ObjectManager.h"
#include "Engine.h"
#include "TaskManager.h"

namespace Riot
{
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

    /*****************************************************************************\
    \*****************************************************************************/
    #define LOAD_COMPONENT( TheComponent )                                          \
        TheComponent::m_pInstance = new TheComponent;                               \
        m_pComponents[ TheComponent::ComponentType ] = TheComponent::m_pInstance;   \
        TheComponent::m_pInstance->m_pObjectIndices = m_pObjectIndices[ TheComponent::ComponentType ]; \
        TheComponent::m_pInstance->m_pComponentIndices = m_pComponentIndices[ TheComponent::ComponentType ]; \
        for( uint i = 0; i < TheComponent::NumMessagesReceived; ++i )               \
        {                                                                           \
            m_bRegistered[ TheComponent::MessagesReceived[i] ][ TheComponent::ComponentType ] = true; \
        } 
    /*****************************************************************************\
    \*****************************************************************************/

    /***************************************\
    | class members                         |
    \***************************************/
    const MessageType    CObjectManager::MessagesReceived[] =
    {
        mNULLMESSAGE,
    };
    const uint           CObjectManager::NumMessagesReceived = 0; //ARRAY_LENGTH(MessagesReceived);


    // CObjectManager constructor
    CObjectManager::CObjectManager()
    {
    }

    // CObjectManager destructor
    CObjectManager::~CObjectManager()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/
    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CObjectManager::Initialize( void )
    {
        Memset( m_Objects, 0, sizeof( m_Objects ) );
        Memset( m_pObjectTypes, 0, sizeof( m_pObjectTypes ) );
        Memset( m_pFuncs, 0, sizeof( m_pFuncs ) );
        Memset( m_pFuncNameHashs, 0, sizeof( m_pFuncNameHashs ) );
        m_nNumObjectTypes = 0;
        m_nNumFuncs = 0;

        m_nNumObjects = 0;

        sint nIndex = 0;
        for( sint i = MAX_OBJECTS-1; i >= 0; --i )
        {
            m_nFreeSlots[ nIndex++ ] = i;
        }
        m_nNumFreeSlots = MAX_OBJECTS;

        Memset( m_nActiveObjects, 0, sizeof( m_nActiveObjects ) );
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CObjectManager::Shutdown( void )
    {
        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            SAFE_DELETE( m_Objects[ m_nActiveObjects[i] ].m_pData );
        }
    }

    //-----------------------------------------------------------------------------
    //  CreateObject
    //  Creates a new object and returns its index
    //-----------------------------------------------------------------------------
    uint CObjectManager::CreateObject( const char* szName, const char* szType )
    {
        if( !szType )
        {
            ASSERT( 0 );
        }

        uint32 nTypeHash = StringHash32( szType );

        uint32 nNameHash;
        if( szName )
        {
            nNameHash = StringHash32( szName );
        }
        else
        {
            // Create a "random" hash
            nNameHash = nTypeHash ^ m_nNumObjects;
        }


        return CreateObject( nNameHash, nTypeHash );
    }
    uint CObjectManager::CreateObject( uint32 nNameHash, uint32 nTypeHash )
    {        
        static const RVector3       defaultVector3 = RVector3Zero();
        static const sint32         defaultInt = 0;
        static const float          defaultFloat = 0.0f;
        static const bool           defaultBool = false;
        static const sint32         defaultMesh = 0;
        static const sint32         defaultTexture = 0;
        static const sint32         defaultMaterial = 0;
        static const ObjectFunc*    defaultFunc = NULL;

        sint nObjectIndex = AtomicIncrement( &m_nNumObjects ) - 1;
        sint nFreeIndex = m_nFreeSlots[ AtomicDecrement( &m_nNumFreeSlots ) ];

        m_nActiveObjects[ nObjectIndex ] = nFreeIndex;

        nObjectIndex = nFreeIndex;

        CObject& o = m_Objects[ nObjectIndex ];

        o.m_nNameHash = nNameHash;
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

        o.m_pData = new byte[ nSize ];
        o.m_nNumProperties = m_pObjectTypes[ nType ].nNumProperties;

        //////////////////////////////////////////
        // Now fill the data with the default values
        byte* pData = (byte*)o.m_pData;
        for( uint i = 0; i < o.m_nNumProperties; ++i )
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

        m_Objects[ nObjectIndex ].SetRenderFunc( NullRenderFunc );
        m_Objects[ nObjectIndex ].SetUpdateFunc( NullUpdateFunc );

        return nObjectIndex;
    }
    uint CObjectManager::CreateObjectFromFile( const char* szFilename )
    {
        ASSERT( 0 );
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
            return -1;
        }

        uint32 nName = StringHash32( (char*)pRight );

        //////////////////////////////////////////
        // Get the objects type
        pReadPos = GetLine( pReadPos, pLine );
        SplitLine( pLine, pLeft, pRight );
        
        if( StringHash32( (char*)pLeft ) != nTypeHash )
        {
            ASSERT( false );
            return -1;
        }

        sint32 nType = StringHash32( (char*)pRight );

        //////////////////////////////////////////
        // Create the default object
        uint nObject = CreateObject( nName, nType );
        CObject& o = m_Objects[ nObject ];
        
        // Get the rest of the properties
        while( *pReadPos != 0 )
        {
            pReadPos = GetLine( pReadPos, pLine );
            SplitLine( pLine, pLeft, pRight );

            void* p = 0;
            o.GetProperty( (char*)pLeft, &p );
        }


        return nObject;
    }

    //-----------------------------------------------------------------------------
    //  DeleteObject
    //  Deletes an object
    //-----------------------------------------------------------------------------
    void CObjectManager::DeleteObject( uint nIndex )
    {
        SAFE_DELETE( m_Objects[ nIndex ].m_pData );

        uint nActive = AtomicDecrement( &m_nNumObjects );

        for( int i = 0; i <= nActive; ++i )
        {
            if( m_nActiveObjects[i] == nIndex )
            {
                m_nActiveObjects[i] = m_nActiveObjects[ nActive ];
            }
        }

        m_nFreeSlots[ AtomicIncrement( &m_nNumFreeSlots ) - 1 ] = nIndex;
    }

    //-----------------------------------------------------------------------------
    //  LoadObjectDeclaration
    //  Loads an object declaration from a file
    //-----------------------------------------------------------------------------
    void CObjectManager::LoadObjectDeclaration( const char* szFilename )
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

    void CObjectManager::PipelineObjectUpdate( void* pData, uint nThreadId, uint nStart, uint nCount )
    {
        CObjectManager* pManager = (CObjectManager*)pData;
        float fDt = Engine::m_fElapsedTime;

        pManager->UpdateObjects( fDt );
    }

    //-----------------------------------------------------------------------------
    //  RegisterFunc
    //  Registers an object function
    //-----------------------------------------------------------------------------
    void CObjectManager::RegisterFunc( const char* szFunc, ObjectFunc* pFunc )
    {
        uint32 nFuncHash = StringHash32( szFunc );

        sint32 nIndex = m_nNumFuncs++;

        m_pFuncNameHashs[ nIndex ] = nFuncHash;
        m_pFuncs[ nIndex ] = pFunc;
    }

    //-----------------------------------------------------------------------------
    //  GetFunction
    //  Returns the function
    //-----------------------------------------------------------------------------
    ObjectFunc* CObjectManager::GetFunction( const char* szFunc )
    {
        uint32 nHash = StringHash32( szFunc );

        return GetFunction( nHash );
    }
    ObjectFunc* CObjectManager::GetFunction( uint32 nHash )
    {
        for( sint i = 0; i < m_nNumFuncs; ++i )
        {
            if( m_pFuncNameHashs[i] == nHash )
                return m_pFuncs[i];
        }

        ASSERT( 0 );
        return 0;
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CObjectManager::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        default:
            {
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  UpdateObjects
    //  Updates the objects
    //-----------------------------------------------------------------------------
    void CObjectManager::UpdateObjects( float fDt )
    {
        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            m_Objects[ m_nActiveObjects[i] ].Update( fDt );
        }
        
        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            m_Objects[ m_nActiveObjects[i] ].Render( fDt );
        }
    }

} // namespace Riot
