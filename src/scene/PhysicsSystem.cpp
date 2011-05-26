/*********************************************************\
File:           PhysicsSystem.cpp
Author:         Kyle Weicht
Created:        5/24/2011
Modified:       5/24/2011 4:21:46 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "PhysicsSystem.h"
#include "Object.h"
#include "engine.h"

namespace Riot
{
    CObject*    CPhysicsSystem::m_pObjects[ MAX_OBJECTS ] = { NULL };
    uint        CPhysicsSystem::m_nNumObjects = 0;

    // CPhysicsSystem constructor
    CPhysicsSystem::CPhysicsSystem()
    {
    }

    // CPhysicsSystem destructor
    CPhysicsSystem::~CPhysicsSystem()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  AddObject
    //  Adds an object
    //-----------------------------------------------------------------------------
    void CPhysicsSystem::AddObject( CObject* pObject )
    {
        m_pObjects[ m_nNumObjects++ ] = pObject;
    }

    void CPhysicsSystem::ProcessObjects( void )
    {
        float dt = Engine::m_fElapsedTime;

        for( uint i = 0; i < m_nNumObjects; ++i )
        {
            CObject* pObject = m_pObjects[i];

            RVector3* pos0;
            RVector3* vel0;
            RVector3* acc0;

            pObject->GetProperty( "position", (void**)&pos0 );
            pObject->GetProperty( "velocity", (void**)&vel0 );
            pObject->GetProperty( "acceleration", (void**)&acc0 );

            if( !pos0 || !vel0 || !acc0 )
            {
                // This object doesn't have all the necessary properties
                continue;
            }

            RVector3 vel05;
            RVector3 pos1;
            RVector3 vel1;
            RVector3 acc1;

            //  1. Calcualte new position
            pos1    = *pos0 + *vel0*dt + (0.5f * *acc0 * Square(dt));

            //  2. Calculate 1/2 of the new velocity
            vel05   = *vel0 + 0.5f * *acc0 * dt;

            //  3. Calculate the new acceleration
            acc1    = *acc0; // force / m;

            //  4. Calculate the other half of the new velocity
            vel1    = vel05 + 0.5 * acc1 * dt;

            *pos0 = pos1;
            *vel0 = vel1;
            *acc0 = acc1;
        }
    }


} // namespace Riot 
