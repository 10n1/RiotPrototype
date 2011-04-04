/*********************************************************\
File:           Material.h
Purpose:        Property for holding pixel shaders/textures
                and in general describing how an object looks
Author:         Kyle Weicht
Created:        3/21/2011
Modified:       4/3/2011 8:27:06 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "Common.h"
#include "RenderObject.h"
#include "Types.h"

class CPixelShader;

class CMaterial : public CRenderObject
{
public:
    // CMaterial constructor
    CMaterial();

    // CMaterial destructor
    virtual ~CMaterial();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  ProcessObject
    //  Applies the material to the pipeline
    //-----------------------------------------------------------------------------
    virtual void ProcessObject( void ) = 0;

private:
    /***************************************\
    | class members                         |
    \***************************************/
};


#endif // #ifndef _MATERIAL_H_
