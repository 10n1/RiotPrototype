/*********************************************************\
File:           BasicPixelShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/21/2011 5:26:03 PM
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

    fTexColor.r = 1.0f;

    return fTexColor;
}
