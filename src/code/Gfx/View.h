/*********************************************************\
File:           View.h
Purpose:        Interface for anything in the engine that
                can "view", aka, a camera
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 7:28:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _VIEW_H_
#define _VIEW_H_
#include <Windows.h>
#include <xnamath.h>

class CView
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
    //  UpdateViewMatrix
    //  Updates the views matrix
    //-----------------------------------------------------------------------------
    void UpdateViewMatrix( void );

    //-----------------------------------------------------------------------------
    //  SetPerspective
    //  Sets the projection matrix
    //-----------------------------------------------------------------------------
    void SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar );

private:
    /***************************************\
    | class members                         |
    \***************************************/
    XMVECTOR    m_Position;
    XMVECTOR    m_Up;
    XMVECTOR    m_Look;
    XMVECTOR    m_Right;

    XMMATRIX    m_mViewMatrix;
    XMMATRIX    m_mProjMatrix;
};


#endif // #ifndef _VIEW_H_
