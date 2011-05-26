/*********************************************************\
File:           RenderSystem.h
Purpose:        
Author:         Kyle Weicht
Created:        5/24/2011
Modified:       5/24/2011 4:25:52 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _RENDERSYSTEM_H_
#define _RENDERSYSTEM_H_
#include "common.h"

namespace Riot
{
    class CObject;

    class CRenderSystem
    {
    public:
        // CRenderSystem constructor
        CRenderSystem();

        // CRenderSystem destructor
        ~CRenderSystem();
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



#endif // #ifndef _RENDERSYSTEM_H_
