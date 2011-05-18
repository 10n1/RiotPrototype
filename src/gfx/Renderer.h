/*********************************************************\
File:           Renderer.h
Purpose:        Abstraction between the API and the engine
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       5/17/2011 9:28:48 PM
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
    class CTerrain;

    enum VertexShader
    {
        eVS3DPosNorTexStd,
        eVS3DPosNorTexInst,
        eVS3DPosColStd,
        eVS2DPosColTex,

        NUM_VERTEX_SHADERS,
    };

    enum PixelShader
    {
        ePS3DStd,
        ePS3DColor,
        ePS2DTex,

        NUM_PIXEL_SHADERS,
    };

    enum SamplerState
    {
        eSamplerLinear,
        eSamplerNearest,

        NUM_SAMPLER_STATES,
    };

    class CRenderKey
    {
    public:
        // CRenderKey constructor
        CRenderKey();

        // CRenderKey destructor
        ~CRenderKey();
        /***************************************\
        | class methods                         |
        \***************************************/
        void Clear( void );
        uint64 Encode( void );
        void Decode( uint64 nKey );

    public:
        /***************************************\
        | class members                         |
        \***************************************/
        uint    m_nVShader;
        uint    m_nPShader;
        uint    m_nSampler;
        uint    m_nTexture;
        uint    m_nTransparant;

        float   m_fDepth;
    };


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
        void Render( CTerrain* pTerrain );

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
        //  UpdateBuffer
        //  Updates a buffers data
        //-----------------------------------------------------------------------------
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData );

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
        void DrawDebugSphere( const RSphere& s );

        //-----------------------------------------------------------------------------
        //  DrawDebugBox
        //  Renders a wireframe debug AABB
        //-----------------------------------------------------------------------------
        void DrawDebugBox( const RAABB& box, const RVector3& vColor );

        //-----------------------------------------------------------------------------
        //  DrawDebugRay
        //  Draws a vector from the start point the length of the vector
        //-----------------------------------------------------------------------------
        void DrawDebugRay( const RVector3& start, const RVector3& dir );

        //-----------------------------------------------------------------------------
        //  SwapBuffers
        //  Swaps last and previous frames buffers
        //-----------------------------------------------------------------------------
        void SwapBuffers( void );

        //-----------------------------------------------------------------------------
        //  DrawText
        //  Renders a string of text on screen at (nLeft, nTop)
        //-----------------------------------------------------------------------------
        void DrawString( uint nLeft, uint nTop, const char* szText );

        //-----------------------------------------------------------------------------
        //  Accessors/Mutators
        //-----------------------------------------------------------------------------
        inline IGraphicsDevice* GetGraphicsDevice( void );
        inline void SetPixelShader( PixelShader nShader );
        inline void SetVertexShader( VertexShader nShader );
        inline void SetSamplerState( SamplerState nState );

    private:
        //-----------------------------------------------------------------------------
        //  LoadShaders
        //  Loads all the shaders
        //-----------------------------------------------------------------------------
        void LoadShaders( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        RTransform      m_pTransforms[2][MAX_RENDER_COMMANDS];
        TRenderCommand  m_pRenderCommands[2][MAX_RENDER_COMMANDS];

        RTransform*     m_pPrevTransforms;
        RTransform*     m_pCurrTransforms;

        TRenderCommand* m_pPrevCommands;
        TRenderCommand* m_pCurrCommands;

        IGraphicsDevice*    m_pDevice;

        // Constant buffers
        IGfxBuffer* m_pViewProjCB;
        IGfxBuffer* m_pWorldCB;
        IGfxBuffer* m_pLightCB;

        // Default and debug meshes
        CMesh*              m_pDefaultMesh;
        IGfxBuffer*         m_pLineBuffer;
        IGfxBuffer*         m_pPlaneBuffer;

        // Textures
        IGfxTexture2D*      m_pDefaultTexture;
        IGfxTexture2D*      m_pWhiteTexture;

        // The graphics objects, that are referenced by enums
        IGfxVertexShader*   m_ppVertexShaders[ NUM_VERTEX_SHADERS ];
        IGfxVertexLayout*   m_ppVertexLayouts[ NUM_VERTEX_SHADERS ];
        IGfxPixelShader*    m_ppPixelShaders[ NUM_PIXEL_SHADERS ];
        IGfxSamplerState*   m_ppSamplerStates[ NUM_SAMPLER_STATES ];

        struct TDebugBox
        {
            RAABB       box;
            RVector3    color;
        };

        struct TDebugRay
        {
            RVector3    start;
            RVector3    end;
        };

        CView*  m_pCurrentView;

        RVector4    m_vLights[MAX_LIGHTS];
        uint32      m_nNumActiveLights;
        uint32      _padding[3];
        atomic_t    m_nNumCommands;
        uint        m_nPrevNumCommands;
        bool        m_bUpdateLighting;

        CMesh*      m_pSphereMesh;

        RSphere     m_DebugSpheres[2][1024*32];
        RSphere*    m_pPrevDebugSpheres;
        RSphere*    m_pCurrDebugSpheres;
        atomic_t    m_nNumSpheres;
        uint        m_nPrevNumSpheres;

        TDebugBox   m_DebugBoxes[2][1024*32];
        TDebugBox*  m_pPrevDebugBoxes;
        TDebugBox*  m_pCurrDebugBoxes;
        atomic_t    m_nNumBoxes;
        uint        m_nPrevNumBoxes;

        TDebugRay   m_DebugRays[2][1024*32];
        TDebugRay*  m_pPrevDebugRays;
        TDebugRay*  m_pCurrDebugRays;
        atomic_t    m_nNumRays;
        uint        m_nPrevNumRays;

        CMesh*      m_pDebugBox;
    };

    //-----------------------------------------------------------------------------
    //  Accessors/Mutators
    //-----------------------------------------------------------------------------
    IGraphicsDevice* CRenderer::GetGraphicsDevice( void )
    {
        return m_pDevice;
    }
    void CRenderer::SetPixelShader( PixelShader nShader )
    {
        ASSERT( nShader >= 0 && nShader < NUM_PIXEL_SHADERS );
        ASSERT( m_ppPixelShaders[ nShader ] );

        m_pDevice->SetPixelShader( m_ppPixelShaders[ nShader ] );
    }
    void CRenderer::SetVertexShader( VertexShader nShader )
    {
        ASSERT( nShader >= 0 && nShader < NUM_VERTEX_SHADERS );
        ASSERT( m_ppVertexLayouts[ nShader ] );
        ASSERT( m_ppVertexShaders[ nShader ] );

        m_pDevice->SetVertexLayout( m_ppVertexLayouts[ nShader ] );
        m_pDevice->SetVertexShader( m_ppVertexShaders[ nShader ] );
    }
    void CRenderer::SetSamplerState( SamplerState nState )
    {
        ASSERT( nState >= 0 && nState < NUM_SAMPLER_STATES );
        ASSERT( m_ppSamplerStates[ nState ] );

        m_pDevice->SetPSSamplerState( m_ppSamplerStates[ nState ] );
    }

} // namespace Riot

#endif // #ifndef _RENDERER_H_
