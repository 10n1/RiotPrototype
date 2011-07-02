/*********************************************************\
File:           BasicPixelShader
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/27/2011 9:14:05 PM
Modified by:    Kyle Weicht
\*********************************************************/
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
varying vec4 ScreenPos;
varying vec4 Color;

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
void main()
{
    gl_FragColor = gl_Color;
}