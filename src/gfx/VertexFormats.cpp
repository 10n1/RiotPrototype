/*********************************************************\
File:           VertexFormats.cpp
Author:         Kyle Weicht
Created:        4/14/2011
Modified:       5/21/2011 2:16:52 PM
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
        { GFX_SEMANTIC_POSITION, 0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_NORMAL,   0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_TEXCOORD, 0, GFX_FORMAT_FLOAT2, 0, GFX_INPUT_DATA_PER_VERTEX },
    };
    IGfxVertexLayout*   VPosNormalTex::VertexLayoutObject = NULL;
    const uint          VPosNormalTex::LayoutSize = ARRAY_LENGTH(Layout);
    const uint          VPosNormalTex::VertexStride = sizeof( VPosNormalTex );
    
    /*
    Screen space vertex format
    */
    InputElementLayout VPosColorTex::Layout[] = 
    { 
        { GFX_SEMANTIC_POSITION, 0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_COLOR,    0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_TEXCOORD, 0, GFX_FORMAT_FLOAT2, 0, GFX_INPUT_DATA_PER_VERTEX },
    };
    IGfxVertexLayout*   VPosColorTex::VertexLayoutObject = NULL;
    const uint          VPosColorTex::LayoutSize = ARRAY_LENGTH(Layout);
    const uint          VPosColorTex::VertexStride = sizeof( VPosColorTex );

    /*
    Position and color format
    */        
    InputElementLayout VPosColor::Layout[] = 
    { 
        { GFX_SEMANTIC_POSITION, 0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_COLOR,    0, GFX_FORMAT_FLOAT4, 0, GFX_INPUT_DATA_PER_VERTEX },
    };
    IGfxVertexLayout*   VPosColor::VertexLayoutObject = NULL;
    const uint          VPosColor::LayoutSize = ARRAY_LENGTH(Layout);
    const uint          VPosColor::VertexStride = sizeof( VPosColor );

    
    /*
    Standard vertex format
    */
    InputElementLayout VPosNormalTexInst::Layout[] = 
    { 
        { GFX_SEMANTIC_POSITION,  0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_NORMAL,    0, GFX_FORMAT_FLOAT3, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_TEXCOORD,  0, GFX_FORMAT_FLOAT2, 0, GFX_INPUT_DATA_PER_VERTEX },
        { GFX_SEMANTIC_TRANSFORM, 0, GFX_FORMAT_FLOAT4, 1, GFX_INPUT_DATA_PER_INSTANCE },
        { GFX_SEMANTIC_TRANSFORM, 1, GFX_FORMAT_FLOAT4, 1, GFX_INPUT_DATA_PER_INSTANCE },
        { GFX_SEMANTIC_TRANSFORM, 2, GFX_FORMAT_FLOAT4, 1, GFX_INPUT_DATA_PER_INSTANCE },
        { GFX_SEMANTIC_TRANSFORM, 3, GFX_FORMAT_FLOAT4, 1, GFX_INPUT_DATA_PER_INSTANCE },
    };
    IGfxVertexLayout*   VPosNormalTexInst::VertexLayoutObject = NULL;
    const uint          VPosNormalTexInst::LayoutSize = ARRAY_LENGTH(Layout);
    const uint          VPosNormalTexInst::VertexStride = sizeof( VPosNormalTex );

} // namespace Riot
