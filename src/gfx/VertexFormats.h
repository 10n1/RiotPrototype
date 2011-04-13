/*********************************************************\
File:           VertexFormats.h
Purpose:        
Author:         Kyle Weicht
Created:        4/12/2011
Modified:       4/12/2011 11:32:19 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VERTEXFORMATS_H_
#define _VERTEXFORMATS_H_
#include "common.h"

#include "D3DGraphics.h"

namespace Riot
{
    enum GFXFORMAT
    {
        FLOAT3 = DXGI_FORMAT_R32G32B32_FLOAT,
    };

    struct InputElementLayout
    {
        char*       szSemanticName;
        GFXFORMAT   nFormat;
        uint        nOffset;
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
    InputElementLayout VVertexPosNormal::Layout[2] = 
    { 
        { "POSITION", FLOAT3, 0 },
        { "NORMAL", FLOAT3, 12 },
    };

} // namespace Riot


#endif // #ifndef _VERTEXFORMATS_H_
