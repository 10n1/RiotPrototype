/*********************************************************\
File:           Graphics.h
Purpose:        Base interface for the graphics hardware/API
                abstraction
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/3/2011 9:13:00 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_
#include "Common.h"
#include "Common.h"
#include "IRefCounted.h"
#include "Types.h"
#include "Scene\Component.h"

#include <Windows.h>
#include <xnamath.h>
#include "RiotMath.h"

#define MAX_COMMANDS (16*1024)

class CWindow;
class CObject;
class CMesh;
class CMaterial;
class CView;
class CRenderObject;

class CGraphics : public IRefCounted
{
public:
    // CGraphics constructor
    CGraphics();

    // CGraphics destructor
    virtual ~CGraphics();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Initialize
    //  Creates the device, then creates any other needed buffers, etc.
    //-----------------------------------------------------------------------------
    virtual uint Initialize( CWindow* pWindow ) = 0;

    //-----------------------------------------------------------------------------
    //  CreateDevice
    //  Creates the device, reading info from the window
    //-----------------------------------------------------------------------------
    virtual uint CreateDevice( CWindow* pWindow ) = 0;
    
    //-----------------------------------------------------------------------------
    //  ReleaseBuffers
    //  Releases all buffers to prepare for a resize
    //-----------------------------------------------------------------------------
    virtual void ReleaseBuffers( void ) = 0;

    //-----------------------------------------------------------------------------
    //  CreateBuffers
    //  Creates all buffers required for rendering
    //-----------------------------------------------------------------------------
    virtual void CreateBuffers( uint nWidth, uint nHeight ) = 0;

    //-----------------------------------------------------------------------------
    //  Resize
    //  Resizes the device
    //-----------------------------------------------------------------------------
    virtual void Resize( uint nWidth, uint nHeight );
    
    
    //-----------------------------------------------------------------------------
    //  PrepareRender
    //  Clears the screen to prepare for rendering
    //-----------------------------------------------------------------------------
    virtual void PrepareRender( void ) = 0;
    
    //-----------------------------------------------------------------------------
    //  Render
    //  Renders everything
    //-----------------------------------------------------------------------------
    virtual void Render( void );
    // TODO: Passing in the objects doesn't feel right...
    
    //-----------------------------------------------------------------------------
    //  Present
    //  Presents the frame
    //-----------------------------------------------------------------------------
    virtual void Present( void ) = 0;
    
    //-----------------------------------------------------------------------------
    //  SetViewProj
    //  Sets the view projection constant buffer
    //-----------------------------------------------------------------------------
    virtual void SetViewProj( const void* pView, const void* pProj ) = 0;
    
    //-----------------------------------------------------------------------------
    //  SetViewProj
    //  Sets the view projection constant buffer
    //-----------------------------------------------------------------------------
    virtual void SetCurrentView( const CView* pView );
    
    //-----------------------------------------------------------------------------
    //  SetLight
    //  Sets the specific light
    //-----------------------------------------------------------------------------
    void SetLight( const XMVECTOR& vDir, uint nIndex );
    
    //-----------------------------------------------------------------------------
    //  AddCommand
    //  Adds a renderable object to the command buffer
    //-----------------------------------------------------------------------------
    void AddCommand( CRenderObject* pObject );

    //-----------------------------------------------------------------------------
    //  AddMatrix
    //  Adds a matrix to the list of world matrices
    //-----------------------------------------------------------------------------
    void AddMatrix( XMMATRIX* pMatrix );
    
    //-----------------------------------------------------------------------------
    //  SetWorldMatrix
    //  Applies the world matrix to the pipeline
    //-----------------------------------------------------------------------------
    virtual void SetWorldMatrix( XMMATRIX* pMatrix ) = 0;

public:
    /***************************************\
    | object creation                       |
    \***************************************/

    // TODO: A list of all the vertex formats somewhere, matched to the appropriate
    //       vertex shader?
    
    //-----------------------------------------------------------------------------
    //  CreateMesh
    //  Creates a mesh from the file
    //-----------------------------------------------------------------------------
    virtual CMesh* CreateMesh( const wchar_t* szFilename ) = 0;
    
    //-----------------------------------------------------------------------------
    //  CreateMaterial
    //  Creates a material from a shader file
    //-----------------------------------------------------------------------------
    virtual CMaterial* CreateMaterial( const wchar_t* szFilename, const char* szEntryPoint, const char* szProfile ) = 0;

protected:
    /***************************************\
    | class members                         |
    \***************************************/
    CRenderObject*  m_ppRenderCommands[MAX_COMMANDS];
    XMMATRIX*       m_ppMatrices[MAX_OBJECTS];
    XMVECTOR        m_vLights[MAX_LIGHTS];
    int32           m_nActiveLights;
    int32           _padding[3];

    CWindow*        m_pWindow;
    CView*          m_pCurrView;

    CMesh*          m_pBoxMesh;


    uint            m_nNumCommands;
    uint            m_nNumMatrices;
    bool            m_bUpdateLighting;
};


#endif // #ifndef _GRAPHICS_H_
