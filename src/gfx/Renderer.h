/*********************************************************\
File:           Renderer.h
Purpose:        Abstraction between the API and the engine
Author:         Kyle Weicht
Created:        4/11/2011
Modified:       5/20/2011 7:53:57 AM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "common.h"
#include "IListener.h"
#include "Graphics.h"
#include "Message.h"
#include "Mesh.h"

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
        eVS3DPosNorTexNoTransform,
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

    enum Material
    {
        eMatStandard,
        eMatWireframe,

        NUM_MATERIALS,
    };

    struct TRenderCommand
    {
    public:
        // TRenderCommand constructor
        TRenderCommand();

        // TRenderCommand destructor
        ~TRenderCommand();
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
        uint    m_nMaterial;
        uint    m_nSampler;
        uint    m_nTexture;
        uint    m_nTransparant;
        uint    m_nMesh;

        float   m_fDepth;
    };

    inline uint  DecodeTransparant( uint64 nKey ) { return ( nKey >> 0 ) & 0x1;   }
    inline uint  DecodeTexture    ( uint64 nKey ) { return ( nKey >> 1 ) & 0xF;   }
    inline uint  DecodeSampler    ( uint64 nKey ) { return ( nKey >> 5 ) & 0x3;   }
    inline uint  DecodeMaterial   ( uint64 nKey ) { return ( nKey >> 7 ) & 0xF;   }
    inline uint  DecodeMesh       ( uint64 nKey ) { return ( nKey >> 11 ) & 0xFF; }
    inline float DecodeDepth      ( uint64 nKey ) 
    { 
        uint nFloat = ( nKey >> 32 );
        float f = (float&)nFloat;

        return f;
    }

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
        sint CreateMesh(  uint nVertexStride, 
            uint nVertexCount, 
            uint nIndexSize, 
            uint nIndexCount, 
            void* pVertices, 
            void* pIndices,
            GFX_BUFFER_USAGE nUsage = GFX_BUFFER_USAGE_DEFAULT );
        sint CreateMesh( void ); 
        sint LoadMesh( const char* szFilename );
        sint CreateDynamicBox( void );

        //-----------------------------------------------------------------------------
        //  UpdateBuffer
        //  Updates a buffers data
        //-----------------------------------------------------------------------------
        void UpdateBuffer( IGfxBuffer* pBuffer, void* pData );

        //-----------------------------------------------------------------------------
        //  LoadTextureXD
        //  Loads a texture
        //-----------------------------------------------------------------------------
        sint LoadTexture2D( const char* szFilename );

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
        void AddCommand( uint64 nCmd, RTransform& transform );

        //-----------------------------------------------------------------------------
        //  AddLight
        //  Adds a light to the scene
        //-----------------------------------------------------------------------------
        void AddPointLight( const RVector3& vPos, float fRange );
        void AddDirLight( const RVector3& vDir );

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
        
        //-----------------------------------------------------------------------------
        //  Sort
        //  Sorts the list of commands for rendering efficiency
        //-----------------------------------------------------------------------------
        void Sort( void );

    private:
        //-----------------------------------------------------------------------------
        //  LoadShaders
        //  Loads all the shaders
        //-----------------------------------------------------------------------------
        void LoadShaders( void );

        //-----------------------------------------------------------------------------
        //  ProcessCommand
        //  Processes a render command
        //-----------------------------------------------------------------------------
        inline void ProcessCommand( uint64 nCmd, const RTransform& transform );
       
    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static const MessageType    MessagesReceived[];
        static const uint           NumMessagesReceived;

        RTransform      m_pTransforms[2][MAX_RENDER_COMMANDS];
        uint64          m_pRenderCommands[2][MAX_RENDER_COMMANDS];

        RTransform*     m_pPrevTransforms;
        RTransform*     m_pCurrTransforms;

        uint64* m_pPrevCommands;
        uint64* m_pCurrCommands;

        IGraphicsDevice*    m_pDevice;

        // Constant buffers
        IGfxBuffer* m_pViewProjCB;
        IGfxBuffer* m_pWorldCB;
        IGfxBuffer* m_pLightCB;

        // Default and debug meshes
        sint                m_nDefaultMesh;
        IGfxBuffer*         m_pLineBuffer;
        IGfxBuffer*         m_pPlaneBuffer;

        // Textures
        sint    m_nDefaultTexture;

        // The graphics objects, that are referenced by enums
        IGfxVertexShader*   m_ppVertexShaders[ NUM_VERTEX_SHADERS ];
        IGfxVertexLayout*   m_ppVertexLayouts[ NUM_VERTEX_SHADERS ];
        IGfxPixelShader*    m_ppPixelShaders[ NUM_PIXEL_SHADERS ];
        IGfxSamplerState*   m_ppSamplerStates[ NUM_SAMPLER_STATES ];

        // Meshes
        CMesh*              m_ppMeshes[ MAX_MESHES ];
        atomic_t            m_nNumMeshes;

        // Textures
        IGfxTexture2D*      m_ppTextures[ MAX_TEXTURES ];
        atomic_t            m_nNumTextures;

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

        struct TLights
        {
            RVector4    vDirLights[ MAX_LIGHTS ];
            RVector4    vPointLights[ MAX_LIGHTS ];
            atomic_t    nNumActivePointLights;
            atomic_t    nNumActiveDirLights;
            uint32      _padding[2];
        };
        TLights     m_pLights[2];
        TLights*    m_pCurrLights;
        TLights*    m_pPrevLights;

        atomic_t    m_nNumCommands;
        sint        m_nPrevNumCommands;

        sint        m_nSphereMesh;

        TDebugBox   m_DebugBoxes[2][1024*32];
        TDebugBox*  m_pPrevDebugBoxes;
        TDebugBox*  m_pCurrDebugBoxes;
        atomic_t    m_nNumBoxes;
        sint        m_nPrevNumBoxes;

        TDebugRay   m_DebugRays[2][1024*32];
        TDebugRay*  m_pPrevDebugRays;
        TDebugRay*  m_pCurrDebugRays;
        atomic_t    m_nNumRays;
        uint        m_nPrevNumRays;

        sint        m_nDebugBox;
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

    //-----------------------------------------------------------------------------
    //  ProcessCommand
    //  Processes a render command
    //-----------------------------------------------------------------------------
    void CRenderer::ProcessCommand( uint64 nCmd, const RTransform& transform )
    {
        //////////////////////////////////////////
        // Set the material
        uint m = DecodeMaterial( nCmd );
        switch( DecodeMaterial( nCmd ) )
        {
        case eMatStandard:
            m_pDevice->SetFillMode( GFX_FILL_SOLID );
            SetVertexShader( eVS3DPosNorTexStd );
            SetPixelShader( ePS3DStd );
            SetSamplerState( (SamplerState)DecodeSampler( nCmd ) );
            m_pDevice->SetPSTexture( 0, m_ppTextures[ DecodeTexture( nCmd ) ] );
            break;
        case eMatWireframe:
            m_pDevice->SetFillMode( GFX_FILL_WIREFRAME );
            SetVertexShader( eVS3DPosColStd );
            SetPixelShader( ePS3DColor );
            break;
        default:
            ASSERT( false );
            break;
        }

        //////////////////////////////////////////
        SetWorldMatrix( transform.GetTransformMatrix() );
        m_ppMeshes[ DecodeMesh( nCmd ) ]->DrawMesh();
    }

} // namespace Riot

#endif // #ifndef _RENDERER_H_
