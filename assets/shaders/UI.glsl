//--------------------------------------------------------------------------------------
// File: UI
//--------------------------------------------------------------------------------------

/*
Texture2D FontTexture : register( t0 );
SamplerState FontSampler : register( s0 );

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos      : POSITION;
    float4 Color    : COLOR;
    float2 Texcoord : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float4 Color    : COLOR;
    float2 Texcoord : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader -- Screen space
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = input.Pos;
    output.Color = input.Color;
    output.Texcoord = input.Texcoord;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    float4 texColor = FontTexture.Sample( FontSampler, input.Texcoord );
    if( texColor.a > 0.5 )
        texColor *= input.Color;
    else
        discard;

    return texColor;
}
*/

uniform sampler2D FontSampler;

vec4 Pos;
vec4 Color;
vec4 Texcoord;

void VS()
{
    //gl_Color = Color;
    //gl_Texcoord[0] = Texcoord;
    gl_Position = Pos;
}


