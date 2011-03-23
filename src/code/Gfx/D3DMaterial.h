/*********************************************************\
File:           D3DMaterial.h
Purpose:        
Author:         Kyle Weicht
Created:        3/22/2011
Modified:       3/22/2011 6:50:27 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DMATERIAL_H_
#define _D3DMATERIAL_H_
#include "Material.h"
#include "types.h"
#include "memory.h"

struct ID3D11PixelShader;
struct ID3D11DeviceContext;
class CD3DGraphics;

class CD3DMaterial : public CMaterial
{
    friend class CD3DGraphics;
public:
    // CD3DMaterial constructor
    CD3DMaterial();

    // CD3DMaterial destructor
    ~CD3DMaterial();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  ApplyMaterial
    //  Applies the material to the pipeline
    //-----------------------------------------------------------------------------
    void ApplyMaterial( void );
private:
    /***************************************\
    | class members                         |
    \***************************************/
    ID3D11PixelShader*      m_pPixelShader;

    ID3D11DeviceContext*    m_pDeviceContext;
};


#endif // #ifndef _D3DMATERIAL_H_
