/*********************************************************\
File:           Renderer.cpp
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       4/14/2011 10:44:54 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Renderer.h"
#include "System.h"
#include "Mesh.h"
#include "VertexFormats.h"

#if USE_OPENGL
#include "OGLGraphics.h"
#else
#include "D3DGraphics.h"
#endif

namespace Riot
{

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
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CRenderer::Shutdown( void )
    {
        SAFE_RELEASE( VPosNormal::VertexLayoutObject );

        SAFE_RELEASE( m_pDefaultMesh );
        SAFE_RELEASE( m_pDefaultVShader );
        SAFE_RELEASE( m_pDefaultVLayout );
        SAFE_RELEASE( m_pDefaultPShader );

        SAFE_RELEASE( m_pWorldCB );
        SAFE_RELEASE( m_pViewProjCB );
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
        
        // Load the defaults
        CreateDefaultObjects();
    }

    //-----------------------------------------------------------------------------
    //  CreateDefaultObjects
    //  Creates the default objects
    //-----------------------------------------------------------------------------
    void CRenderer::CreateDefaultObjects( void )
    {
        // Create the vertex shader/layout
        m_pDevice->CreateVertexShaderAndLayout( L"Assets/Shaders/StandardVertexShader.hlsl", 
            "VS", 
            VPosNormal::Layout, 
            VPosNormal::LayoutSize,
            &m_pDefaultVShader,
            &m_pDefaultVLayout );
        
        // ...then the pixel shader
        m_pDefaultPShader = m_pDevice->CreatePixelShader( L"Assets/Shaders/StandardVertexShader.hlsl", "PS" );


        // ...finally, set them
        m_pDevice->SetVertexLayout( m_pDefaultVLayout );
        m_pDevice->SetVertexShader( m_pDefaultVShader );
        m_pDevice->SetPixelShader( m_pDefaultPShader );
        m_pDevice->SetPrimitiveType( GFX_PRIMITIVE_TRIANGLELIST );
    }

    //-----------------------------------------------------------------------------
    //  Render
    //  Performs rendering
    //-----------------------------------------------------------------------------
    void CRenderer::Render( void )
    {
        m_pDevice->Clear();
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
                                    void* pIndices )
    {
        CMesh* pMesh = new CMesh;

        //////////////////////////////////////////
        // Create the vertex buffer
        pMesh->m_pVertexBuffer = m_pDevice->CreateVertexBuffer( nVertexStride * nVertexCount, pVertices );
        
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
        VPosNormal vertices[] =
        {
            { RVector3( -1.0f, 1.0f, -1.0f ), RVector3( 0.0f, 1.0f, 0.0f ) },
            { RVector3( 1.0f, 1.0f, -1.0f ), RVector3( 0.0f, 1.0f, 0.0f ) },
            { RVector3( 1.0f, 1.0f, 1.0f ), RVector3( 0.0f, 1.0f, 0.0f ) },
            { RVector3( -1.0f, 1.0f, 1.0f ), RVector3( 0.0f, 1.0f, 0.0f ) },

            { RVector3( -1.0f, -1.0f, -1.0f ), RVector3( 0.0f, -1.0f, 0.0f ) },
            { RVector3( 1.0f, -1.0f, -1.0f ), RVector3( 0.0f, -1.0f, 0.0f ) },
            { RVector3( 1.0f, -1.0f, 1.0f ), RVector3( 0.0f, -1.0f, 0.0f ) },
            { RVector3( -1.0f, -1.0f, 1.0f ), RVector3( 0.0f, -1.0f, 0.0f ) },

            { RVector3( -1.0f, -1.0f, 1.0f ), RVector3( -1.0f, 0.0f, 0.0f ) },
            { RVector3( -1.0f, -1.0f, -1.0f ), RVector3( -1.0f, 0.0f, 0.0f ) },
            { RVector3( -1.0f, 1.0f, -1.0f ), RVector3( -1.0f, 0.0f, 0.0f ) },
            { RVector3( -1.0f, 1.0f, 1.0f ), RVector3( -1.0f, 0.0f, 0.0f ) },

            { RVector3( 1.0f, -1.0f, 1.0f ), RVector3( 1.0f, 0.0f, 0.0f ) },
            { RVector3( 1.0f, -1.0f, -1.0f ), RVector3( 1.0f, 0.0f, 0.0f ) },
            { RVector3( 1.0f, 1.0f, -1.0f ), RVector3( 1.0f, 0.0f, 0.0f ) },
            { RVector3( 1.0f, 1.0f, 1.0f ), RVector3( 1.0f, 0.0f, 0.0f ) },

            { RVector3( -1.0f, -1.0f, -1.0f ), RVector3( 0.0f, 0.0f, -1.0f ) },
            { RVector3( 1.0f, -1.0f, -1.0f ), RVector3( 0.0f, 0.0f, -1.0f ) },
            { RVector3( 1.0f, 1.0f, -1.0f ), RVector3( 0.0f, 0.0f, -1.0f ) },
            { RVector3( -1.0f, 1.0f, -1.0f ), RVector3( 0.0f, 0.0f, -1.0f ) },

            { RVector3( -1.0f, -1.0f, 1.0f ), RVector3( 0.0f, 0.0f, 1.0f ) },
            { RVector3( 1.0f, -1.0f, 1.0f ), RVector3( 0.0f, 0.0f, 1.0f ) },
            { RVector3( 1.0f, 1.0f, 1.0f ), RVector3( 0.0f, 0.0f, 1.0f ) },
            { RVector3( -1.0f, 1.0f, 1.0f ), RVector3( 0.0f, 0.0f, 1.0f ) },
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

        m_pDefaultMesh = CreateMesh( VPosNormal::VertexStride, ARRAY_LENGTH( vertices ), sizeof(uint16), ARRAY_LENGTH( indices ), vertices, indices );

        m_pDefaultMesh->AddRef();
        return m_pDefaultMesh;
    }


    //-----------------------------------------------------------------------------
    //  SetViewProj
    //  Sets the view projection constant buffer
    //-----------------------------------------------------------------------------
    void CRenderer::SetViewProj( const void* pView, const void* pProj )
    {
        RMatrix4 mMatrices[1] = 
        {
            ( *((RMatrix4*)pView) * *((RMatrix4*)pProj)), 
        };

        Transpose( mMatrices[0] );

        m_pDevice->UpdateBuffer( m_pViewProjCB, mMatrices );
        m_pDevice->SetVSConstantBuffer( 0, m_pViewProjCB );
    }

    //-----------------------------------------------------------------------------
    //  SetWorldMatrix
    //  Applies the world matrix to the pipeline
    //-----------------------------------------------------------------------------
    void CRenderer::SetWorldMatrix( RMatrix4* pMatrix )
    {
        m_pDevice->UpdateBuffer( m_pWorldCB, pMatrix );
        m_pDevice->SetVSConstantBuffer( 1, m_pViewProjCB );
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
                uint nWidth = msg.nMessage >> 16;
                uint nHeight = msg.nMessage & 0x0000FFFF;

                m_pDevice->Resize( nWidth, nHeight );

                break;
            }
        default:
            {
            }
        }
    }

} // namespace Riot
