/*********************************************************\
File:           UI.h
Purpose:        User interface elements...renders to a
separate target
Author:         Omar A Rodriguez
Created:        4/24/2011
\*********************************************************/
#ifndef _UI_H_
#define _UI_H_
#include "common.h"
#include "Graphics.h"
#include "GraphicsObjects.h"

namespace Riot
{
    //////////////////////////////////////////
    // UI item definition
    typedef struct _UIString
    {
        uint nLeft;
        uint nTop;
        char szText[255];
    } UIString;

    class UI
    {
        //---------------------------------------------------------------------------------
        //  Methods
    public:
        //-----------------------------------------------------------------------------
        //  Initialize
        //  Define shaders, input layout, and font texture
        //-----------------------------------------------------------------------------
        static void Initialize( IGraphicsDevice* pDevice );
        //-----------------------------------------------------------------------------
        //  AddString
        //  Add some text to the list of strings
        //-----------------------------------------------------------------------------
        static void AddString( uint nLeft, uint nTop, const char* szText );
        //-----------------------------------------------------------------------------
        //  DrawString
        //  Draw szText at (nLeft, nTop)
        //-----------------------------------------------------------------------------
        static void DrawString( IGraphicsDevice* pDevice, uint nLeft, uint nTop, const char* szText );
        //-----------------------------------------------------------------------------
        //  Draw()
        //  Draw all the strings
        //-----------------------------------------------------------------------------
        static void Draw( IGraphicsDevice* pDevice );
        //-----------------------------------------------------------------------------
        //  Destroy
        //  Release the memory allocated on Init
        //-----------------------------------------------------------------------------
        static void Destroy( void );

        //---------------------------------------------------------------------------------
        //  Members
    private:
        static float32 m_fScreenX;
        static float32 m_fScreenY;

        static IGraphicsDevice* m_pDevice;
        static IGfxVertexShader* m_pVertexShader;
        static IGfxPixelShader* m_pPixelShader;
        static IGfxVertexLayout* m_pInputLayout;
        static IGfxSamplerState* m_pFontSampler;
        static IGfxTexture2D* m_pFontTexture;
        static IGfxBlendState* m_pFontBlend;
        static IGfxBuffer* m_pVertexBuffer;

        static UIString* m_pUIStrings;
        static atomic_t m_nNumStrings;
    };

} // namespace Riot

#endif // _UI_H_