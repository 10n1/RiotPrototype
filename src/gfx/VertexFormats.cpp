/*********************************************************\
File:           VertexFormats.cpp
Author:         Kyle Weicht
Created:        4/14/2011
Modified:       4/20/2011 9:01:23 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "VertexFormats.h"

namespace Riot
{
    
    /*
    Standard vertex format
    */
    InputElementLayout VPosNormalTex::Layout[] = 
    { 
        { GFX_SEMANTIC_POSITION, GFX_FORMAT_FLOAT3, 0 },
        { GFX_SEMANTIC_NORMAL, GFX_FORMAT_FLOAT3, 12 },
        { GFX_SEMANTIC_TEXCOORD, GFX_FORMAT_FLOAT2, 24 },
    };
    IGfxVertexLayout*  VPosNormalTex::VertexLayoutObject = NULL;
    const uint VPosNormalTex::LayoutSize = ARRAY_LENGTH(Layout);
    const uint VPosNormalTex::VertexStride = sizeof( VPosNormalTex );

} // namespace Riot
