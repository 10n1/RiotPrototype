/*********************************************************\
File:           VertexShader.h
Purpose:        Vertex shaders
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 10:34:22 PM
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
    //  SetShader
    //  Applys the shader to the pipeline
    //-----------------------------------------------------------------------------
    virtual void SetShader( void ) = 0;

private:
    /***************************************\
    | class members                         |
    \***************************************/

};


#endif // #ifndef _VERTEXSHADER_H_
