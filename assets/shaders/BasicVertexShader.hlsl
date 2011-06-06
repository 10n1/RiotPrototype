/*********************************************************\
File:           BasicVertexShader
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       5/4/2011 11:14:58 AM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer cbViewProj : register( b0 )
{
	matrix mViewProj;
};

cbuffer cbWorld : register( b1 )
{
	matrix mWorld;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos          : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoords    : TEXCOORD0;
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
    output.Pos       = mul( input.Pos, mWorld );
    output.ScreenPos = mul( output.Pos, mViewProj );
    output.Normal    = mul( input.Normal, mWorld);
    output.TexCoords = input.TexCoords;
    
    return output;
}
