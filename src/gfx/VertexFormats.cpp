/*********************************************************\
File:           VertexFormats.cpp
Author:         Kyle Weicht
Created:        4/14/2011
Modified:       4/14/2011 10:14:57 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "VertexFormats.h"

namespace Riot
{
    
    /*
    Standard vertex format
    */
    InputElementLayout VPosNormal::Layout[] = 
    { 
        { GFX_SEMANTIC_POSITION, GFX_FORMAT_FLOAT3, 0 },
        { GFX_SEMANTIC_NORMAL, GFX_FORMAT_FLOAT3, 12 },
    };
    IGfxVertexLayout*  VPosNormal::VertexLayoutObject = NULL;
    const uint VPosNormal::LayoutSize = ARRAY_LENGTH(Layout);
    const uint VPosNormal::VertexStride = sizeof( VPosNormal );

} // namespace Riot
