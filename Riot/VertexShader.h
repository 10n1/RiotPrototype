#pragma once
#include <D3D11.h>
#include <D3DX11.h>

class VertexShader
{
public:
    VertexShader(void);
    virtual ~VertexShader(void);

    int Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3DBlob* pBlob );
    void SetShader( void );
private:
    ID3D11Device*           m_pDevice;
    ID3D11DeviceContext*    m_pContext;
    ID3D11VertexShader*     m_pVertexShader;
    ID3D11InputLayout*      m_pInputLayout;
};

