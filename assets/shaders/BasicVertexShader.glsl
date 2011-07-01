/*********************************************************\
File:           BasicVertexShader.glsl
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/17/2011 10:58:19 PM
Modified by:    Kyle Weicht
\*********************************************************/
//#version 330
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
uniform mat4 mViewProj;
uniform mat4 mWorld;

//--------------------------------------------------------------------------------------
//in vec4 vPosition;
attribute vec4 vPosition;
//in vec3 vNormal;
attribute vec3 vNormal;

//out vec4 ScreenPos;
//varying vec4 ScreenPos;
//out vec4 Pos;
varying vec4 Pos;
//out vec3 Normal;
varying vec4 Normal;

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
void main( void )
{
    //PS_INPUT output = (PS_INPUT)0;
    //output.Pos       = mul( input.Pos, mWorld );
    //output.ScreenPos = mul( output.Pos, mViewProj );
    //output.Normal    = mul( input.Normal, mWorld);
    
    
    Pos       = vPosition * mWorld;
    //ScreenPos = Pos * mViewProj;
    Normal    = vec4( vNormal, 1 ) * mWorld;

    //gl_Normal = Normal;
    gl_Position = Pos * mViewProj; //ScreenPos;
}
