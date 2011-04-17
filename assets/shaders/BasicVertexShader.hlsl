/*********************************************************\
File:           BasicVertexShader.hlsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 3:53:58 PM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

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
    float4 Pos      : POSITION;
    float3 Normal   : NORMAL;
};

struct PS_INPUT
{
    float4 ScreenPos    : SV_POSITION;
    float4 Pos          : TEXCOORD0;
    float3 Normal       : TEXCOORD1;
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
    
    return output;
}
