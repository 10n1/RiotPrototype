/*********************************************************\
File:           RenderCommand.h
Purpose:        
Author:         Kyle Weicht
Created:        4/4/2011
Modified:       4/4/2011 8:09:52 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RENDERCOMMAND_H_
#define _RENDERCOMMAND_H_
#include "common.h"

class CMesh;
class CMaterial;

struct CRenderCommand
{
    CMaterial*  m_pMaterial;
    CMesh*      m_pMesh;
};


#endif // #ifndef _RENDERCOMMAND_H_
