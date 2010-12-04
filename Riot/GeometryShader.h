#pragma once
#include <D3D11.h>
#include <D3DX11.h>

class GeometryShader
{
public:
    GeometryShader(void);
    ~GeometryShader(void);
    
    int Create( ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3DBlob* pBlob );
    void SetShader( void );
private:
    ID3D11Device*           m_pDevice;
    ID3D11DeviceContext*    m_pContext;
    ID3D11GeometryShader*   m_pGeometryShader;
};

