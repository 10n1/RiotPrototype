#pragma once
#include "Direct3DDevice.h"

class __declspec( align(16) ) RiotObject
{
public:
    RiotObject(void);
    virtual ~RiotObject(void);

    //-----------------------------------------------------------------------------
    //  _Create
    //  Internal object function to create an object from the render objects
    //-----------------------------------------------------------------------------
    int _Create( Mesh* pMesh, VertexShader* pVShader, PixelShader* pPShader, ID3D11ShaderResourceView* pDiffuseTexture );

    //-----------------------------------------------------------------------------
    //  Render
    //  Renders the object
    //-----------------------------------------------------------------------------
    void Render( void );

    //-----------------------------------------------------------------------------
    //  Accessors
    //-----------------------------------------------------------------------------
    XMMATRIX GetWorldMatrix( void ) { return m_mWorld; }

    //-----------------------------------------------------------------------------
    //  Mutators
    //-----------------------------------------------------------------------------
    void SetWorldMatrix( XMMATRIX& mWorld ) { m_mWorld = mWorld; }
private:
    XMMATRIX                    m_mWorld;
    Mesh*                       m_pMesh;
    VertexShader*               m_pVertexShader;
    PixelShader*                m_pPixelShader;
    ID3D11ShaderResourceView*   m_pDiffuseTexture;
};

