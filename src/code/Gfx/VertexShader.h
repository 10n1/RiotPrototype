/*********************************************************\
File:           VertexShader.h
Purpose:        Vertex shaders
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/21/2011 9:47:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VERTEXSHADER_H_
#define _VERTEXSHADER_H_
#include "IRefCounted.h"
#include "types.h"

class CVertexShader : public IRefCounted
{
public:
    // CVertexShader constructor
    CVertexShader();

    // CVertexShader destructor
    ~CVertexShader();
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


#endif // #ifndef _VERTEXSHADER_H_
