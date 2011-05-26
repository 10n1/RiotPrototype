/*********************************************************\
File:           PhysicsSystem.h
Purpose:        
Author:         Kyle Weicht
Created:        5/24/2011
Modified:       5/24/2011 4:20:41 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_
#include "common.h"

namespace Riot
{
    class CObject;

    class CPhysicsSystem
    {
    public:
        // CPhysicsSystem constructor
        CPhysicsSystem();

        // CPhysicsSystem destructor
        ~CPhysicsSystem();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  AddObject
        //  Adds an object
        //-----------------------------------------------------------------------------
        static void AddObject( CObject* pObject );

        static void ProcessObjects( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/
        static CObject*    m_pObjects[ MAX_OBJECTS ];
        static uint        m_nNumObjects;
    };

} // namespace Riot


#endif // #ifndef _PHYSICSSYSTEM_H_
