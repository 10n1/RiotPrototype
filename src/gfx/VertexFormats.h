/*********************************************************\
File:           VertexFormats.h
Purpose:        Stores the basic vertex format information
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/27/2011 3:28:25 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VERTEXFORMATS_H_
#define _VERTEXFORMATS_H_
#include "common.h"

namespace Riot
{
    class IGfxVertexLayout;

    //-----------------------------------------------------------------------------
    /*
    These are all typedef'ed and externed so they can be defined dynamicaly for
    each API (DX & OpenGL )
    */
    //////////////////////////////////////////
    //  GFX_FORMAT
    //  Synonomous to DXGI_FORMAT_*
    typedef const uint GFX_FORMAT;

    extern GFX_FORMAT GFX_FORMAT_FLOAT3;
    extern GFX_FORMAT GFX_FORMAT_FLOAT2;
    extern GFX_FORMAT GFX_FORMAT_UINT16;
    extern GFX_FORMAT GFX_FORMAT_UINT32;
    
    // These are handy for index/vertex strides, etc.
    extern const uint GFX_FORMAT_FLOAT3_SIZE;
    extern const uint GFX_FORMAT_UINT16_SIZE;
    extern const uint GFX_FORMAT_UINT32_SIZE;
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  GFX_SEMANTIC
    //  DX defines semantics statcially (the position is always POSITION), while
    //  OpenGL uses however its written in the shader
    typedef const char* GFX_SEMANTIC;

    extern GFX_SEMANTIC GFX_SEMANTIC_POSITION;
    extern GFX_SEMANTIC GFX_SEMANTIC_NORMAL;
    extern GFX_SEMANTIC GFX_SEMANTIC_TEXCOORD;
    //-----------------------------------------------------------------------------
        
    //-----------------------------------------------------------------------------
    //  GFX_PRIMITIVE_TYPE
    //  Defines the primitive type (triangle list/strip etc.)
    typedef const uint GFX_PRIMITIVE_TYPE;

    extern GFX_PRIMITIVE_TYPE GFX_PRIMITIVE_TRIANGLELIST;
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  GFX_TEXTURE_SAMPLE
    //  Defines the texture sampling type (linear, nearest, etc.)
    typedef const uint GFX_TEXTURE_SAMPLE;

    extern GFX_TEXTURE_SAMPLE GFX_TEXTURE_SAMPLE_NEAREST;
    extern GFX_TEXTURE_SAMPLE GFX_TEXTURE_SAMPLE_LINEAR;
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  GFX_FILL_MODE
    //  Defines the various fill modes
    typedef const uint GFX_FILL_MODE;

    extern GFX_FILL_MODE    GFX_FILL_SOLID;
    extern GFX_FILL_MODE    GFX_FILL_WIREFRAME;
    //-----------------------------------------------------------------------------

    
    //-----------------------------------------------------------------------------
    //  GFX_BUFFER_USAGE
    //  Defines the various buffer usage cases (CPU/GPU read/write)
    typedef const uint GFX_BUFFER_USAGE;

    extern GFX_BUFFER_USAGE    GFX_BUFFER_USAGE_DEFAULT;
    extern GFX_BUFFER_USAGE    GFX_BUFFER_USAGE_DYNAMIC;
    extern GFX_BUFFER_USAGE    GFX_BUFFER_USAGE_IMMUTABLE;
    //-----------------------------------------------------------------------------

#ifdef OS_WINDOWS
#pragma warning(push)
#pragma warning( disable:4510 ) // Default constructor cannot be implicitly created
#pragma warning( disable:4512 ) // Copy constructor cannot be implicitly created
#pragma warning( disable:4610 ) // User defined constructor required
#endif // #ifdef OS_WINDOWS
    struct InputElementLayout
    {
        GFX_SEMANTIC    szSemanticName;
        GFX_FORMAT      nFormat;
        uint            nOffset;
    };
#ifdef OS_WINDOWS
#pragma warning( pop )
#endif // #ifdef OS_WINDOWS

    /*
    Standard vertex format
    */
    struct VPosNormalTex
    {
        RVector3 Pos;
        RVector3 Normal;
        RVector2 TexCoord;

        static InputElementLayout Layout[];
        static IGfxVertexLayout*  VertexLayoutObject;
        static const uint LayoutSize;
        static const uint VertexStride;// = sizeof( VVertexPosNormal );
    };

} // namespace Riot


#endif // #ifndef _VERTEXFORMATS_H_
