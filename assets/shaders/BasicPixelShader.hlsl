/*********************************************************\
File:           BasicPixelShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/20/2011 8:52:44 AM
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
    const float4 fAmbientLightColor = float4( 1.0f, 1.0f, 1.0f, 1.0f );
    const float  fAmbientLightIntensity = 0.1f;
    const float  fDiffuseLightIntensity = 0.9f;


    float4 finalColor = 0.0f;

    float4 fTexColor = diffuseTexture.Sample( linearSampler, input.TexCoords );

    finalColor += fTexColor * fAmbientLightColor * fAmbientLightIntensity;
    fTexColor = fTexColor * fDiffuseLightIntensity;

    for( int i = 0; i < nActiveDirLights; ++i )
    {
        float3 vLightDir = vDirLightDir[i];
        finalColor += fTexColor * saturate( dot( vLightDir, normalize(input.Normal) ) );
    }

    for( int i = 0; i < nActivePointLights; ++i )
    {
        float3 vLightDir = vPointLightPos[i];

        float fDistance = distance( vLightDir, input.Pos );

        vLightDir = normalize( vLightDir - input.Pos );

        if( fDistance > vPointLightPos[i].w )
            continue;

        finalColor += fTexColor * saturate( dot( vLightDir, normalize(input.Normal) ) ) * ( 1.0f - (fDistance/vPointLightPos[i].w) );
    }

    return finalColor;
}
