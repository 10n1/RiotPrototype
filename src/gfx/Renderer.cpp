/*********************************************************\
File:           Renderer.cpp
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       4/27/2011 6:04:34 PM
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


namespace Riot
{
    bool bShowDebugSpheres = false;

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
        m_pDevice       = NULL;
        m_pViewProjCB   = NULL;
        m_pWorldCB      = NULL;

        m_pDefaultMesh      = NULL;        
        m_pDefaultVShader   = NULL;
        m_pDefaultVLayout   = NULL;
        m_pDefaultPShader   = NULL;
        m_pDefaultTexture   = NULL;
        m_pLinearSamplerState   = NULL;
        m_pNearestSamplerState  = NULL;

        m_pCurrentView  = NULL;

        m_nNumCommands  = 0;
        Memset( m_pRenderCommands, 0, sizeof( m_pRenderCommands ) );

        m_nNumActiveLights  = 0;
        m_bUpdateLighting   = false;

        m_nNumSpheres = 0;
        m_nNumBoxes = 0;

        m_pSphereMesh = NULL;
        m_pDebugBox = NULL;
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CRenderer::Shutdown( void )
    {
        SAFE_RELEASE( m_pDebugBox );
        SAFE_RELEASE( m_pSphereMesh );

        SAFE_RELEASE( VPosNormalTex::VertexLayoutObject );

        SAFE_RELEASE( m_pWhiteTexture );
        SAFE_RELEASE( m_pNearestSamplerState );
        SAFE_RELEASE( m_pDefaultTexture );
        SAFE_RELEASE( m_pLinearSamplerState );
        SAFE_RELEASE( m_pDefaultMesh );
        SAFE_RELEASE( m_pDefaultVShader );
        SAFE_RELEASE( m_pDefaultVLayout );
        SAFE_RELEASE( m_pDefaultPShader );

        SAFE_RELEASE( m_pWorldCB );
        SAFE_RELEASE( m_pViewProjCB );
        SAFE_RELEASE( m_pLightCB );
        SAFE_RELEASE( m_pDevice );
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
        m_pLightCB = m_pDevice->CreateConstantBuffer( sizeof( RVector4 ) * MAX_LIGHTS + 16, NULL );
        
        // Load the defaults
        CreateDefaultObjects();
    }

    //-----------------------------------------------------------------------------
    //  CreateDefaultObjects
    //  Creates the default objects
    //-----------------------------------------------------------------------------
    void CRenderer::CreateDefaultObjects( void )
    {
#if USE_OPENGL
        const char szVertexShader[] =  "Assets/Shaders/BasicVertexShader.glsl";
        const char szPixelShader[] =  "Assets/Shaders/BasicPixelShader.glsl";
#else
        const char szVertexShader[] =  "Assets/Shaders/BasicVertexShader.hlsl";
        const char szPixelShader[] =  "Assets/Shaders/BasicPixelShader.hlsl";
#endif

        // Create the vertex shader/layout
        m_pDevice->CreateVertexShaderAndLayout( szVertexShader, 
            "main", 
            VPosNormalTex::Layout, 
            VPosNormalTex::LayoutSize,
            &m_pDefaultVShader,
            &m_pDefaultVLayout );
        
        // pixel shader
        m_pDefaultPShader = m_pDevice->CreatePixelShader( szPixelShader, "main" );

        // sampler state
        m_pLinearSamplerState = m_pDevice->CreateSamplerState( GFX_TEXTURE_SAMPLE_LINEAR );
        m_pNearestSamplerState = m_pDevice->CreateSamplerState( GFX_TEXTURE_SAMPLE_NEAREST );

        // Texture
        m_pDefaultTexture = m_pDevice->LoadTexture( "Assets/Textures/DefaultTexture.png" );
        m_pWhiteTexture = m_pDevice->LoadTexture( "Assets/Textures/white.png" );

        // debug sphere
        m_pSphereMesh = LoadMesh( "Assets/meshes/sphere.mesh" );

        // debug box
        m_pDebugBox = CreateDynamicBox();

        // ...finally, set them
        m_pDevice->SetVertexLayout( m_pDefaultVLayout );
        m_pDevice->SetVertexShader( m_pDefaultVShader );
        m_pDevice->SetPixelShader( m_pDefaultPShader );
        m_pDevice->SetPrimitiveType( GFX_PRIMITIVE_TRIANGLELIST );
        m_pDevice->SetPSSamplerState( m_pLinearSamplerState );
    }

    //-----------------------------------------------------------------------------
    //  Render
    //  Performs rendering
    //-----------------------------------------------------------------------------
    void CRenderer::Render( void )
    {
        // Restore solid fill mode
        m_pDevice->SetFillMode( GFX_FILL_SOLID );

        // Update lighting
        if( m_bUpdateLighting )
        {
            m_pDevice->UpdateBuffer( m_pLightCB, m_vLights );
            m_pDevice->SetPSConstantBuffer( 0, m_pLightCB );
            m_bUpdateLighting = false;
        }

        // Clear
        m_pDevice->Clear();

        SetWorldMatrix( RMatrix4Identity() );

        // Render
        ASSERT( m_pCurrentView );
        RMatrix4 mView = m_pCurrentView->GetViewMatrix();
        RMatrix4 mProj = m_pCurrentView->GetProjMatrix();
        SetViewProj( mView, mProj );

        for( sint i = 0; i < m_nNumCommands; ++i )
        {
            RMatrix4 mWorld = m_pTransforms[i].GetTransformMatrix();
            SetWorldMatrix( mWorld );

            IGfxTexture2D* pTexture = m_pRenderCommands[i].pTexture;

            if( pTexture == NULL )
            {
                m_pDevice->SetPSSamplerState( m_pNearestSamplerState );
                pTexture = m_pDefaultTexture;
            }
            else
            {
                m_pDevice->SetPSSamplerState( m_pLinearSamplerState );
            }

            m_pDevice->SetPSTexture( 0, pTexture );
            m_pRenderCommands[i].pMesh->DrawMesh();
        }
        m_nNumCommands = 0;

        // Draw the debug cubes
        if( bShowDebugSpheres )
        {
            m_pDevice->SetFillMode( GFX_FILL_WIREFRAME );
            m_pDevice->SetPSSamplerState( m_pNearestSamplerState );
            m_pDevice->SetPSTexture( 0, m_pWhiteTexture );
            for( sint i = 0; i < m_nNumSpheres; ++i )
            {
                RMatrix4 mWorld = RMatrix4Scale( m_DebugSpheres[i].w );
                mWorld.r3 = m_DebugSpheres[i];
                mWorld.r3.w = 1.0f;
                SetWorldMatrix( mWorld );
                m_pSphereMesh->DrawMesh();
            }
            m_nNumSpheres = 0;
            m_pDevice->SetFillMode( GFX_FILL_SOLID );
        }

        // Draw the debug boxes
        {

            m_pDevice->SetFillMode( GFX_FILL_WIREFRAME );
            m_pDevice->SetPSSamplerState( m_pNearestSamplerState );
            m_pDevice->SetPSTexture( 0, m_pWhiteTexture );

            for( sint i = 0; i < m_nNumBoxes; ++i )
            {
                RVector3 vMin = m_DebugBoxesMin[i];
                RVector3 vMax = m_DebugBoxesMax[i];

                RMatrix4 mWorld = RMatrix4Identity();
                SetWorldMatrix( mWorld );

                VPosNormalTex vertices[] =
                {
                    { RVector3(  vMin.x,  vMax.y,  vMin.z ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMax.y,  vMin.z ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMax.y,  vMax.z ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
                    { RVector3(  vMin.x,  vMax.y,  vMax.z ), RVector3(  0.0f,  1.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },

                    { RVector3(  vMin.x,  vMin.y,  vMin.z ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMin.y,  vMin.z ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMin.y,  vMax.z ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
                    { RVector3(  vMin.x,  vMin.y,  vMax.z ), RVector3(  0.0f, -1.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },

                    { RVector3(  vMin.x,  vMin.y,  vMax.z ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
                    { RVector3(  vMin.x,  vMin.y,  vMin.z ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
                    { RVector3(  vMin.x,  vMax.y,  vMin.z ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
                    { RVector3(  vMin.x,  vMax.y,  vMax.z ), RVector3( -1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },

                    { RVector3(  vMax.x,  vMin.y,  vMax.z ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMin.y,  vMin.z ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMax.y,  vMin.z ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 1.0f, 1.0f ) },
                    { RVector3(  vMax.x,  vMax.y,  vMax.z ), RVector3(  1.0f,  0.0f,  0.0f ), RVector2( 0.0f, 1.0f ) },

                    { RVector3(  vMin.x,  vMin.y,  vMin.z ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 0.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMin.y,  vMin.z ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 1.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMax.y,  vMin.z ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 1.0f, 1.0f ) },
                    { RVector3(  vMin.x,  vMax.y,  vMin.z ), RVector3(  0.0f,  0.0f, -1.0f ), RVector2( 0.0f, 1.0f ) },

                    { RVector3(  vMin.x,  vMin.y,  vMax.z ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 0.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMin.y,  vMax.z ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 1.0f, 0.0f ) },
                    { RVector3(  vMax.x,  vMax.y,  vMax.z ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 1.0f, 1.0f ) },
                    { RVector3(  vMin.x,  vMax.y,  vMax.z ), RVector3(  0.0f,  0.0f,  1.0f ), RVector2( 0.0f, 1.0f ) },
                };

                m_pDevice->UpdateBuffer( m_pDebugBox->m_pVertexBuffer, vertices );

                m_pDebugBox->DrawMesh();
            }

            m_nNumBoxes = 0;
            m_pDevice->SetFillMode( GFX_FILL_SOLID );
        }

        // Present
        m_pDevice->Present();
    }


    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates a mesh from the file
    //-----------------------------------------------------------------------------
    CMesh* CRenderer::CreateMesh(   uint nVertexStride, 
                                    uint nVertexCount, 
                                    uint nIndexSize, 
                                    uint nIndexCount, 
                                    void* pVertices, 
                                    void* pIndices,
                                    GFX_BUFFER_USAGE nUsage )
    {
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

        return pMesh;
    }
    CMesh* CRenderer::CreateMesh( void )
    {
        // If the cube has already been created, return it
        if( m_pDefaultMesh )
        {
            m_pDefaultMesh->AddRef();
            return m_pDefaultMesh;
        }
        
        //////////////////////////////////////////
        // Define vertex buffer
        VPosNormalTex vertices[] =
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

        m_pDefaultMesh = CreateMesh( VPosNormalTex::VertexStride, ARRAY_LENGTH( vertices ), sizeof(uint16), ARRAY_LENGTH( indices ), vertices, indices, GFX_BUFFER_USAGE_IMMUTABLE );

        m_pDefaultMesh->AddRef();
        return m_pDefaultMesh;
    }
    CMesh* CRenderer::CreateDynamicBox( void )
    {
        //////////////////////////////////////////
        // Define vertex buffer
        VPosNormalTex vertices[] =
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

        CMesh* pMesh = CreateMesh( VPosNormalTex::VertexStride, ARRAY_LENGTH( vertices ), sizeof(uint16), ARRAY_LENGTH( indices ), vertices, indices, GFX_BUFFER_USAGE_DYNAMIC );

        return pMesh;
    }

    CMesh* CRenderer::LoadMesh( const char* szFilename )
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
        void* pVertices;
        void* pIndices;
    
        FILE* pFile = fopen( szFilename, "rb" );
        fread( &nVertexStride, sizeof( nVertexStride ), 1, pFile );
        fread( &nVertexCount, sizeof( nVertexCount ), 1, pFile );
        fread( &nIndexSize, sizeof( nIndexSize ), 1, pFile );
        nIndexSize = (nIndexSize == 32 ) ? 4 : 2;
        fread( &nIndexCount, sizeof( nIndexCount ), 1, pFile );

        pVertices   = new byte[nVertexStride * nVertexCount];
        pIndices    = new byte[nIndexCount * nIndexSize];

        fread( pVertices, nVertexStride, nVertexCount, pFile );
        fread( pIndices, nIndexSize, nIndexCount, pFile );
        fclose( pFile );


        CMesh* pMesh = NULL;

        pMesh = CreateMesh( nVertexStride, nVertexCount, nIndexSize, nIndexCount, pVertices, pIndices );
        
        delete [] pIndices;
        delete [] pVertices;
        
        return pMesh;
    }


    //-----------------------------------------------------------------------------
    //  LoadTextureXD
    //  Loads a texture
    //-----------------------------------------------------------------------------
    IGfxTexture2D* CRenderer::LoadTexture2D( const char* szFilename )
    {
        return m_pDevice->LoadTexture( szFilename );
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
    void CRenderer::AddCommand( const TRenderCommand& cmd, RTransform& transform )
    {
        ASSERT( m_nNumCommands < MAX_RENDER_COMMANDS );

        uint nIndex = AtomicIncrement( &m_nNumCommands ) - 1;
        m_pRenderCommands[nIndex] = cmd;

        m_pTransforms[nIndex] = transform;
    }

    //-----------------------------------------------------------------------------
    //  SetLight
    //  Sets the specific light
    //-----------------------------------------------------------------------------
    void CRenderer::SetLight( const RVector3& vPos, uint nIndex )
    {
        static CMutex mutex;

        mutex.Lock();
        m_bUpdateLighting = true;
        m_vLights[ nIndex ] = Homogonize(vPos);
        m_nNumActiveLights = nIndex + 1;
        mutex.Unlock();
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
                uint nWidth = (uint)(msg.nMessage >> 16);
                uint nHeight = msg.nMessage & 0x0000FFFF;

                m_pDevice->Resize( nWidth, nHeight );

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
    void CRenderer::DrawDebugSphere( const RVector4& fSphere )
    {
        sint nIndex = AtomicIncrement( &m_nNumSpheres ) - 1;

        m_DebugSpheres[nIndex] = fSphere;
    }

    //-----------------------------------------------------------------------------
    //  DrawDebugBox
    //  Renders a wireframe debug AAB
    //-----------------------------------------------------------------------------
    void CRenderer::DrawDebugBox( const RVector3& vMin,const RVector3& vMax )
    {
        sint nIndex = AtomicIncrement( &m_nNumBoxes ) - 1;

        m_DebugBoxesMin[nIndex] = vMin;
        m_DebugBoxesMax[nIndex] = vMax;
    }

} // namespace Riot
