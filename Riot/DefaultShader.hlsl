cbuffer PerFrame
{
    matrix mWorldViewProj;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
float4 DefaultVertexShader( float4 Pos : POSITION ) : SV_POSITION
{
    return mul( Pos, mWorldViewProj );
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 DefaultPixelShader( float4 Pos : SV_POSITION ) : SV_Target
{
    return float4( 1.0f, 1.0f, 1.0f, 1.0f );
}
