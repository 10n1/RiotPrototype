/*********************************************************\
File:           BasicPixelShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 3:53:58 PM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer Lights : register( b0 )
{
    float4 vLightPos[8];
    int    nActiveLights;
}


//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float4 Pos          : TEXCOORD0;
    float3 Normal       : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( PS_INPUT input ) : SV_Target
{
    float4 finalColor = 0.0f;

    for( int i = 0; i < nActiveLights; ++i )
    {
        float3 vLightDir = normalize( vLightPos[i] - input.Pos );
        finalColor += saturate( dot( vLightDir, normalize(input.Normal) ) );
    }

    return finalColor;
}
