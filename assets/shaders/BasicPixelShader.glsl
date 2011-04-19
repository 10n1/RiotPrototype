/*********************************************************\
File:           BasicPixelShader.glsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 11:47:24 PM
Modified by:    Kyle Weicht
\*********************************************************/
#version 330

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

uniform vec4 vLightPos[8];
uniform int nActiveLights;

//--------------------------------------------------------------------------------------
in vec4 ScreenPos;
in vec4 Pos;
in vec3 Normal;

out vec4 fFinalColor;

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
void main( void )
{
    //float4 finalColor = 0.0f;
    //
    //for( int i = 0; i < nActiveLights; ++i )
    //{
    //    float3 vLightDir = normalize( vLightPos[i] - input.Pos );
    //    finalColor += saturate( dot( vLightDir, normalize(input.Normal) ) );
    //}
    //
    //return finalColor;

    
    fFinalColor = vec4( 0.0f, 0.0f, 0.0f, 0.0f );

    for( int i = 0; i < nActiveLights; ++i )
    {
        vec3 vLightDir = normalize( vLightPos[i].xyz - Pos.xyz );
        //fFinalColor += saturate( dot( vLightDir, normalize(Normal) ) );
        fFinalColor += clamp( dot( vLightDir, normalize(Normal) ), 0.0f, 1.0f );
    }
}
