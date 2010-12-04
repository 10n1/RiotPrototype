/*********************************************************\
File:           main.cpp
\*********************************************************/
#pragma once
#include <Windows.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include "Mesh.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) if(p) { p->Release(); p = NULL; } 
#endif


#ifndef SAFE_DELETE
#define SAFE_DELETE(p) if(p) { delete p; p = NULL; } 
#endif

class Direct3DDevice
{
public:
    Direct3DDevice(void);
    ~Direct3DDevice(void);
    
    //-----------------------------------------------------------------------------
    //  Initialize
    //  Initializes DirectX
    //-----------------------------------------------------------------------------
    int Initialize( HWND hWnd );

    //-----------------------------------------------------------------------------
    //  Resize
    //  Resizes the DX buffers, etc.
    //-----------------------------------------------------------------------------
    int Resize( unsigned int nWidth, unsigned int nHeight );

    //-----------------------------------------------------------------------------
    //  Render
    //  Renders the scene
    //-----------------------------------------------------------------------------
    void Render( void );

    //-----------------------------------------------------------------------------
    //  CreateShader
    //  Creates a shader from a file
    //-----------------------------------------------------------------------------
    int CreateShader( VertexShader** pShader, const char* szFilename = 0, const char* szEntryPoint = 0, const char* szShaderModel = 0 );
    int CreateShader( PixelShader** pShader, const char* szFilename = 0, const char* szEntryPoint = 0, const char* szShaderModel = 0 );
    int CreateShader( GeometryShader** pShader, const char* szFilename = 0, const char* szEntryPoint = 0, const char* szShaderModel = 0);

    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates a shader from raw data
    //-----------------------------------------------------------------------------
    int CreateMesh( void* pVertices, 
                    unsigned int nVertexSize, 
                    unsigned int nNumVertices,
                    void* pIndices,
                    unsigned int nNumIndices,
                    Mesh** pMesh );

    //-----------------------------------------------------------------------------
    //  Set*Matrix
    //  Sets the appropriate constant buffer on the GPU
    //-----------------------------------------------------------------------------
    void SetViewProjMatrix( XMMATRIX& mViewProj );
    void SetWorldMatrix( XMMATRIX& mWorld );

    //-----------------------------------------------------------------------------
    //  Mutators
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  Accessors
    //-----------------------------------------------------------------------------
    ID3D11Device* GetDevice( void ) { return m_pDevice; }
    ID3D11DeviceContext* GetContext( void ) { return m_pContext; }

private:
    //-----------------------------------------------------------------------------
    //  CompileShader
    //  Compiles a shader from a file
    //-----------------------------------------------------------------------------
    int CompileShader( const char* szFilename, const char* szEntryPoint, const char* szShaderModel, ID3DBlob** ppBlob );

private:
    ID3D11Device*           m_pDevice;
    ID3D11DeviceContext*    m_pContext;
    IDXGISwapChain*         m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11Texture2D*        m_pDepthStencilResource;
    ID3D11DepthStencilView* m_pDepthStencilView;

    ID3D11Buffer*           m_pViewProjCB;
    ID3D11Buffer*           m_pWorldCB;
};

