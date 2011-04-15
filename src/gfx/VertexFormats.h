/*********************************************************\
File:           VertexFormats.h
Purpose:        Stores the basic vertex format information
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/14/2011 8:00:13 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VERTEXFORMATS_H_
#define _VERTEXFORMATS_H_
#include "common.h"

#include "D3DGraphics.h"

namespace Riot
{

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
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    //  GFX_SEMANTIC
    //  DX defines semantics statcially (the position is always POSITION), while
    //  OpenGL uses however its written in the shader
    typedef const char* GFX_SEMANTIC;

    extern GFX_SEMANTIC GFX_SEMANTIC_POSITION;
    extern GFX_SEMANTIC GFX_SEMANTIC_NORMAL;
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
    struct VVertexPosNormal
    {
        RVector3 Pos;
        RVector3 Normal;

        static InputElementLayout Layout[];
    };

} // namespace Riot


#endif // #ifndef _VERTEXFORMATS_H_
