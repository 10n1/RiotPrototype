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

cbuffer cbWorld : register( b1 )
{
	matrix mWorld;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos          : POSITION;
    float4 Color        : COLOR;
};

struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float4 Color        : COLOR0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;

    float4 Pos          = mul( input.Pos, mWorld );
    output.ScreenPos    = mul( Pos, mViewProj );

    output.Color        = input.Color;
    
    return output;
}

*/

uniform mat4 mWorld;
uniform mat4 mViewProj;

vec4 vPos;

void main()
{
    vec4 mWorldPos = vPos * mWorld;
    gl_Position = mWorldPos * mViewProj;
}
