//--------------------------------------------------------------------------------------
// File: UIPixelShader.glsl
//--------------------------------------------------------------------------------------

// vec4 Color;
// vec4 Texcoord;

void main()
{
    //gl_FragColor = gl_Color * texture2D( FontSampler, gl_TexCoord[0].xy );
    gl_FragColor = vec4( 1, 1, 0, 1 );
}

