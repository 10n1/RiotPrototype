/*********************************************************\
File:           Object.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/17/2011 5:06:25 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Object.h"
#include "Mesh.h"
#include "ComponentManager.h"
#include "Engine.h"

namespace Riot
{

    // CObject constructor
    CObject::CObject()
    {
        for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        {
            m_pComponentIndices[i] = -1; // -1 means the object doesn't have that component
        }
    }

    // CObject destructor
    CObject::~CObject()
    {
    }

    //-----------------------------------------------------------------------------
    //  GetComponentIndex
    //  Returns which slot in the component this object owns
    //-----------------------------------------------------------------------------
    sint CObject::GetComponentIndex( eComponentType nComponent )
    {
        return m_pComponentIndices[ nComponent ];
    }

    //-----------------------------------------------------------------------------
    //  Reset
    //  Removes all components and resets the object
    //-----------------------------------------------------------------------------
    void CObject::Reset( void )
    {
        CComponentManager* pManager = Engine::GetComponentManager();

        //for( uint i = 0; i < eNUMCOMPONENTS; ++i )
        for( eComponentType i = (eComponentType)0; i < eNUMCOMPONENTS; i = (eComponentType)(i + 1) )
        {
            // Make sure we have a component to detach
            if( m_pComponentIndices[ i ] != -1 )
            {
                pManager->RemoveComponent( i, m_pComponentIndices[ i ] );
                m_pComponentIndices[ i ] = -1;
            }
        }
    }

} //namespace Riot
