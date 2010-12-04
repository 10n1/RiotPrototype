Texture2D		DiffuseTexture : register( t0 );
SamplerState	DiffuseTextureSampler : register( s0 );

cbuffer PerFrame
{
    matrix mViewProj;
};

cbuffer PerDraw
{
    matrix mWorld;
};

struct VSOut
{
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOut DefaultVertexShader( float4 Pos : POSITION, float2 TexCoord : TEXCOORD )
{
    VSOut output;
    output.Pos = mul( Pos, mWorld );
    output.Pos = mul( output.Pos, mViewProj );
    output.TexCoord = TexCoord;
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 DefaultPixelShader( VSOut Input ) : SV_Target
{
    return DiffuseTexture.Sample(DiffuseTextureSampler, Input.TexCoord); // float4( 1.0f, 1.0f, 1.0f, 1.0f );
}
