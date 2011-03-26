/*********************************************************\
File:           Material.h
Purpose:        Property for holding pixel shaders/textures
                and in general describing how an object looks
Author:         Kyle Weicht
Created:        3/21/2011
Modified:       3/23/2011 7:07:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "Common.h"
#include "IRefCounted.h"
#include "Types.h"

class CPixelShader;

class CMaterial : public IRefCounted
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
    //  ApplyMaterial
    //  Applies the material to the pipeline
    //-----------------------------------------------------------------------------
    virtual void ApplyMaterial( void ) = 0;

private:
    /***************************************\
    | class members                         |
    \***************************************/
};


#endif // #ifndef _MATERIAL_H_
