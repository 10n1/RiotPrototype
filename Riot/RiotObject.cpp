#include "RiotObject.h"

extern Direct3DDevice* g_pD3D;

RiotObject::RiotObject(void)
    : m_mWorld(XMMatrixIdentity())
    , m_pMesh(NULL)
    , m_pVertexShader(NULL)
    , m_pPixelShader(NULL)
    , m_pDiffuseTexture(NULL)
{
}


RiotObject::~RiotObject(void)
{
    SAFE_DELETE(m_pMesh);
    SAFE_DELETE(m_pVertexShader);
    SAFE_DELETE(m_pPixelShader);
    SAFE_RELEASE(m_pDiffuseTexture);
}

int RiotObject::_Create( Mesh* pMesh, VertexShader* pVShader, PixelShader* pPShader, ID3D11ShaderResourceView* pDiffuseTexture )
{
    unsigned int hr = 0;

    m_pMesh = pMesh;
    m_pVertexShader = pVShader;
    m_pPixelShader = pPShader;
    m_pDiffuseTexture = pDiffuseTexture;

    return hr;
}

void RiotObject::Render( void )
{
    m_pVertexShader->SetShader();
    m_pPixelShader->SetShader();
    XMMATRIX mWorld = XMMatrixTranspose( m_mWorld );
    g_pD3D->SetWorldMatrix( mWorld );
    g_pD3D->GetContext()->PSSetShaderResources( 0, 1, &m_pDiffuseTexture );
    m_pMesh->Draw();
}
