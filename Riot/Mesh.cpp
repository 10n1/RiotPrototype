#include "Mesh.h"
#include "Direct3DDevice.h"

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
    D3D11_BUFFER_DESC bufferDesc = { 0 };
    bufferDesc.ByteWidth = nNumVertices * nVertexSize;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA initData = { 0 };
    initData.pSysMem = pVertices;

    pDevice->CreateBuffer( &bufferDesc, &initData, &m_pVertexBuffer );

    bufferDesc.ByteWidth = nNumIndices * sizeof( unsigned int );
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    initData.pSysMem = pIndices;

    pDevice->CreateBuffer( &bufferDesc, &initData, &m_pIndexBuffer );

    return 0;
}

void Mesh::Draw( void )
{
}
