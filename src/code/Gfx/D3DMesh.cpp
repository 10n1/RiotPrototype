/*********************************************************\
File:           D3DMesh.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/22/2011 7:59:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "D3DMesh.h"
#include <D3D11.h>
#include "memory.h"
#include <xnamath.h>

// CD3DMesh constructor
CD3DMesh::CD3DMesh()
    : m_pVertexLayout( NULL )
    , m_pVertexBuffer( NULL )
    , m_pIndexBuffer( NULL )
    , m_pDeviceContext( NULL )
    , m_pVertexShader( NULL )
    , m_pWorldMatrixCB( NULL )
{
}


// CD3DMesh destructor
CD3DMesh::~CD3DMesh()
{
    SAFE_RELEASE( m_pWorldMatrixCB );
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pVertexLayout );
    SAFE_RELEASE( m_pVertexBuffer );
    SAFE_RELEASE( m_pIndexBuffer );
}

//-----------------------------------------------------------------------------
//  DrawMesh
//  Passes the mesh to the GPU and renders it
//-----------------------------------------------------------------------------
void CD3DMesh::DrawMesh( void )
{
    // Set the shader
    m_pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );

    // Set constant buffer
    XMMATRIX mWorld = XMMatrixRotationQuaternion( m_vOrientation );
    mWorld = mWorld * XMMatrixTranslationFromVector( m_vPosition );
    mWorld = XMMatrixTranspose( mWorld );
    m_pDeviceContext->UpdateSubresource( m_pWorldMatrixCB, 0, NULL, &mWorld, 0, 0 );
    m_pDeviceContext->VSSetConstantBuffers( 1, 1, &m_pWorldMatrixCB );

    // Draw the mesh
    DXGI_FORMAT nIndexFormat = ( m_nIndexSize == 16 ) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT; // TODO: Clean this up
    uint nOffset = 0;
    m_pDeviceContext->IASetInputLayout( m_pVertexLayout );
    m_pDeviceContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &m_nVertexSize, &nOffset );
    m_pDeviceContext->IASetIndexBuffer( m_pIndexBuffer, nIndexFormat, 0 );
    m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ); // TODO: Will this ever change?
    m_pDeviceContext->DrawIndexed( m_nIndexCount, 0, 0 );
}
