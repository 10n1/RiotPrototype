/*********************************************************\
File:           D3DPixelShader.h
Purpose:        
Author:         Kyle Weicht
Created:        3/20/2011
Modified:       3/21/2011 9:47:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DPIXELSHADER_H_
#define _D3DPIXELSHADER_H_
#include "PixelShader.h"

struct ID3D11PixelShader;
struct ID3D11DeviceContext;
class CD3DGraphics;

class CD3DPixelShader : public CPixelShader
{
    friend class CD3DGraphics;
public:
    // CD3DPixelShader constructor
    CD3DPixelShader();

    // CD3DPixelShader destructor
    ~CD3DPixelShader();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  ApplyShader
    //  Applys the shader to the pipeline
    //-----------------------------------------------------------------------------
    void ApplyShader( void );
private:
    /***************************************\
    | class members                         |
    \***************************************/    
    ID3D11PixelShader*     m_pShader;

    ID3D11DeviceContext*    m_pDeviceContext;
};


#endif // #ifndef _D3DPIXELSHADER_H_
