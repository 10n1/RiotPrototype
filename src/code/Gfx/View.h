/*********************************************************\
File:           View.h
Purpose:        Interface for anything in the engine that
                can "view", aka, a camera
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/3/2011 8:47:02 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VIEW_H_
#define _VIEW_H_
#include "Common.h"
#include "Scene\Object.h"
#include "Types.h"
#include <Windows.h>
#include <xnamath.h>
#include "RiotMath.h"

class CView : public CObject
{
public:
    // CView constructor
    CView();

    // CView destructor
    ~CView();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Translate
    //  Moves the camera
    //-----------------------------------------------------------------------------
    void TranslateX( float fTrans );
    void TranslateY( float fTrans );
    void TranslateZ( float fTrans );
    // TODO: Should the view be attached to an object
    //       Maybe it should have the option? So some views
    //       are independent, while others are attached
    //       to their actors (eg: the player)

    //-----------------------------------------------------------------------------
    //  Rotate
    //  Rotates the camera
    //-----------------------------------------------------------------------------
    void RotateX( float fRad );
    void RotateY( float fRad );
    
    //-----------------------------------------------------------------------------
    //  Update
    //  Updates the object
    //  TODO: Pre- and Post- updates?
    //-----------------------------------------------------------------------------
    void Update( float fDeltaTime );

    //-----------------------------------------------------------------------------
    //  SetPerspective
    //  Sets the projection matrix
    //-----------------------------------------------------------------------------
    void SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar );

    //-----------------------------------------------------------------------------
    //  GetView/ProjMatrix
    //  Returns the view/proj matrix
    //-----------------------------------------------------------------------------
    const XMMATRIX& GetViewMatrix( void );
    const XMMATRIX& GetProjMatrix( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    XMMATRIX    m_mViewMatrix;
    XMMATRIX    m_mProjMatrix;

    XMVECTOR    m_vPosition;
    XMVECTOR    m_vUp;
    XMVECTOR    m_vLook;
    XMVECTOR    m_vRight;
};


#endif // #ifndef _VIEW_H_
