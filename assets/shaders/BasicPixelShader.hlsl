/*********************************************************\
File:           BasicPixelShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/23/2011 12:44:35 PM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D       diffuseTexture : register( t0 );
SamplerState    linearSampler : register( s0 );

cbuffer Lights : register( b0 )
{
    float4 vLight[8];
    float4 vColor[8];
    int    nLightType;
    int    nActiveLights;
    int    __padding[2];
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
    int nBitToTest = 1;

    float4 finalColor = 0.0f;

    float4 fTexColor = diffuseTexture.Sample( linearSampler, input.TexCoords );

    finalColor += fTexColor * fAmbientLightColor * fAmbientLightIntensity;
    fTexColor = fTexColor * fDiffuseLightIntensity;

    for( int i = 0; i < nActiveLights; ++i )
    {
        if( nLightType & nBitToTest == 0 )
        {
            float3 vLightDir = vLight[i];
            finalColor += fTexColor * vColor[i] * saturate( dot( vLightDir, normalize(input.Normal) ) );
        }
        else
        {
            float3 vLightDir = vLight[i];
        
            float fDistance = distance( vLightDir, input.Pos );
        
            vLightDir = normalize( vLightDir - input.Pos );
        
            if( fDistance > vLight[i].w )
                continue;
        
            finalColor += fTexColor * vColor[i] * saturate( dot( vLightDir, normalize(input.Normal) ) ) * ( 1.0f - (fDistance/vLight[i].w) );
        }

        nBitToTest = nBitToTest << 1;
    }

    return finalColor;
}
