/*********************************************************\
File:           Renderer.cpp
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       4/14/2011 8:07:53 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Renderer.h"
#include "System.h"
#include "Mesh.h"
#include "VertexFormats.h"

#if USE_OPENGL
#include "D3DGraphics.h"
#else
#include "OGLGraphics.h"
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
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CRenderer::Shutdown( void )
    {
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
        m_pViewProjCB = m_pDevice->CreateConstantBuffer( sizeof( RMatrix4 ) );
        m_pWorldCB = m_pDevice->CreateConstantBuffer( sizeof( RMatrix4 ) );
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
                                    uint nIndexFormat, 
                                    uint nIndexCount, 
                                    void* pVertices, 
                                    void* pIndices )
    {
        CMesh* pMesh = new CMesh;

        InputElementLayout x = VVertexPosNormal::Layout[1];

        return pMesh;
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
