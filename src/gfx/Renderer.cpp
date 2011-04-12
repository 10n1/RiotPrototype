/*********************************************************\
File:           Renderer.cpp
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       4/11/2011 10:54:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Renderer.h"
#include "System.h"

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
        m_pDevice = NULL;
    }

    //-----------------------------------------------------------------------------
    //  Shutdown
    //-----------------------------------------------------------------------------
    void CRenderer::Shutdown( void )
    {
        SAFE_RELEASE( m_pDevice );
    }

    //-----------------------------------------------------------------------------
    //  CreateGraphicsDevice
    //  Creates the device
    //-----------------------------------------------------------------------------
    void CRenderer::CreateGraphicsDevice( CWindow* pWindow )
    {
        m_pDevice = System::CreateOpenGLDevice( pWindow );
        m_pDevice->SetClearColor( 0.25f, 0.25f, 0.75f, 1.0f );
        m_pDevice->SetClearDepth( 1.0f );
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

                m_pDevice->SetViewport( nWidth, nHeight );

                break;
            }
        default:
            {
            }
        }
    }

} // namespace Riot
