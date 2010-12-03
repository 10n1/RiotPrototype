#include "PixelShader.h"
#include "Direct3DDevice.h"

PixelShader::PixelShader(void)
    : m_pPixelShader(NULL)
    , m_pDevice(NULL)
    , m_pContext(NULL)
{
}


PixelShader::~PixelShader(void)
{
    SAFE_RELEASE( m_pPixelShader );
}

int PixelShader::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3DBlob* pBlob )
{
    HRESULT hr = S_OK;

    m_pDevice = pDevice;
    m_pContext = pContext;

    // Create the shader
    hr = m_pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pPixelShader );

    if( FAILED( hr ) )
    {
        MessageBox( 0, "Shader creation failed!", "Error", 0 );
        return hr;
    }

    return hr;
}

void PixelShader::SetShader( void )
{
    m_pContext->PSSetShader( m_pPixelShader, NULL, 0 );
}
