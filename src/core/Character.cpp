#include "Character.h"
#include "Engine.h"
#include "ObjectManager.h"
#include "Mesh.h"
#include "View.h"
#include "IComponent.h"

namespace Riot
{
    CCharacter::CCharacter( void )
        : m_nMesh( -1 )
        , m_pView( NULL )
    {
        m_tTransform = RTransform();
        m_tTransform.position = RVector3( 10.0f, 90.0f, 20.0f );
        m_tTransform.scale = 100.0f;

        m_nCharacterID = Engine::GetObjectManager()->CreateObject();
        m_nMesh = Engine::GetRenderer()->LoadMesh( "Assets/Meshes/drone.sdkmesh.mesh" );
        Engine::GetObjectManager()->AddComponent( m_nCharacterID, Riot::eComponentCharacter );
        Engine::GetObjectManager()->AddComponent( m_nCharacterID, Riot::eComponentCollidable );
        Engine::GetObjectManager()->AddComponent( m_nCharacterID, Riot::eComponentRigidBody );
        Engine::GetObjectManager()->AddComponent( m_nCharacterID, Riot::eComponentRender );
        Engine::GetObjectManager()->AddComponent( m_nCharacterID, Riot::eComponentLight );
        Engine::GetObjectManager()->SendMessage( Riot::eComponentMessageMesh, m_nCharacterID, m_nMesh );
        Engine::GetObjectManager()->SendMessage( Riot::eComponentMessageTransform, m_nCharacterID, &m_tTransform );

        //m_pView = new CView();
    }

    CCharacter::~CCharacter( void )
    {
        SAFE_RELEASE( m_pView );
    }

    CView* CCharacter::GetView( void )
    {
        return m_pView;
    }

    RTransform& CCharacter::GetTransform( void )
    {
        return m_tTransform;
    }
};

