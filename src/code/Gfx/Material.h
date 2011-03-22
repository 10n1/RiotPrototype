/*********************************************************\
File:           Material.h
Purpose:        Property for holding pixel shaders/textures
                and in general describing how an object looks
Author:         Kyle Weicht
Created:        3/21/2011
Modified:       3/21/2011 9:47:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MATERIAL_H_
#define _MATERIAL_H_
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
    //  SetPixelShader
    //  Sets the materials pixel shader
    //  TODO: Support multiple pixel shaders
    //-----------------------------------------------------------------------------
    void SetPixelShader( CPixelShader* pShader );

    //-----------------------------------------------------------------------------
    //  ApplyMaterial
    //  Applies the material to the pipeline
    //-----------------------------------------------------------------------------
    void ApplyMaterial( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    CPixelShader*   m_pPixelShader;
};


#endif // #ifndef _MATERIAL_H_
