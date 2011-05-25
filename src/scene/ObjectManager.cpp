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
#include "RenderSystem.h"
#include "CollisionSystem.h"
#include "PhysicsSystem.h"

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
        m_nNumObjectTypes = 0;

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
        static const RVector3       defaultFloat3 = RVector3Zero();
        static const sint32         defaultInt = 0;
        static const float          defaultFloat = 0.0f;
        static const bool           defaultBool = false;
        static const RQuaternion    defaultQuaternion = RQuaternionZero();

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

        TObjectDefinition& def = m_pObjectTypes[ nType ];

        //////////////////////////////////////////
        // Count up the size
        sint32 nSize = 0;
        for( uint i = 0; i < def.nNumProperties; ++i )
        {
            nSize += GetSizeOfType( def.nType[i] );
        }

        o.m_pData = new byte[ nSize ];
        o.m_nNumProperties = def.nNumProperties;

        //////////////////////////////////////////
        // Now fill the data with the default values
        byte* pData = (byte*)o.m_pData;
        for( uint i = 0; i < o.m_nNumProperties; ++i )
        {
            eDataType dt = GetDataType( def.nType[i] );
            switch( dt )
            {
            case eTypeFloat3:
                {
                    DataFloat3* p = (DataFloat3*)pData;
                    p->nType = eTypeFloat3;
                    p->nNameHash = def.nTypeHash[i];
                    p->data = defaultFloat3;
                    
                    sint nSize = sizeof( DataFloat3 );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeInt:
                {
                    DataInt* p = (DataInt*)pData;
                    p->nType = eTypeInt;
                    p->nNameHash = def.nTypeHash[i];
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
                    p->nNameHash = def.nTypeHash[i];
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
                    p->nNameHash = def.nTypeHash[i];
                    p->data = defaultBool;
                    sint nSize = sizeof( DataBool );
                    pData += nSize;
                    p->nOffset = nSize;
                    break;
                }
            case eTypeQuaternion:
                {
                    DataQuaternion* p = (DataQuaternion*)pData;
                    p->nType = eTypeQuaternion;
                    p->nNameHash = def.nTypeHash[i];
                    p->data = defaultQuaternion;

                    sint nSize = sizeof( DataQuaternion );
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

        if( def.nSystems & eSystemPhysics ) CPhysicsSystem::AddObject( &o );
        if( def.nSystems & eSystemRender ) CRenderSystem::AddObject( &o );
        if( def.nSystems & eSystemCollision ) CCollisionSystem::AddObject( &o );

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
        def.nNameHash = StringHash32( (char*)pRight );

        // Get the rest of the properties
        while( *pReadPos != 0 )
        {
            pReadPos = GetLine( pReadPos, pLine );
            SplitLine( pLine, pLeft, pRight );

            //def.nType[ def.nNumProperties ] = StringHash32( (char*)pLeft );
            //def.nTypeHash[ def.nNumProperties ] = StringHash32( (char*)pRight );
            //def.nNumProperties++;

            uint32 nTypeHash = StringHash32( (char*)pLeft );
            uint32 nNameHash = StringHash32( (char*)pRight );
            AddPropertyToDefinition( def, nTypeHash, nNameHash );
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
    }

    void CObjectManager::AddPropertyToDefinition( TObjectDefinition& def, uint32 nTypeHash, uint32 nNameHash )
    {
        eDataType nType = GetDataType( nTypeHash );
        if( nType == eTypeSystem )
        {
            static const uint32 nPhysicsHash = StringHash32( "physics" );
            static const uint32 nRenderHash = StringHash32( "render" );
            static const uint32 nCollisionHash = StringHash32( "collision" );

            if( nNameHash == nPhysicsHash )
            {
                def.nSystems |= eSystemPhysics;
                def.nSystems |= eSystemCollision;
            }
            else if( nNameHash == nRenderHash ) 
            {
                def.nSystems |= eSystemRender;
                def.nSystems |= eSystemCollision;
            }
            else if( nNameHash == nCollisionHash ) 
            {
                def.nSystems |= eSystemCollision;
            }
        }
        else if( nType != eTypeNull )
        {   
            bool bFound = false;
            for( uint i = 0; i < def.nNumProperties; ++i )
            {
                if( def.nTypeHash[i] == nNameHash )
                {
                    return;
                }
            }

            def.nType[ def.nNumProperties ] = nTypeHash;
            def.nTypeHash[ def.nNumProperties ] = nNameHash;
            def.nNumProperties++;
        }
        else
        {
            for( uint i = 0; i < m_nNumObjectTypes; ++i )
            {
                if( m_pObjectTypes[i].nNameHash == nTypeHash )
                {
                    for( uint j = 0; j < m_pObjectTypes[i].nNumProperties; ++j )
                    {
                        AddPropertyToDefinition( def, m_pObjectTypes[i].nType[j], m_pObjectTypes[i].nTypeHash[j] );
                    }
                    def.nSystems |= m_pObjectTypes[i].nSystems;
                    return;
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  GetSizeOfType
    //  Returns the size of the type
    //-----------------------------------------------------------------------------
    uint CObjectManager::GetSizeOfType( uint32 nTypeHash )
    {
        switch( GetDataType( nTypeHash ) )
        {
        case eTypeFloat3:
            return sizeof( DataFloat3 );
        case eTypeInt:
            return sizeof( DataInt );
        case eTypeFloat:
            return sizeof( DataFloat );
        case eTypeBool:
            return sizeof( DataBool );
        case eTypeQuaternion:
            return sizeof( DataQuaternion );
        case eTypeSystem:
            return 0;
        case eTypeNull:
            return 0;
        default:
            ASSERT( 0 );
            break;
        }

        return 0;
    }

} // namespace Riot
