//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ViewProj : register( b0 )
{
	matrix ViewProj;
}

cbuffer World : register( b1 )
{
	matrix World;
}

cbuffer Lights : register( b0 )
{
    float4 vLightPos[8];
    int    nActiveLights;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 ScreenPos : SV_POSITION;
    float4 Pos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.ScreenPos = output.Pos;
    output.ScreenPos = mul( output.Pos, ViewProj );
    output.Normal = mul( input.Normal, World);
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    float4 finalColor = 0.0f;
    for( int i = 0; i < nActiveLights; ++i )
    {
        //float3 vLightDir = normalize(input.Pos - vLightPos[i]);
        float3 vLightDir = normalize( vLightPos[i] - input.Pos );
        finalColor += saturate( dot( vLightDir, normalize(input.Normal) ) );
    }

    return finalColor;
}
