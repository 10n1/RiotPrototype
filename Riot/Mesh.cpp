#include "Mesh.h"
#include "Direct3DDevice.h"

static const int s_nIndexSize = sizeof( unsigned short );

Mesh::Mesh(void)
    : m_pVertices( NULL )
    , m_pIndices( NULL )
    , m_nNumVertices( 0 )
    , m_nNumIndices( 0 )
    , m_nVertexSize( 0 )
    , m_pDevice( NULL )
    , m_pContext( NULL )
    , m_pVertexBuffer( NULL )
    , m_pIndexBuffer( NULL )
{
}


Mesh::~Mesh(void)
{
    SAFE_RELEASE( m_pVertexBuffer );
    SAFE_RELEASE( m_pIndexBuffer );
    SAFE_DELETE( m_pVertices );
    SAFE_DELETE( m_pIndices );
}

int Mesh::Create( ID3D11Device* pDevice, 
                  ID3D11DeviceContext* pContext, 
                  void* pVertices, 
                  unsigned int nVertexSize, 
                  unsigned int nNumVertices,
                  void* pIndices,
                  unsigned int nNumIndices )
{
    HRESULT hr = S_OK;
    m_pDevice = pDevice;
    m_pContext = pContext;

    m_nVertexSize = nVertexSize;
    m_nNumVertices = nNumVertices;
    m_nNumIndices = nNumIndices;

    // Create the vertex buffer
    D3D11_BUFFER_DESC bufferDesc = { 0 };
    bufferDesc.ByteWidth = nNumVertices * nVertexSize;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = pVertices;

    hr = pDevice->CreateBuffer( &bufferDesc, &initData, &m_pVertexBuffer );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        MessageBox( 0, "Vertex buffer could not be created", "Error", 0 );
        return hr;
    }

    // Create the index buffer
    bufferDesc.ByteWidth = nNumIndices * s_nIndexSize;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    initData.pSysMem = pIndices;

    hr = pDevice->CreateBuffer( &bufferDesc, &initData, &m_pIndexBuffer );
    if( FAILED( hr ) )
    {
        // TODO: Handle for real
        MessageBox( 0, "Index buffer could not be created", "Error", 0 );
        return hr;
    }

    // TODO: Should we maintain the verts and indices in
    //  m_pVerts/m_pIndices, or just let the GPU maintain them
    //  I think the GPU should, theres no reason to keep
    //  a copy CPU side

    return hr;
}

void Mesh::Draw( void )
{
    // Set the buffers
    unsigned int stride = m_nVertexSize;
    unsigned int offset = 0;
    m_pContext->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
    m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_pContext->IASetIndexBuffer( m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    m_pContext->DrawIndexed( m_nNumIndices, 0, 0 );
}
