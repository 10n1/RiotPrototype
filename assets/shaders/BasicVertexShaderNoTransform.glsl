/*********************************************************\
File:           BasicVertexShaderNoTransform
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
    output.Pos       = input.Pos;
    output.ScreenPos = mul( output.Pos, mViewProj );
    output.Normal    = input.Normal;
    output.TexCoords = input.TexCoords;
    
    return output;
}

*/

vec4 vPos;

void main()
{
    gl_Position = vPos;
}
