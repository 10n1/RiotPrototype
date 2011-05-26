/*********************************************************\
File:           RenderSystem.cpp
Author:         Kyle Weicht
Created:        5/24/2011
Modified:       5/24/2011 4:26:07 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "RenderSystem.h"
#include "Object.h"
#include "engine.h"
#include "Renderer.h"

namespace Riot
{    
    CObject*    CRenderSystem::m_pObjects[ MAX_OBJECTS ] = { NULL };
    uint        CRenderSystem::m_nNumObjects = 0;

    // CRenderSystem constructor
    CRenderSystem::CRenderSystem()
    {
    }

    // CRenderSystem destructor
    CRenderSystem::~CRenderSystem()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  AddObject
    //  Adds an object
    //-----------------------------------------------------------------------------
    void CRenderSystem::AddObject( CObject* pObject )
    {
        m_pObjects[ m_nNumObjects++ ] = pObject;
    }

    void CRenderSystem::ProcessObjects( void )
    {
        static CRenderer* pRenderer = Engine::GetRenderer();

        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            CObject* pObject = m_pObjects[i];
            sint* nMesh;
            sint* nTex;
            RVector3* vPos;
            pObject->GetProperty( "mesh", (void**)&nMesh );
            pObject->GetProperty( "diffuse", (void**)&nTex );
            pObject->GetProperty( "position", (void**)&vPos );

            TRenderCommand cmd;
            RTransform t;
            cmd.m_nMesh = *nMesh;
            cmd.m_nTexture = *nTex;
            t.position = *vPos;

            pRenderer->AddCommand( cmd.Encode(), t );
        }
    }

} // namespace Riot
