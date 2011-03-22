/*********************************************************\
File:           PixelShader.h
Purpose:        Pixel shaders
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/21/2011 9:47:28 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _PIXELSHADER_H_
#define _PIXELSHADER_H_
#include "IRefCounted.h"
#include "types.h"

class CPixelShader : public IRefCounted
{
public:
    // CPixelShader constructor
    CPixelShader();

    // CPixelShader destructor
    ~CPixelShader();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  ApplyShader
    //  Applys the shader to the pipeline
    //-----------------------------------------------------------------------------
    virtual void ApplyShader( void ) = 0;

private:
    /***************************************\
    | class members                         |
    \***************************************/

};


#endif // #ifndef _PIXELSHADER_H_
