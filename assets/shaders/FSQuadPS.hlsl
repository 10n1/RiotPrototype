/*********************************************************\
File:           BasicPixelShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/22/2011 11:51:11 AM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D       diffuseTexture : register( t0 );
SamplerState    linearSampler : register( s0 );

//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float2 TexCoords    : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( PS_INPUT input ) : SV_Target
{
    float4 fTexColor = diffuseTexture.Sample( linearSampler, input.TexCoords );

    return fTexColor;
}
