/*********************************************************\
File:           Object.h
Purpose:        Base interface for every object in the scene
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/23/2011 7:07:43 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OBJECT_H_
#define _OBJECT_H_
#include "Common.h"
#include "IRefCounted.h"
#include "Types.h"

#include <Windows.h> // TODO: Remove XNA math
#include <xnamath.h>

class CMesh;
class CMaterial;

class CObject : public IRefCounted
{
public:
    // CObject constructor
    CObject();

    // CObject destructor
    virtual ~CObject();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Update
    //  Updates the object
    //  TODO: Pre- and Post- updates?
    //-----------------------------------------------------------------------------
    virtual void Update( float fDeltaTime );

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    CMesh*      GetMesh( void );
    CMaterial*  GetMaterial( void );
    void SetMesh( CMesh* pMesh );
    void SetMaterial( CMaterial* pMaterial );

    const XMVECTOR& GetPosition( void );
    const XMVECTOR& GetOrientation( void );
    void SetPosition( const XMVECTOR& vPosition );
    void SetOrientation( const XMVECTOR& vOrientation );
protected:
    /***************************************\
    | class members                         |
    \***************************************/
    XMVECTOR    m_vPosition;
    XMVECTOR    m_vOrientation;

    CMesh*      m_pMesh;
    CMaterial*  m_pMaterial;
};


#endif // #ifndef _OBJECT_H_
