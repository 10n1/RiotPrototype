/*********************************************************\
File:           VertexFormats.cpp
Author:         Kyle Weicht
Created:        4/14/2011
Modified:       4/14/2011 8:00:12 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "VertexFormats.h"

namespace Riot
{
    
    /*
    Standard vertex format
    */
    InputElementLayout VVertexPosNormal::Layout[2] = 
    { 
        { GFX_SEMANTIC_POSITION, GFX_FORMAT_FLOAT3, 0 },
        { GFX_SEMANTIC_NORMAL, GFX_FORMAT_FLOAT3, 12 },
    };

} // namespace Riot
