/*********************************************************\
File:           BasicPixelShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/19/2011 8:32:09 PM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D       diffuseTexture : register( t0 );
SamplerState    linearSampler : register( s0 );

cbuffer Lights : register( b0 )
{
    float4 vDirLightDir[8];
    float4 vPointLightPos[8];
    int    nActivePointLights;
    int    nActiveDirLights;
}


//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float4 Pos          : TEXCOORD0;
    float3 Normal       : TEXCOORD1;
    float2 TexCoords    : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( PS_INPUT input ) : SV_Target
{
    float4 finalColor = 0.0f;

    float4 fTexColor = diffuseTexture.Sample( linearSampler, input.TexCoords );

    for( int i = 0; i < nActivePointLights; ++i )
    {
        float3 vLightDir = normalize( vPointLightPos[i] - input.Pos );
        finalColor += fTexColor * saturate( dot( vLightDir, normalize(input.Normal) ) );
    }

    return finalColor;
}
