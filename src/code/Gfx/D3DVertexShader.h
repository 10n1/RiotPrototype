/*********************************************************\
File:           D3DVertexShader.h
Purpose:        
Author:         Kyle Weicht
Created:        3/20/2011
Modified:       3/20/2011 12:36:10 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _D3DVERTEXSHADER_H_
#define _D3DVERTEXSHADER_H_
#include "VertexShader.h"

struct ID3D11VertexShader;
struct ID3D11DeviceContext;
class CD3DGraphics;

class CD3DVertexShader : public CVertexShader
{
    friend class CD3DGraphics;
public:
    // CD3DVertexShader constructor
    CD3DVertexShader();

    // CD3DVertexShader destructor
    ~CD3DVertexShader();
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  SetShader
    //  Applys the shader to the pipeline
    //-----------------------------------------------------------------------------
    void SetShader( void );
private:
    /***************************************\
    | class members                         |
    \***************************************/    
    ID3D11VertexShader*     m_pShader;

    ID3D11DeviceContext*    m_pDeviceContext;
};


#endif // #ifndef _D3DVERTEXSHADER_H_
