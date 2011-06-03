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
#include "IListener.h"

namespace Riot
{
    //////////////////////////////////////////
    // UI item definition
    typedef void (ButtonFunc)(void);
    
    typedef struct _UIString
    {
        uint nLeft;
        uint nTop;
        char szText[255];
    } UIString;
    
    typedef struct _UIButton
    {
        char        szText[256];
        ButtonFunc* pFunc;
        uint        nLeft;
        uint        nRight;
        uint        nTop;
        uint        nBottom;
    } UIButton;

    class UI : public IListener
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
        //-----------------------------------------------------------------------------
        //  AddButton
        //  Adds a button to render, returning the id
        //-----------------------------------------------------------------------------
        static uint AddButton( uint nLeft, uint nRight, uint nTop, uint nBottom, const char* szText, ButtonFunc* pFunc );
        //-----------------------------------------------------------------------------
        //  RemoveButton
        //  Removes a button
        //-----------------------------------------------------------------------------
        static void RemoveButton( uint nIndex );
        
        //-----------------------------------------------------------------------------
        //  SwapBuffers
        //  Swaps last and previous frames buffers
        //-----------------------------------------------------------------------------
        static void SwapBuffers( void );
        
        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );        

        //---------------------------------------------------------------------------------
        //  Members
    private:
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;
        
        enum 
        { 
            MAX_BUTTONS = 128,
            MAX_STRINGS = 128,
        };
        
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
        
        static UIButton m_pButtons[MAX_BUTTONS];
        static atomic_t m_nNumButtons;

        static UIString* m_pUIStrings[2];
        static UIString* m_pCurrStrings;
        static UIString* m_pPrevStrings;
        static atomic_t m_nNumStrings;
        static uint     m_nNumPrevStrings;
    };

} // namespace Riot

#endif // _UI_H_