/*********************************************************\
File:           GraphicsObjects.h
Purpose:        Interfaces for all the graphics objects in the engine
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       5/21/2011 3:40:05 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICSOBJECTS_H_
#define _GRAPHICSOBJECTS_H_
#include "common.h"
#include "IRefCounted.h"

// All interfaces are method-less
#define DEFINE_BASE_GRAPHICS_INTERFACE( Name )    \
class IGfx##Name : public IRefCounted  \
    {                               \
    public:                         \
        virtual ~IGfx##Name() { }      \
    }

namespace Riot
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    DEFINE_BASE_GRAPHICS_INTERFACE( RenderTarget );
    DEFINE_BASE_GRAPHICS_INTERFACE( Buffer );
    DEFINE_BASE_GRAPHICS_INTERFACE( VertexLayout );
    DEFINE_BASE_GRAPHICS_INTERFACE( VertexShader );
    DEFINE_BASE_GRAPHICS_INTERFACE( PixelShader );
    DEFINE_BASE_GRAPHICS_INTERFACE( Texture2D );
    DEFINE_BASE_GRAPHICS_INTERFACE( SamplerState );
    DEFINE_BASE_GRAPHICS_INTERFACE( BlendState );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
    
    
    enum RenderMaterial
    {

    };

    class CMaterial : public IRefCounted
    {
        friend class CRenderer;
    public:
        // CMaterial constructor
        CMaterial();

        // CMaterial destructor
        ~CMaterial();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  ApplyMaterial
        //  Applies the material to the pipeline
        //-----------------------------------------------------------------------------
        void ApplyMaterial( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        IGfxPixelShader*    m_pShader;
    };

} // namespace Riot

#undef CREATE_GRAPHICS_INTERFACE


#endif // #ifndef _GRAPHICSOBJECTS_H_
