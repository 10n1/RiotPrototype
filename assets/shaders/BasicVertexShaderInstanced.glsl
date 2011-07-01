/*********************************************************\
File:           BasicVertexShader
\*********************************************************/

/*

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

*/

uniform mat4 mWorld;
uniform mat4 mViewProj;

attribute vec4 vPos;

void main()
{
    vec4 vWorldPos = vPos * mWorld;
    gl_Position = vWorldPos * mViewProj;
}
