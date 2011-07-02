/*********************************************************\
File:           BasicPixelShader.glsl
\*********************************************************/

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

uniform vec4 vLightPos[8];
uniform int nActiveLights;

//--------------------------------------------------------------------------------------
varying vec4 Pos;
varying vec4 Normal;

vec4 fFinalColor;

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
void main( void )
{
    fFinalColor = vec4( 0.0, 0.0, 0.0, 0.0 );

    for( int i = 0; i < nActiveLights; ++i )
    {
        vec3 vLightDir = normalize( vLightPos[i].xyz - Pos.xyz );
        fFinalColor += clamp( dot( vec4(vLightDir,1), normalize(Normal) ), 0.0, 1.0 );
    }

    gl_FragColor = fFinalColor;
}

