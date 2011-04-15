/*********************************************************\
File:           VertexFormats.h
Purpose:        Stores the basic vertex format information
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 10:23:12 PM
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
    //-----------------------------------------------------------------------------
        
    //-----------------------------------------------------------------------------
    //  GFX_PRIMITIVE_TYPE
    //  Defines the primitive type (triangle list/strip etc.)
    typedef const uint GFX_PRIMITIVE_TYPE;

    extern GFX_PRIMITIVE_TYPE GFX_PRIMITIVE_TRIANGLELIST;
    //-----------------------------------------------------------------------------

    struct InputElementLayout
    {
        GFX_SEMANTIC    szSemanticName;
        GFX_FORMAT      nFormat;
        uint            nOffset;
    };

    /*
    Standard vertex format
    */
    struct VPosNormal
    {
        RVector3 Pos;
        RVector3 Normal;

        static InputElementLayout Layout[];
        static IGfxVertexLayout*  VertexLayoutObject;
        static const uint LayoutSize;
        static const uint VertexStride;// = sizeof( VVertexPosNormal );
    };

} // namespace Riot


#endif // #ifndef _VERTEXFORMATS_H_
