/*********************************************************\
File:           FSQuadPS
\*********************************************************/

/*

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
//Texture2D       diffuseTexture : register( t0 );
//SamplerState    linearSampler : register( s0 );

//--------------------------------------------------------------------------------------
struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float2 TexCoords    : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( PS_INPUT input ) : SV_Target
{
    float4 fTexColor = diffuseTexture.Sample( linearSampler, input.TexCoords );
    return fTexColor;
}

*/

void main()
{
    gl_FragColor = vec4( 1, 1, 0, 1 );
}
