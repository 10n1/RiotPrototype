/*********************************************************\
File:       UI.cpp
Purpose:    User interface...renders to a separate target
\*********************************************************/
#include "UI.h"
#include "Engine.h"
#include "Renderer.h"
#include <string.h>

namespace Riot
{
    //////////////////////////////////////////
    // UI vertex definition
    typedef struct _UIVertex
    {
        RVector3 vPos;
        RVector3 vColor;
        RVector2 vTexcoord;
    } UIVertex;

    //////////////////////////////////////////
    // static members
    float32                UI::m_fScreenX      = 0.0f;
    float32                UI::m_fScreenY      = 0.0f;
    IGraphicsDevice*       UI::m_pDevice       = NULL;
    IGfxVertexShader*      UI::m_pVertexShader = NULL;
    IGfxPixelShader*       UI::m_pPixelShader  = NULL;
    IGfxVertexLayout*      UI::m_pInputLayout  = NULL;
    IGfxSamplerState*      UI::m_pFontSampler  = NULL;
    IGfxTexture2D*         UI::m_pFontTexture  = NULL;
    IGfxBlendState*        UI::m_pFontBlend    = NULL;
    IGfxBuffer*            UI::m_pVertexBuffer = NULL;
    static const uint      gs_nMaxNumChars     = 255 * 6;

    static const uint   gs_nMaxNumStrings   = 100;
    UIString*           UI::m_pUIStrings[2] = { 0 };//new UIString[ gs_nMaxNumStrings ];
    atomic_t            UI::m_nNumStrings   = 0;
    uint                UI::m_nNumPrevStrings = 0;
    UIString*           UI::m_pCurrStrings = NULL;
    UIString*           UI::m_pPrevStrings = NULL;

    //-----------------------------------------------------------------------------
    //  Initialize
    //  Define shaders, input layout, and font texture
    //-----------------------------------------------------------------------------
    void UI::Initialize( IGraphicsDevice* pDevice )
    {
        m_pUIStrings[0] = new UIString[ gs_nMaxNumStrings ];
        m_pUIStrings[1] = new UIString[ gs_nMaxNumStrings ];

        m_pPrevStrings = m_pUIStrings[0];
        m_pCurrStrings = m_pUIStrings[1];

        m_nNumStrings = 0;
        m_nNumPrevStrings = 0;

        //////////////////////////////////////////
        // Load vertex shader
        pDevice->CreateVertexShaderAndLayout( "Assets/Shaders/UI",
                                              "VS",
                                              VPosColorTex::Layout,
                                              VPosColorTex::LayoutSize,
                                              &m_pVertexShader,
                                              &m_pInputLayout );

        //////////////////////////////////////////
        // Load pixel shader
        m_pPixelShader = pDevice->CreatePixelShader( "Assets/Shaders/UI", "PS" );

        //////////////////////////////////////////
        // Load the font texture
        m_pFontTexture = pDevice->LoadTexture( "Assets/Textures/Font.dds" );


        //////////////////////////////////////////
        // Create the sampler state
        m_pFontSampler = pDevice->CreateSamplerState( GFX_TEXTURE_SAMPLE_NEAREST );

        ////////////////////////////////////////////
        //// Create the blend state
        //D3D11_BLEND_DESC blendDesc;
        //ZeroMemory( &blendDesc, sizeof( D3D11_BLEND_DESC ) );
        //blendDesc.AlphaToCoverageEnable = false;
        //blendDesc.IndependentBlendEnable = false;
        //blendDesc.RenderTarget[0].BlendEnable = true;
        //blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        //blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        //blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        //blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        //blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        //blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        //blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;
        //hr = m_pDevice->CreateBlendState( &blendDesc, &m_pFontBlend );
        //if( FAILED( hr ) )
        //{
        //    // TODO: Handle error gracefully
        //    DebugBreak();
        //    MessageBox( 0, L"Failed to create the blend sampler", L"Error", 0 );
        //}

        //////////////////////////////////////////
        // Init the vertex buffer
        UIVertex*               pVertices       = new UIVertex[ gs_nMaxNumChars ];

        for( uint i = 0; i < gs_nMaxNumChars; ++i )
        {
            pVertices[ i ].vPos = RVector3( 0.0f, 0.0f, 0.0f );
            pVertices[ i ].vColor = RVector3( 1.0f, 0.0f, 0.0f );
            pVertices[ i ].vTexcoord = RVector2( 0.0f, 0.0f );
        }

        m_pVertexBuffer = pDevice->CreateVertexBuffer( gs_nMaxNumChars * sizeof( UIVertex ), pVertices, Riot::GFX_BUFFER_USAGE_DYNAMIC );

        SAFE_DELETE_ARRAY( pVertices );
    }

    //-----------------------------------------------------------------------------
    //  Destroy
    //  Release the memory allocated on Init
    //-----------------------------------------------------------------------------
    void UI::Destroy( void )
    {
        SAFE_DELETE_ARRAY( m_pUIStrings[0] );
        SAFE_DELETE_ARRAY( m_pUIStrings[1] );

        SAFE_RELEASE( m_pVertexShader );
        SAFE_RELEASE( m_pPixelShader );
        SAFE_RELEASE( m_pInputLayout );
        SAFE_RELEASE( m_pFontSampler );
        SAFE_RELEASE( m_pFontTexture );
        SAFE_RELEASE( m_pVertexBuffer );
    }

    //-----------------------------------------------------------------------------
    //  SwapBuffers
    //  Swaps last and previous frames buffers
    //-----------------------------------------------------------------------------
    void UI::SwapBuffers( void )
    {
        Swap( m_pPrevStrings, m_pCurrStrings );
        m_nNumPrevStrings = m_nNumStrings;
        m_nNumStrings = 0;
    }

    //-----------------------------------------------------------------------------
    //  AddText
    //  Add some text to the list of strings
    //-----------------------------------------------------------------------------
    void UI::AddString( uint nLeft, uint nTop, const char* szText )
    {
        sint nIndex = AtomicIncrement( &m_nNumStrings ) - 1;
        m_pCurrStrings[ nIndex ].nLeft = nLeft;
        m_pCurrStrings[ nIndex ].nTop = nTop;
        strcpy( m_pCurrStrings[ nIndex ].szText, szText );
    }

    //-----------------------------------------------------------------------------
    //  Draw()
    //  Draw all the strings
    //-----------------------------------------------------------------------------
    void UI::Draw( IGraphicsDevice* pDevice )
    {
        // draw all strings
        for( uint i = 0; i < m_nNumPrevStrings; ++i )
        {
            DrawString( pDevice, m_pPrevStrings[ i ].nLeft, m_pPrevStrings[ i ].nTop, m_pPrevStrings[ i ].szText );
        }

        //AtomicExchange( &m_nNumStrings, 0 );
    }

    //-----------------------------------------------------------------------------
    //  DrawString
    //  Draw szText at (nLeft, nTop)
    //-----------------------------------------------------------------------------
    void UI::DrawString( IGraphicsDevice* pDevice, uint nLeft, uint nTop, const char* szText )
    {
        float32 fScaleFactor = 2.0f;
        float32 fCharWidth = (950.0f / 95.0f) / 950.0f;
        float32 fFontWidth = ( ( 950.0f / 95.0f ) / 1024.0f ) * fScaleFactor;
        float32 fFontHeight = (20.0f / 768.0f) * fScaleFactor;
        uint nNumChars = (uint)strlen( szText );

        // adjust screen coords to have (0, 0) at the top left
        // and increase X left -> right and Y top -> bottom
        m_fScreenX = 2.0f * ( nLeft / 1024.0f ) - 1.0f;  // [-1, 1]
        m_fScreenY = -2.0f * ( nTop / 768.0f ) + 1.0f - fFontHeight; // [1-font_height, -1-font_height]

        // Vertices info
        uint nNumVertices = nNumChars * 6;
        UIVertex* pVertices = new UIVertex[ nNumVertices ];
        uint j = 0;

        // Create quads for the string
        for( uint i = 0; i < nNumChars; ++i )
        {
            float32 fCurrentScreenX = m_fScreenX + ( i * fFontWidth );
            float32 fLeftX = fCurrentScreenX;
            float32 fRightX = fCurrentScreenX + (fCharWidth * fScaleFactor);
            float32 fTopY = m_fScreenY + fFontHeight;
            float32 fBottomY = m_fScreenY;

            char cCurrentChar = szText[i];
            uint nAsciiPos = cCurrentChar - 32;
            float32 fTexcoord_x0 = nAsciiPos * fCharWidth;      // left
            float32 fTexcoord_x1 = fTexcoord_x0 + fCharWidth;   // right
            float32 fTexcoord_y0 = 0.0f;                        // top
            float32 fTexcoord_y1 = 1.0f;                        // bottom

            j = i * 6;

            // Triangle 1
            // left bottom
            pVertices[ j + 0 ].vPos      = RVector3( fLeftX,  fBottomY, 0.0f );
            pVertices[ j + 0 ].vColor    = RVector3( 1.0f, 1.0f, 0.0f );
            pVertices[ j + 0 ].vTexcoord = RVector2( fTexcoord_x0, fTexcoord_y1 );
            // left top
            pVertices[ j + 1 ].vPos      = RVector3( fLeftX,  fTopY, 0.0f );
            pVertices[ j + 1 ].vColor    = RVector3( 1.0f, 1.0f, 0.0f );
            pVertices[ j + 1 ].vTexcoord = RVector2( fTexcoord_x0, fTexcoord_y0 );
            // right top
            pVertices[ j + 2 ].vPos      = RVector3( fRightX, fTopY, 0.0f );
            pVertices[ j + 2 ].vColor    = RVector3( 1.0f, 1.0f, 0.0f );
            pVertices[ j + 2 ].vTexcoord = RVector2( fTexcoord_x1, fTexcoord_y0 );
            // Triangle 2
            // left bottom
            pVertices[ j + 3 ].vPos      = RVector3( fLeftX,  fBottomY, 0.0f );
            pVertices[ j + 3 ].vColor    = RVector3( 1.0f, 1.0f, 0.0f );
            pVertices[ j + 3 ].vTexcoord = RVector2( fTexcoord_x0, fTexcoord_y1 );
            // right top
            pVertices[ j + 4 ].vPos      = RVector3( fRightX, fTopY, 0.0f );
            pVertices[ j + 4 ].vColor    = RVector3( 1.0f, 1.0f, 0.0f );
            pVertices[ j + 4 ].vTexcoord = RVector2( fTexcoord_x1, fTexcoord_y0 );
            // right bottom
            pVertices[ j + 5 ].vPos      = RVector3( fRightX, fBottomY, 0.0f );
            pVertices[ j + 5 ].vColor    = RVector3( 1.0f, 1.0f, 0.0f );
            pVertices[ j + 5 ].vTexcoord = RVector2( fTexcoord_x1, fTexcoord_y1 );
        }

        // Update the vertex buffer
        pDevice->UpdateBuffer( m_pVertexBuffer, pVertices, nNumVertices * sizeof( UIVertex ) );

        // Set the shaders
        pDevice->SetVertexShader( m_pVertexShader );
        pDevice->SetPixelShader( m_pPixelShader );

        // Set shader stuff
        pDevice->SetPSSamplerState( m_pFontSampler );
        pDevice->SetPSTexture( 0, m_pFontTexture );
        //m_pContext->OMSetBlendState( m_pFontBlend, 0, 0xFFFFFFFF );

        // Draw text
        pDevice->SetVertexLayout( m_pInputLayout );
        pDevice->SetVertexBuffer( 0, m_pVertexBuffer, sizeof( UIVertex ) );
        pDevice->SetPrimitiveType( Riot::GFX_PRIMITIVE_TRIANGLELIST );
        pDevice->Draw( nNumVertices );

        SAFE_DELETE_ARRAY( pVertices );
    }

}