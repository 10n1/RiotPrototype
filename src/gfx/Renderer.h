/*********************************************************\
File:           Renderer.h
Purpose:        Abstraction between the API and the engine
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       4/27/2011 10:23:27 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "common.h"
#include "IListener.h"
#include "Graphics.h"
#include "Message.h"

namespace Riot
{
    class CWindow;
    class CMesh;
    class CView;

    class CRenderer : public IListener
    {
        friend class Engine;
    public:
        // CRenderer constructor
        CRenderer();

        // CRenderer destructor
        ~CRenderer();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  Initialize
        //-----------------------------------------------------------------------------
        void Initialize( void );

        //-----------------------------------------------------------------------------
        //  Shutdown
        //-----------------------------------------------------------------------------
        void Shutdown( void );
        
        //-----------------------------------------------------------------------------
        //  CreateGraphicsDevice
        //  Creates the device
        //-----------------------------------------------------------------------------
        void CreateGraphicsDevice( CWindow* pWindow );
        
        //-----------------------------------------------------------------------------
        //  CreateDefaultObjects
        //  Creates the default objects
        //-----------------------------------------------------------------------------
        void CreateDefaultObjects( void );

        //-----------------------------------------------------------------------------
        //  Render
        //  Performs rendering
        //-----------------------------------------------------------------------------
        void Render( void );

        //-----------------------------------------------------------------------------
        //  ProcessMessage
        //  Processes the input message
        //-----------------------------------------------------------------------------
        void ProcessMessage( const TMessage& msg );        

        //-----------------------------------------------------------------------------
        //  CreateMesh
        //  Creates a mesh from the data
        //  No parameters indicates default mesh
        //-----------------------------------------------------------------------------
        CMesh* CreateMesh(  uint nVertexStride, 
                            uint nVertexCount, 
                            uint nIndexSize, 
                            uint nIndexCount, 
                            void* pVertices, 
                            void* pIndices,
                            GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT );
        CMesh* CreateMesh( void ); 
        CMesh* LoadMesh( const char* szFilename );
        CMesh* CreateDynamicBox( void );

        //-----------------------------------------------------------------------------
        //  LoadTextureXD
        //  Loads a texture
        //-----------------------------------------------------------------------------
        IGfxTexture2D* LoadTexture2D( const char* szFilename );

        //-----------------------------------------------------------------------------
        //  SetViewProj
        //  Sets the view projection constant buffer
        //-----------------------------------------------------------------------------
        void SetViewProj( const RMatrix4& pView, const RMatrix4& pProj );

        //-----------------------------------------------------------------------------
        //  SetWorldMatrix
        //  Applies the world matrix to the pipeline
        //-----------------------------------------------------------------------------
        void SetWorldMatrix( const RMatrix4& pMatrix );

        //-----------------------------------------------------------------------------
        //  SetCurrentView
        //  Sets the current camera view
        //-----------------------------------------------------------------------------
        void SetCurrentView( CView* pView );

        //-----------------------------------------------------------------------------
        //  AddCommand
        //  Adds a renderable object to the command buffer
        //-----------------------------------------------------------------------------
        void AddCommand( const TRenderCommand& cmd, RTransform& transform );
        
        //-----------------------------------------------------------------------------
        //  SetLight
        //  Sets the specific light
        //-----------------------------------------------------------------------------
        void SetLight( const RVector3& vDir, uint nIndex );
        
        //-----------------------------------------------------------------------------
        //  GetDefaultMeshData
        //  Returns the default mesh data
        //-----------------------------------------------------------------------------
        VPosNormalTex* GetDefaultMeshData( void );
        
        //-----------------------------------------------------------------------------
        //  DrawDebugSphere
        //  Renders a wireframe debug sphere
        //-----------------------------------------------------------------------------
        void DrawDebugSphere( const RVector4& fSphere );
        
        //-----------------------------------------------------------------------------
        //  DrawDebugBox
        //  Renders a wireframe debug AAB
        //-----------------------------------------------------------------------------
        void DrawDebugBox( const RVector3& vMin,const RVector3& vMax, const RVector3& vColor );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        RTransform      m_pTransforms[MAX_RENDER_COMMANDS];
        TRenderCommand  m_pRenderCommands[MAX_RENDER_COMMANDS];

        IGraphicsDevice*    m_pDevice;

        IGfxBuffer* m_pViewProjCB;
        IGfxBuffer* m_pWorldCB;
        IGfxBuffer* m_pLightCB;

        IGfxVertexShader*   m_pDefaultVShader;
        IGfxVertexLayout*   m_pDefaultVLayout;
        IGfxPixelShader*    m_pDefaultPShader;
        CMesh*              m_pDefaultMesh;
        IGfxTexture2D*      m_pDefaultTexture;
        IGfxTexture2D*      m_pWhiteTexture;
        IGfxSamplerState*   m_pLinearSamplerState;
        IGfxSamplerState*   m_pNearestSamplerState;

        IGfxVertexLayout*   m_pWireframeVLayout;
        IGfxVertexShader*   m_pWireframeVShader;
        IGfxPixelShader*    m_pWireframePShader;

        CView*  m_pCurrentView;

        RVector4    m_vLights[MAX_LIGHTS];
        uint32      m_nNumActiveLights;
        uint32      _padding[3];
        atomic_t    m_nNumCommands;
        bool        m_bUpdateLighting;

        CMesh*      m_pSphereMesh;

        RVector4    m_DebugSpheres[1024];
        atomic_t    m_nNumSpheres;

        RVector3    m_DebugBoxesMin[1024*32];
        RVector3    m_DebugBoxesMax[1024*32];
        RVector3    m_DebugBoxesColor[1024*32];
        atomic_t    m_nNumBoxes;

        CMesh*      m_pDebugBox;
    };


} // namespace Riot

#endif // #ifndef _RENDERER_H_
