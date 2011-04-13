/*********************************************************\
File:           OGLGraphics.h
Purpose:        OpenGL graphics interface
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/12/2011 9:41:48 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OGLGRAPHICS_H_
#define _OGLGRAPHICS_H_
#include "common.h"
#include "Graphics.h"
#include "SystemOpenGL.h"
#include "GraphicsObjects.h"

namespace Riot
{

    class COGLDevice : public CGraphicsDevice
    {
        friend class System;
    public:
        // COGLDevice constructor
        COGLDevice();

        // COGLDevice destructor
        ~COGLDevice();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Initialize
        //  Performs any API specific initialization tasks (wind order, etc)
        //-----------------------------------------------------------------------------
        Result Initialize( CWindow* pWindow );

        //-----------------------------------------------------------------------------
        //  Resize
        //  Resizes the graphics device (along with the back buffer )
        //-----------------------------------------------------------------------------
        void Resize( uint nWidth, uint nHeight );

        //-----------------------------------------------------------------------------
        //  Hardware methods
        //----------------------------------------------------------------------------- 
        void SetDefaultRenderDepthTarget( void );
       
        //
        void SetViewport( uint nWidth, uint nHeight );
        void SetClearColor( float fRed, float fGreen, float fBlue, float fAlpha );
        void SetClearDepth( float fDepth );
        void Clear( void );
        void Present( void );
        //

        //
        IGfxBuffer* CreateConstantBuffer( uint nSize, void* pInitialData = NULL );
        //

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        SystemOpenGL::TOpenGLDevice m_pDevice;
    };

} // namespace Riot

#endif // #ifndef _OGLGRAPHICS_H_
