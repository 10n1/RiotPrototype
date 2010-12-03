#pragma once
#include <D3D11.h>
#include <D3DX11.h>

class Mesh
{
public:
    Mesh(void);
    ~Mesh(void);

    int Create( ID3D11Device* pDevice, 
                ID3D11DeviceContext* pContext, 
                void* pVertices, 
                unsigned int nVertexSize, 
                unsigned int nNumVertices,
                void* pIndices,
                unsigned int nNumIndices );

    void Draw( void );

private:
    void*           m_pVertices;
    void*           m_pIndices;
    unsigned int    m_nNumVertices;
    unsigned int    m_nNumIndices;
    unsigned int    m_nVertexSize;

    ID3D11Device*           m_pDevice;
    ID3D11DeviceContext*    m_pContext;
    ID3D11Buffer*           m_pVertexBuffer;
    ID3D11Buffer*           m_pIndexBuffer;
};

