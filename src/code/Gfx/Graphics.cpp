/*********************************************************\
File:           GraphicsDevice.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/4/2011 8:10:04 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Graphics.h"
#include "Window.h"
#include "Memory.h"
#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"

// CGraphics constructor
CGraphics::CGraphics()
    : m_pWindow( NULL )
    , m_pCurrView( NULL )
    , m_nNumCommands( 0 )
    , m_bUpdateLighting( false )
    , m_nActiveLights( 0 )
    , m_pBoxMesh( NULL )
    , m_nNumMatrices( 0 )
{
    memset( m_ppRenderCommands, 0, sizeof( m_ppRenderCommands ) );
}

// CGraphics destructor
CGraphics::~CGraphics()
{
    SAFE_RELEASE( m_pBoxMesh );
}


//-----------------------------------------------------------------------------
//  Resize
//  Resizes the device
//-----------------------------------------------------------------------------
void CGraphics::Resize( uint nWidth, uint nHeight )
{    
    // Minimized protection
    // TODO: Better way to do this?
    if( nWidth != 0 && nHeight != 0 )
    {
        ReleaseBuffers();
        CreateBuffers( nWidth, nHeight );
    }
}

//-----------------------------------------------------------------------------
//  SetViewProj
//  Sets the view projection constant buffer
//-----------------------------------------------------------------------------
void CGraphics::SetCurrentView( const CView* pView )
{
    m_pCurrView = (CView*)pView;
}

//-----------------------------------------------------------------------------
//  Render
//  Renders everything
//-----------------------------------------------------------------------------
void CGraphics::Render( void )
{
    uint nMatrix = 0;
    for( uint i = 0; i < m_nNumCommands; ++i )
    {
        m_ppRenderCommands[i].m_pMaterial->ProcessObject();
        SetWorldMatrix( m_ppMatrices[nMatrix++] );
        m_ppRenderCommands[i].m_pMesh->ProcessObject();
    }

    m_nNumCommands = 0;
    m_nNumMatrices = 0;
}

//-----------------------------------------------------------------------------
//  AddCommand
//  Adds a renderable object to the command buffer
//-----------------------------------------------------------------------------
void CGraphics::AddCommand( const CRenderCommand& command )
{
    m_ppRenderCommands[ m_nNumCommands++ ] = command;
}



//-----------------------------------------------------------------------------
//  SetLight
//  Sets the specific light
//-----------------------------------------------------------------------------
void CGraphics::SetLight( const XMVECTOR& vDir, uint nIndex )
{
    m_bUpdateLighting = true;
    m_vLights[ nIndex ] = vDir;
    m_nActiveLights = nIndex + 1;
}

//-----------------------------------------------------------------------------
//  AddMatrix
//  Adds a matrix to the list of world matrices
//-----------------------------------------------------------------------------
void CGraphics::AddMatrix( XMMATRIX* pMatrix )
{
    m_ppMatrices[ m_nNumMatrices++ ] = pMatrix;
}
