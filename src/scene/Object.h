/*********************************************************\
File:           Object.h
Purpose:        Base interface for every object in the scene
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/17/2011 5:06:17 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "common.h"
#include "Component.h"

namespace Riot
{
    class CMesh;

    class CObject
    {
        friend class CObjectManager;
    public:
        // CObject constructor
        CObject();

        // CObject destructor
        ~CObject();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  GetComponentIndex
        //  Returns which slot in the component this object owns
        //-----------------------------------------------------------------------------
        sint GetComponentIndex( eComponentType nComponent );

        //-----------------------------------------------------------------------------
        //  Reset
        //  Removes all components and resets the object
        //-----------------------------------------------------------------------------
        void Reset( void );

    private:
        /***************************************\
        | class members                         |
        \***************************************/    
        sint    m_pComponentIndices[eNUMCOMPONENTS];
    };


} //namespace Riot

#endif // #ifndef _OBJECT_H_
