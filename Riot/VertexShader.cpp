#include "VertexShader.h"
#include "Direct3DDevice.h"


VertexShader::VertexShader(void) 
    : m_pVertexShader(NULL)
    , m_pInputLayout(NULL)
    , m_pDevice(NULL)
    , m_pContext(NULL)
{
}


VertexShader::~VertexShader(void)
{
    SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pInputLayout );
}


int VertexShader::Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3DBlob* pBlob )
{
    HRESULT hr = S_OK;

    m_pDevice = pDevice;
    m_pContext = pContext;

    // Create the shader
    hr = m_pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &m_pVertexShader );

    if( FAILED( hr ) )
    {
        MessageBox( 0, "Shader creation failed!", "Error", 0 );
        return hr;
    }

    // Create the vertex layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
    };
	UINT numElements = ARRAYSIZE( layout );

    hr = pDevice->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &m_pInputLayout );
    
    if( FAILED( hr ) )
    {
        MessageBox( 0, "Input layout creation failed!", "Error", 0 );
        return hr;
    }

    return hr;
}

void VertexShader::SetShader( void )
{
    m_pContext->IASetInputLayout( m_pInputLayout );
    m_pContext->VSSetShader( m_pVertexShader, NULL, 0 );
}
