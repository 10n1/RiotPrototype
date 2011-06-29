/*********************************************************\
File:           FSQuadVS
\*********************************************************/

/*
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float2 Pos          : POSITION;
};

struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float2 TexCoords    : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.ScreenPos.xy = input.Pos;
    output.ScreenPos.z = 0;
    output.ScreenPos.w = 1;

    output.TexCoords = (input.Pos + 1.0f) / 2.0f;
    output.TexCoords.y *= -1;
    
    return output;
}
*/

vec4 vPos;

void main()
{
    gl_Position = vec4( vPos.x, vPos.y, 0, 1 );
}
