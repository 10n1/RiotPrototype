/*********************************************************\
File:           BasicVertexShader.glsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 10:58:19 PM
Modified by:    Kyle Weicht
\*********************************************************/
#version 330
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
uniform mat4 mViewProj;
uniform mat4 mWorld;

//--------------------------------------------------------------------------------------
in vec4 vPosition;
in vec3 vNormal;

out vec4 ScreenPos;
out vec4 Pos;
out vec3 Normal;

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void main( void )
{
    //PS_INPUT output = (PS_INPUT)0;
    //output.Pos       = mul( input.Pos, mWorld );
    //output.ScreenPos = mul( output.Pos, mViewProj );
    //output.Normal    = mul( input.Normal, mWorld);
    
    
    Pos       = input.Pos * mWorld;
    ScreenPos = output.Pos * mViewProj;
    Normal    = input.Normal * mWorld;

    gl_Position = ScreenPos;
}
