/*********************************************************\
File:           BasicVertexShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/21/2011 2:00:40 PM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbViewProj : register( b0 )
{
	matrix mViewProj;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos          : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoords    : TEXCOORD0;
    float4x4    mWorld  : Transform;
};

struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float4 Pos          : TEXCOORD0;
    float3 Normal       : TEXCOORD1;
    float2 TexCoords    : TEXCOORD2;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos       = mul( input.Pos, input.mWorld );
    output.ScreenPos = mul( output.Pos, mViewProj );
    output.Normal    = mul( input.Normal, input.mWorld);
    output.TexCoords = input.TexCoords;
    
    return output;
}
