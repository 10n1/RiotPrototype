/*********************************************************\
File:           Object.h
Purpose:        Base interface for every object in the scene
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/3/2011 8:47:02 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "Common.h"
#include "IRefCounted.h"
#include "Types.h"
#include "Component.h"

#include <Windows.h> // TODO: Remove XNA math
#include <xnamath.h>
#include "RiotMath.h"

class CMesh;
class CMaterial;
class CObjectManager;

class CObject : public IRefCounted
{
    friend class CObjectManager;
public:
    // CObject constructor
    CObject();

    // CObject destructor
    virtual ~CObject();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  AddComponent
    //  Adds a component of the specified type
    //-----------------------------------------------------------------------------
    void AddComponent( eComponentType nType );

    //-----------------------------------------------------------------------------
    //  GetComponentIndex
    //  Returns which slot in the component this object owns
    //-----------------------------------------------------------------------------
    sint GetComponentIndex( eComponentType nComponent );

    //-----------------------------------------------------------------------------
    //  RemoveComponent
    //  Removes a component of the specified type
    //-----------------------------------------------------------------------------
    void RemoveComponent( eComponentType nType );
    
    //-----------------------------------------------------------------------------
    //  Reset
    //  Removes all components and resets the object
    //-----------------------------------------------------------------------------
    void Reset( void );

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    CMesh*      GetMesh( void );
    CMaterial*  GetMaterial( void );
    void SetMesh( CMesh* pMesh );
    void SetMaterial( CMaterial* pMaterial );
protected:
    /***************************************\
    | class members                         |
    \***************************************/
    sint        m_pComponentIndices[eNUMCOMPONENTS];

    CMesh*      m_pMesh;
    CMaterial*  m_pMaterial;
};


#endif // #ifndef _OBJECT_H_
