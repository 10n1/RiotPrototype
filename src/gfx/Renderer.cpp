/*********************************************************\
File:           Renderer.cpp
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       5/21/2011 4:40:50 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include <fstream>
#include "config.h"

#if USE_OPENGL
#include "OGLGraphics.h"
#else
#include "D3DGraphics.h"
#endif

#include "Renderer.h"
#include "System.h"
#include "Mesh.h"
#include "VertexFormats.h"
#include "View.h"
#include "TaskManager.h"
#include "UI.h"
#include "Terrain.h"


namespace Riot
{
    struct BaseKey
    {
        uint32  nValue;
        float   fDepth;
    };
    union Key
    {
        uint64  nKey;
        BaseKey nBase;
    };

    // TRenderCommand constructor
    TRenderCommand::TRenderCommand()
    {
        Clear();
    }

    // TRenderCommand destructor
    TRenderCommand::~TRenderCommand()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/
    void TRenderCommand::Clear( void )
    {
        m_nMaterial = eMatStandard;
        m_nSampler = eSamplerLinear;
        m_nTexture = 0;
        m_nTransparant = 0;
        m_nMesh = 0;

        m_fDepth = 0.0f;
    }
    uint64 TRenderCommand::Encode( void )
    {
        Key k;
        BaseKey base;

        // Encode the handles
        base.nValue =     ( m_nMesh << 11 )         // 8 bits for Mesh
                        | ( m_nMaterial << 7 )      // 4 bits for material
                        | ( m_nSampler << 5 )       // 2 bits for sampler
                        | ( m_nTexture << 1 )       // 4 bits for texture
                        | ( m_nTransparant << 0 );  // 1 bit for transparency

        // Then add the depth
        base.fDepth = m_fDepth;

        // Now return the key
        k.nBase = base;
        return k.nKey;
    }
    void TRenderCommand::Decode( uint64 nKey )
    {
        Key k;
        BaseKey base;
        k.nKey = nKey;
        base = k.nBase;

        // Get the depth
        m_fDepth = k.nBase.fDepth;

        // Now get the handles
        m_nTransparant  = DecodeTransparant( nKey );
        m_nTexture      = DecodeTexture( nKey );
        m_nSampler      = DecodeSampler( nKey );
        m_nMaterial     = DecodeMaterial( nKey );
        m_nMesh         = DecodeMesh( nKey );
    }

    /***************************************\
    | class members                         |
    \***************************************/
    const MessageType    CRenderer::MessagesReceived[] = 
    {
        mFullscreen,
        mResize
    };

    const uint           CRenderer::NumMessagesReceived = ARRAY_LENGTH( MessagesReceived );

    // CRenderer constructor
    CRenderer::CRenderer()
    {
    }

    // CRenderer destructor
    CRenderer::~CRenderer()
    {
    }

    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Initialize
    //-----------------------------------------------------------------------------
    void CRenderer::Initialize( void )
    {        
        m_pLineBuffer   = NULL;

        m_pDevice       = NULL;
        m_pViewProjCB   = NULL;
        m_pWorldCB      = NULL;

        m_nDefaultMesh      = INVALID_HANDLE;
        m_nDefaultTexture   = INVALID_HANDLE;

        m_pCurrentView  = NULL;

        m_nNumCommands  = 0;
        Memset( m_pRenderCommands, 0, sizeof( m_pRenderCommands ) );

        m_nNumBoxes = 0;

        m_nSphereMesh = INVALID_HANDLE;
        m_nDebugBox = INVALID_HANDLE;

        m_pPrevCommands = m_pRenderCommands[0];
        m_pCurrCommands = m_pRenderCommands[1];

        m_pPrevTransforms = m_pTransforms[0];
        m_pCurrTransforms = m_pTransforms[1];

        m_pPrevDebugBoxes = m_DebugBoxes[0];     
        m_pCurrDebugBoxes = m_DebugBoxes[1];

        m_pPrevDebugRays = m_DebugRays[0];
        m_pCurrDebugRays = m_DebugRays[1];

        m_pPrevLights    = &m_pLights[0];
        m_pCurrLights    = &m_pLights[1];

        Memset( m_ppVertexShaders, 0, sizeof( m_ppVertexShaders ) );
        Memset( m_ppVertexLayouts, 0, sizeof( m_ppVertexLayouts ) );
        Memset( m_ppPixelShaders, 0, sizeof( m_ppPixelShaders ) );
        Memset( m_ppSamplerStates, 0, sizeof( m_ppSamplerStates ) );

        Memset( m_ppMeshes, 0, sizeof( m_ppMeshes ) );
        m_nNumMeshes = 0;

        Memset( m_ppTextures, 0, sizeof( m_ppTextures ) );
        m_nNumTextures = 0;

        Memset( m_pLights, 0, sizeof( m_pLights ) );

        m_nWidth = m_nHeight = -1;

        m_pTestRT = NULL;
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CRenderer::Shutdown( void )
    {
        SAFE_RELEASE( m_pTestRT );

        for( sint i = 0; i < m_nNumMeshes; ++i )
        {
            SAFE_RELEASE( m_ppMeshes[i] );
        }

        for( uint i = 0; i < NUM_VERTEX_SHADERS; ++i )
        {
            SAFE_RELEASE( m_ppVertexShaders[i] );
            SAFE_RELEASE( m_ppVertexLayouts[i] );
        }

        for( uint i = 0; i < NUM_SAMPLER_STATES; ++i )
        {
            SAFE_RELEASE( m_ppSamplerStates[i] );
        }

        for( uint i = 0; i < NUM_PIXEL_SHADERS; ++i )
        {
            SAFE_RELEASE( m_ppPixelShaders[i] );
        }

        for( uint i = 0; i < m_nNumTextures; ++i )
        {
            SAFE_RELEASE( m_ppTextures[i] );
        }

        SAFE_RELEASE( m_pLineBuffer );

        SAFE_RELEASE( m_pWorldCB );
        SAFE_RELEASE( m_pViewProjCB );
        SAFE_RELEASE( m_pLightCB );
        SAFE_RELEASE( m_pDevice );

        UI::Destroy();
    }

    //-----------------------------------------------------------------------------
    //  CreateGraphicsDevice
    //  Creates the device
    //-----------------------------------------------------------------------------
    void CRenderer::CreateGraphicsDevice( CWindow* pWindow )
    {
        Result nResult = rResultSuccess;

        // Make the new device
#if USE_OPENGL
        m_pDevice = new COGLDevice;
#else
        m_pDevice = new CD3DDevice;
#endif

        // Then initialize it
        m_pDevice->Initialize( pWindow );
        ASSERT( nResult == rResultSuccess );

        // Then set some basic parameters
        m_pDevice->SetClearColor( 0.25f, 0.25f, 0.75f, 1.0f );
        m_pDevice->SetClearDepth( 1.0f );

        // Create the constant buffers
        m_pViewProjCB = m_pDevice->CreateConstantBuffer( sizeof( RMatrix4 ), NULL );
        m_pWorldCB = m_pDevice->CreateConstantBuffer( sizeof( RMatrix4 ), NULL );
        m_pLightCB = m_pDevice->CreateConstantBuffer( sizeof( TLights ), NULL );
        
        // Load the defaults
        CreateDefaultObjects();

        // Init UI
        UI::Initialize( m_pDevice );

    }

    //-----------------------------------------------------------------------------
    //  CreateDefaultObjects
    //  Creates the default objects
    //-----------------------------------------------------------------------------
    void CRenderer::CreateDefaultObjects( void )
    {
        // Texture
        m_nDefaultTexture = LoadTexture2D( "Assets/Textures/DefaultTexture.png" );
        
        // Default mesh
        m_nDefaultMesh = CreateMesh();

        // debug sphere
        m_nSphereMesh = LoadMesh( "Assets/meshes/sphere.mesh" );

        // debug box
        m_nDebugBox = CreateDynamicBox();
        
        //////////////////////////////////////////
        //  Load Shaders
        LoadShaders();

        // a vertex shader for drawing lines        
        VPosColor pLineVertices[] =
        {
            { RVector3(  0.0f,  0.0f,  0.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
        };
        m_pLineBuffer = m_pDevice->CreateVertexBuffer( sizeof( pLineVertices ), pLineVertices );

        // Set the defaults
        SetVertexShader( eVS3DPosNorTexStd );
        SetPixelShader( ePS3DStd );
        SetSamplerState( eSamplerLinear );
        m_pDevice->SetPrimitiveType( GFX_PRIMITIVE_TRIANGLELIST );
    }

    //-----------------------------------------------------------------------------
    //  LoadShaders
    //  Loads all the shaders
    //-----------------------------------------------------------------------------
    void CRenderer::LoadShaders( void )
    {
        //////////////////////////////////////////
        // Load the pixel shaders
        m_ppPixelShaders[ ePS3DStd ] = m_pDevice->CreatePixelShader( "Assets/Shaders/BasicPixelShader.hlsl", "main" );
        m_ppPixelShaders[ ePS3DColor ] = m_pDevice->CreatePixelShader( "Assets/Shaders/PosColorPixelShader.hlsl", "main" );
        m_ppPixelShaders[ ePS2DTex ] = m_pDevice->CreatePixelShader( "Assets/Shaders/UI.hlsl", "PS" );

        //////////////////////////////////////////
        // Load the vertex shaders/layouts
        m_pDevice->CreateVertexShaderAndLayout( "Assets/Shaders/BasicVertexShader.hlsl", 
            "main", 
            VPosNormalTex::Layout, 
            VPosNormalTex::LayoutSize,
            &m_ppVertexShaders[ eVS3DPosNorTexStd ],
            &m_ppVertexLayouts[ eVS3DPosNorTexStd ] );
        m_pDevice->CreateVertexShaderAndLayout( "Assets/Shaders/BasicVertexShaderNoTransform.hlsl", 
            "main", 
            VPosNormalTex::Layout, 
            VPosNormalTex::LayoutSize,
            &m_ppVertexShaders[ eVS3DPosNorTexNoTransform ],
            NULL );
        m_ppVertexLayouts[ eVS3DPosNorTexNoTransform ] = m_ppVertexLayouts[ eVS3DPosNorTexStd ];
        
        m_pDevice->CreateVertexShaderAndLayout( "Assets/Shaders/BasicVertexShaderInstanced.hlsl", 
            "main", 
            VPosNormalTexInst::Layout, 
            VPosNormalTexInst::LayoutSize,
            &m_ppVertexShaders[ eVS3DPosNorTexInst ],
            &m_ppVertexLayouts[ eVS3DPosNorTexInst ] );
        
        m_pDevice->CreateVertexShaderAndLayout( "Assets/Shaders/PosColorVertexShader.hlsl", 
            "main", 
            VPosColor::Layout, 
            VPosColor::LayoutSize,
            &m_ppVertexShaders[ eVS3DPosColStd ],
            &m_ppVertexLayouts[ eVS3DPosColStd ] );

        //////////////////////////////////////////
        // Load the sampler states
        m_ppSamplerStates[ eSamplerLinear ] = m_pDevice->CreateSamplerState( GFX_TEXTURE_SAMPLE_LINEAR );
        m_ppSamplerStates[ eSamplerNearest ] = m_pDevice->CreateSamplerState( GFX_TEXTURE_SAMPLE_NEAREST );
    }

    //-----------------------------------------------------------------------------
    //  Render
    //  Performs rendering
    //-----------------------------------------------------------------------------
    void CRenderer::Render( CTerrain* pTerrain )
    {
        //////////////////////////////////////////
        // Just render the text
        if( !gnRenderOn )
        {   // Don't render if we shouldn't
            m_nPrevNumCommands  = 0;
            m_nPrevNumBoxes     = 0;

            m_pDevice->Clear();
            UI::Draw( m_pDevice );
            m_pDevice->Present();
            return;
        }

        //////////////////////////////////////////
        // Sort the list
        Sort();

        //////////////////////////////////////////
        // Restore solid fill mode
        if( gnRenderWireframe )
            m_pDevice->SetFillMode( GFX_FILL_WIREFRAME );
        else
            m_pDevice->SetFillMode( GFX_FILL_SOLID );

        //////////////////////////////////////////
        // Update lighting
        m_pDevice->UpdateBuffer( m_pLightCB, m_pPrevLights );
        m_pDevice->SetPSConstantBuffer( 0, m_pLightCB );

        //////////////////////////////////////////
        // Clear
        m_pDevice->Clear();

        if( m_pTestRT )
        {
            m_pDevice->ClearRenderTarget( m_pTestRT );
            m_pDevice->SetRenderTarget( m_pTestRT );
        }

        //////////////////////////////////////////
        // Render
        ASSERT( m_pCurrentView );

        SetViewProj( m_pCurrentView->GetViewMatrix(), m_pCurrentView->GetProjMatrix() );

        //////////////////////////////////////////
        // Render the terrain
        SetVertexShader( eVS3DPosNorTexNoTransform );
        SetPixelShader( ePS3DStd );
        pTerrain->Render();
        SetVertexShader( eVS3DPosNorTexStd );

        //////////////////////////////////////////
        // Perform basic object rendering
        for( sint i = 0; i < m_nPrevNumCommands/2; ++i )
        {
            ProcessCommand( m_pPrevCommands[i], m_pPrevTransforms[i] );
        }

        m_pDevice->SetDefaultRenderDepthTarget();
        
        for( sint i = m_nPrevNumCommands/2; i < m_nPrevNumCommands; ++i )
        {
            ProcessCommand( m_pPrevCommands[i], m_pPrevTransforms[i] );
        }

        //////////////////////////////////////////
        // Draw the debug volumes
        if( gnShowBoundingVolumes )
        {
            m_pDevice->SetFillMode( GFX_FILL_WIREFRAME );
            SetVertexShader( eVS3DPosColStd );
            SetPixelShader( ePS3DColor );

            // Draw the debug boxes
            for( sint i = 0; i < m_nPrevNumBoxes; ++i )
            {
                RVector3 vMin = m_pPrevDebugBoxes[i].box.min;
                RVector3 vMax = m_pPrevDebugBoxes[i].box.max;
                RVector3 vColor = m_pPrevDebugBoxes[i].color;

                RMatrix4 mWorld = RMatrix4Identity();
                SetWorldMatrix( mWorld );

                VPosColor vertices[] =
                {
                    { RVector3(  vMin.x,  vMax.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMax.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMax.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMax.y,  vMax.z ), Homogonize( vColor ) },

                    { RVector3(  vMin.x,  vMin.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMin.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMin.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMin.y,  vMax.z ), Homogonize( vColor ) },

                    { RVector3(  vMin.x,  vMin.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMin.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMax.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMax.y,  vMax.z ), Homogonize( vColor ) },

                    { RVector3(  vMax.x,  vMin.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMin.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMax.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMax.y,  vMax.z ), Homogonize( vColor ) },

                    { RVector3(  vMin.x,  vMin.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMin.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMax.y,  vMin.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMax.y,  vMin.z ), Homogonize( vColor ) },

                    { RVector3(  vMin.x,  vMin.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMin.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMax.x,  vMax.y,  vMax.z ), Homogonize( vColor ) },
                    { RVector3(  vMin.x,  vMax.y,  vMax.z ), Homogonize( vColor ) },
                };

                m_pDevice->UpdateBuffer( m_ppMeshes[m_nDebugBox]->m_pVertexBuffer, vertices );

                m_ppMeshes[m_nDebugBox]->DrawMesh();
            }

            // Draw the rays
            m_pDevice->SetPrimitiveType( GFX_PRIMITIVE_LINELIST );
            for( sint i = 0; i < m_nPrevNumRays; ++i )
            {
                RVector3 vStart = m_pPrevDebugRays[i].start;
                RVector3 vEnd = m_pPrevDebugRays[i].end;

                RVector4 vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
                RVector4 vBlack( 0.0f, 0.0f, 0.0f, 0.0f );
                VPosColor vertices[] =
                {
                    { vStart, vBlack },
                    { vEnd, vWhite },
                };

                m_pDevice->UpdateBuffer( m_pLineBuffer, vertices );

                m_pDevice->SetVertexBuffer( 0, m_pLineBuffer, VPosColor::VertexStride );
                m_pDevice->DrawPrimitive( 2 );
            }

            m_pDevice->SetFillMode( GFX_FILL_SOLID );
            m_pDevice->SetPrimitiveType( GFX_PRIMITIVE_TRIANGLELIST );
            SetVertexShader( eVS3DPosNorTexStd );
            SetPixelShader( ePS3DStd );
        }

        //////////////////////////////////////////
        // Draw the UI
        UI::Draw( m_pDevice );

        // Present
        m_pDevice->Present();
    }

    //-----------------------------------------------------------------------------
    //  UpdateMesh
    //  Updates a meshes data
    //-----------------------------------------------------------------------------
    void CRenderer::UpdateBuffer( IGfxBuffer* pBuffer, void* pData )
    {
        m_pDevice->UpdateBuffer( pBuffer, pData );
    }

    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates a mesh from the file
    //-----------------------------------------------------------------------------
    sint CRenderer::CreateMesh(   uint nVertexStride, 
                                    uint nVertexCount, 
                                    uint nIndexSize, 
                                    uint nIndexCount, 
                                    void* pVertices, 
                                    void* pIndices,
                                    GFX_BUFFER_USAGE nUsage )
    {
        ASSERT( m_nNumMeshes < MAX_MESHES );

        CMesh* pMesh = new CMesh;

        //////////////////////////////////////////
        // Create the vertex buffer
        pMesh->m_pVertexBuffer = m_pDevice->CreateVertexBuffer( nVertexStride * nVertexCount, pVertices, nUsage );
        
        //////////////////////////////////////////
        // Create the index buffer
        uint nSize = nIndexCount * nIndexSize;
        pMesh->m_pIndexBuffer = m_pDevice->CreateIndexBuffer( nSize, pIndices );

        pMesh->m_nIndexCount    = nIndexCount;
        pMesh->m_nIndexSize     = nIndexSize;
        pMesh->m_nVertexStride  = nVertexStride;

        pMesh->m_pDevice    = m_pDevice; // Allow the mesh access to the device

        //
        sint nIndex = AtomicIncrement( &m_nNumMeshes ) - 1;
        m_ppMeshes[ nIndex ] = pMesh;

        return nIndex;
    }
    sint CRenderer::CreateMesh( void )
    {
        // If the cube has already been created, return it
        if( m_nDefaultMesh != INVALID_HANDLE )
        {
            return m_nDefaultMesh;
        }

        //////////////////////////////////////////
        // Define the index buffer
        uint16 indices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        m_nDefaultMesh = CreateMesh( VPosNormalTex::VertexStride, 24, sizeof(uint16), ARRAY_LENGTH( indices ), GetDefaultMeshData(), indices, GFX_BUFFER_USAGE_IMMUTABLE );

        return m_nDefaultMesh;
    }
    sint CRenderer::CreateDynamicBox( void )
    {
        //////////////////////////////////////////
        // Define vertex buffer
        VPosColor vertices[] =
        {
            { RVector3( -1.0f,  1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
                                               
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f, -1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f, -1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f, -1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
                                               
            { RVector3( -1.0f, -1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
                                               
            { RVector3(  1.0f, -1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f, -1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
                                               
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f, -1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f, -1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
                                               
            { RVector3( -1.0f, -1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f, -1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f,  1.0f ), RVector4( 1.0f, 1.0f, 1.0f, 1.0f ) },
        };

        //////////////////////////////////////////
        // Define the index buffer
        uint16 indices[] =
        {
            3,1,0,
            2,1,3,

            6,4,5,
            7,4,6,

            11,9,8,
            10,9,11,

            14,12,13,
            15,12,14,

            19,17,16,
            18,17,19,

            22,20,21,
            23,20,22
        };

        sint nMesh = CreateMesh( VPosColor::VertexStride, ARRAY_LENGTH( vertices ), sizeof(uint16), ARRAY_LENGTH( indices ), vertices, indices, GFX_BUFFER_USAGE_DEFAULT );

        return nMesh;
    }

    sint CRenderer::LoadMesh( const char* szFilename )
    {        
        /********************* File Format ***********************\
        float   fVertexSize
        float   fVertexCount
        uint    nIndexSize
        uint    nIndexCount
        float   fVerts[]
        uint    nIndices[]
        \*********************************************************/
        uint nVertexStride;
        uint nVertexCount; 
        uint nIndexSize; 
        uint nIndexCount; 
        byte* pData;
    
        FILE* pFile = fopen( szFilename, "rb" );
        fread( &nVertexStride, sizeof( nVertexStride ), 1, pFile );
        fread( &nVertexCount, sizeof( nVertexCount ), 1, pFile );
        fread( &nIndexSize, sizeof( nIndexSize ), 1, pFile );
        nIndexSize = (nIndexSize == 32 ) ? 4 : 2;
        fread( &nIndexCount, sizeof( nIndexCount ), 1, pFile );

        pData   = new byte[ (nVertexStride * nVertexCount) + (nIndexCount * nIndexSize) ];

        fread( pData, nVertexStride, nVertexCount, pFile );
        fread( pData + (nVertexStride * nVertexCount), nIndexSize, nIndexCount, pFile );
        fclose( pFile );

        sint nMesh = CreateMesh( nVertexStride, nVertexCount, nIndexSize, nIndexCount, pData, pData + (nVertexStride * nVertexCount) );
        
        delete [] pData;
        
        return nMesh;
    }


    //-----------------------------------------------------------------------------
    //  LoadTextureXD
    //  Loads a texture
    //-----------------------------------------------------------------------------
    sint CRenderer::LoadTexture2D( const char* szFilename )
    {
        sint nIndex = AtomicIncrement( &m_nNumTextures ) - 1;

        m_ppTextures[ nIndex ] = m_pDevice->LoadTexture( szFilename );
        return nIndex;
    }

    //-----------------------------------------------------------------------------
    //  SetViewProj
    //  Sets the view projection constant buffer
    //-----------------------------------------------------------------------------
    void CRenderer::SetViewProj( const RMatrix4& pView, const RMatrix4& pProj )
    {
        RMatrix4 mMatrices[1] = 
        {
            ( pView * pProj ), 
        };

        mMatrices[0] = Transpose( mMatrices[0] );

        m_pDevice->UpdateBuffer( m_pViewProjCB, mMatrices );
        m_pDevice->SetVSConstantBuffer( 0, m_pViewProjCB );
    }

    //-----------------------------------------------------------------------------
    //  SetWorldMatrix
    //  Applies the world matrix to the pipeline
    //-----------------------------------------------------------------------------
    void CRenderer::SetWorldMatrix( const RMatrix4& pMatrix )
    {
        RMatrix4 mMatrix = Transpose( pMatrix );
        m_pDevice->UpdateBuffer( m_pWorldCB, &mMatrix );
        m_pDevice->SetVSConstantBuffer( 1, m_pWorldCB );
    }

    //-----------------------------------------------------------------------------
    //  SetCurrentView
    //  Sets the current camera view
    //-----------------------------------------------------------------------------
    void CRenderer::SetCurrentView( CView* pView )
    {
        m_pCurrentView = pView;
    }


    //-----------------------------------------------------------------------------
    //  AddCommand
    //  Adds a renderable object to the command buffer
    //-----------------------------------------------------------------------------
    void CRenderer::AddCommand( uint64 nCmd, RTransform& transform )
    {
        ASSERT( m_nNumCommands < MAX_RENDER_COMMANDS );

        uint nIndex = AtomicIncrement( &m_nNumCommands ) - 1;
        m_pCurrCommands[nIndex] = nCmd;

        m_pCurrTransforms[nIndex] = transform;
    }

    //-----------------------------------------------------------------------------
    //  AddLight
    //  Adds a light to the scene
    //-----------------------------------------------------------------------------
    void CRenderer::AddPointLight( const RVector3& vPos, float fRange )
    {
        ASSERT( m_pCurrLights->nNumActivePointLights < MAX_LIGHTS );
        sint nIndex = AtomicIncrement( &m_pCurrLights->nNumActivePointLights ) - 1;

        m_pCurrLights->vPointLights[ nIndex ] = RVector4( vPos, fRange );
    }

    void CRenderer::AddDirLight( const RVector3& vDir )
    {
        ASSERT( m_pCurrLights->nNumActiveDirLights < MAX_LIGHTS );
        sint nIndex = AtomicIncrement( &m_pCurrLights->nNumActiveDirLights ) - 1;

        m_pCurrLights->vDirLights[ nIndex ] = Homogonize( Normalize( -vDir ) );
    }

    //-----------------------------------------------------------------------------
    //  GetDefaultMeshData
    //  Returns the default mesh data
    //-----------------------------------------------------------------------------
    VPosNormalTex* CRenderer::GetDefaultMeshData( void )
    {
        static VPosNormalTex vertices[] =
        {
            { RVector3( -1.0f,  1.0f, -1.0f ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
            { RVector3(  1.0f,  1.0f, -1.0f ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f,  1.0f ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },
                                                                        
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
            { RVector3(  1.0f, -1.0f, -1.0f ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
            { RVector3(  1.0f, -1.0f,  1.0f ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
            { RVector3( -1.0f, -1.0f,  1.0f ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },
                                                                        
            { RVector3( -1.0f, -1.0f,  1.0f ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
            { RVector3( -1.0f,  1.0f, -1.0f ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f,  1.0f ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },
                                                                        
            { RVector3(  1.0f, -1.0f,  1.0f ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
            { RVector3(  1.0f, -1.0f, -1.0f ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
            { RVector3(  1.0f,  1.0f, -1.0f ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },
                                                                 
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 0.0f, 0.0f ) },
            { RVector3(  1.0f, -1.0f, -1.0f ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 1.0f, 0.0f ) },
            { RVector3(  1.0f,  1.0f, -1.0f ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f, -1.0f ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 0.0f, 1.0f ) },
                                                                 
            { RVector3( -1.0f, -1.0f,  1.0f ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 0.0f, 0.0f ) },
            { RVector3(  1.0f, -1.0f,  1.0f ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 1.0f, 0.0f ) },
            { RVector3(  1.0f,  1.0f,  1.0f ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 1.0f, 1.0f ) },
            { RVector3( -1.0f,  1.0f,  1.0f ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 0.0f, 1.0f ) },
        };

        return vertices;
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CRenderer::ProcessMessage( const TMessage& msg )
    {
        switch( msg.nType )
        {
        case mFullscreen:
            {
                break;
            }
        case mResize:
            {
                m_nWidth = (uint)(msg.nMessage >> 16);
                m_nHeight = msg.nMessage & 0x0000FFFF;

                m_pDevice->Resize( m_nWidth, m_nHeight );

                // Create the render target
                SAFE_RELEASE( m_pTestRT );
                m_pTestRT = m_pDevice->CreateRenderTarget( GFX_FORMAT_BYTE4, m_nWidth, m_nHeight );

                break;
            }
        default:
            {
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  DrawDebugSphere
    //  Renders a wireframe debug sphere
    //-----------------------------------------------------------------------------
    void CRenderer::DrawDebugSphere( const RSphere& fSphere )
    {
        if( !gnShowBoundingVolumes )
            return;

        TRenderCommand cmd;
        cmd.m_nMesh = m_nSphereMesh;
        cmd.m_nMaterial = eMatWireframe;
        
        RTransform t( RQuaternionZero(), fSphere.position, fSphere.radius );

        AddCommand( cmd.Encode(), t );
    }

    //-----------------------------------------------------------------------------
    //  DrawDebugBox
    //  Renders a wireframe debug AAB
    //-----------------------------------------------------------------------------
    void CRenderer::DrawDebugBox( const RAABB& box, const RVector3& vColor )
    {
        if( !gnShowBoundingVolumes )
            return;

        sint nIndex = AtomicIncrement( &m_nNumBoxes ) - 1;

        m_pCurrDebugBoxes[nIndex].box = box;
        m_pCurrDebugBoxes[nIndex].color = vColor;
    }

    //-----------------------------------------------------------------------------
    //  DrawDebugRay
    //  Draws a vector from the start point the length of the vector
    //-----------------------------------------------------------------------------
    void CRenderer::DrawDebugRay( const RVector3& start, const RVector3& dir )
    {
        sint nIndex = AtomicIncrement( &m_nNumRays ) - 1;

        m_pCurrDebugRays[nIndex].start = start;
        m_pCurrDebugRays[nIndex].end = start + dir;
    }

    //-----------------------------------------------------------------------------
    //  SwapBuffers
    //  Swaps last and previous frames buffers
    //-----------------------------------------------------------------------------
    void CRenderer::SwapBuffers( void )
    {
        Swap( m_pPrevCommands, m_pCurrCommands );
        Swap( m_pPrevTransforms, m_pCurrTransforms );
        Swap( m_pPrevDebugBoxes, m_pCurrDebugBoxes );
        Swap( m_pPrevDebugRays, m_pCurrDebugRays );
        Swap( m_pPrevLights, m_pCurrLights );

        m_nPrevNumCommands  = m_nNumCommands;
        m_nPrevNumBoxes     = m_nNumBoxes;
        m_nPrevNumRays      = m_nNumRays;

        m_nNumBoxes     = 0;
        m_nNumCommands  = 0;
        m_nNumRays      = 0;
        m_pCurrLights->nNumActivePointLights = 0;
        m_pCurrLights->nNumActiveDirLights = 0;

        UI::SwapBuffers();
    }

    //-----------------------------------------------------------------------------
    //  DrawText
    //  Renders a string of text on screen at (nLeft, nTop)
    //-----------------------------------------------------------------------------
    void CRenderer::DrawString( uint nLeft, uint nTop, const char* szText )
    {
        UI::AddString( nLeft, nTop, szText );
    }

    //-----------------------------------------------------------------------------
    //  Sort
    //  Sorts the list of commands for rendering efficiency
    //-----------------------------------------------------------------------------
    void CRenderer::Sort( void )
    {
        //uint nMeshInstanceCount[ MAX_MESHES ][ 1024 ] = { 0 };
        //////////////////////////////////////////
        // Transparency sorting
        sint nStart = 0;
        sint nEnd = m_nPrevNumCommands - 1;

        while( nStart < m_nPrevNumCommands )
        {
            if( DecodeTransparant( m_pPrevCommands[nStart] ) )
            {
                while( DecodeTransparant( m_pPrevCommands[ nEnd ] ) )
                {
                    --nEnd;
                }

                Swap( m_pPrevCommands[nStart], m_pPrevCommands[ nEnd ] );
                Swap( m_pPrevTransforms[nStart], m_pPrevTransforms[nEnd] );

                //uint  nMesh = DecodeMesh( m_pPrevCommands[nStart] );
                //uint& nCount = nMeshInstanceCount[ nMesh ][0];
                //nMeshInstanceCount[ nMesh ][ ++nCount ] = nStart;

                --nEnd;
            }



            nStart++;
        }

        //////////////////////////////////////////
        // Material Sorting

    }


} // namespace Riot
