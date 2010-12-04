#include "GeometryShader.h"
#include "Direct3DDevice.h"

GeometryShader::GeometryShader(void)
    : m_pGeometryShader(NULL)
    , m_pDevice(NULL)
    , m_pContext(NULL)
{
}


GeometryShader::~GeometryShader(void)
{
    SAFE_RELEASE( m_pGeometryShader );
}

int GeometryShader::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3DBlob* pBlob )
{
    HRESULT hr = S_OK;

    m_pDevice = pDevice;
    m_pContext = pContext;

    // Create the shader
    hr = m_pDevice->CreateGeometryShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pGeometryShader );

    if( FAILED( hr ) )
    {
        MessageBox( 0, "Shader creation failed!", "Error", 0 );
        return hr;
    }

    return hr;
}

void GeometryShader::SetShader( void )
{
    m_pContext->GSSetShader( m_pGeometryShader, NULL, 0 );
}
