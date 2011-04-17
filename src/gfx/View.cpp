/*********************************************************\
File:           View.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/17/2011 1:23:38 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "View.h"
#include "memory.h"

namespace Riot
{

    // CView constructor
    CView::CView()
    {
        SetPerspective( 60.0f, 1024.0f/768.0f, 0.1f, 10000.0f );

        m_vNewPosition = RVector4( 0.0f, 0.0f, -5.0f, 0.0f );
        m_vNewLook =     RVector4( 0.0f, 0.0f, 1.0f, 0.0f );
        m_vNewUp =       RVector4( 0.0f, 1.0f, 0.0f, 0.0f );
    }

    // CView destructor
    CView::~CView()
    {
    }

    //-----------------------------------------------------------------------------
    //  Translate
    //  Moves the camera
    //-----------------------------------------------------------------------------
    void CView::TranslateX( float fTrans )
    {
        m_vNewPosition += (m_vNewRight * fTrans);
    }

    void CView::TranslateY( float fTrans )
    {
        m_vNewPosition += (m_vNewUp * fTrans);
    }

    void CView::TranslateZ( float fTrans )
    {
        m_vNewPosition += (m_vNewLook * fTrans);
    }

    //-----------------------------------------------------------------------------
    //  Rotate
    //  Rotates the camera
    //-----------------------------------------------------------------------------
    void CView::RotateX( float fRad )
    {
        //RMatrix4 newRot = RotationAxisMatrix( -fRad, m_vNewRight );
        RMatrix4 newRot = RMatrix4( RQuatGetMatrix( RQuatFromAxisAngle( m_vNewRight.xyz(), -fRad ) ) );
        //RVector4 newNewLook = m_vLook * newRot;
        RVector4 newNewLook = m_vNewLook * newRot ;

        float newY = newNewLook.y;
        if( newY < 0.99f && newY > -0.99f )
        {
            m_vNewLook = newNewLook;
        }
    }

    void CView::RotateY( float fRad )
    {
        m_vNewLook = m_vNewLook * RMatrix4RotationY( fRad );
    }


    //-----------------------------------------------------------------------------
    //  Update
    //  Updates the object
    //  TODO: Pre- and Post- updates?
    //-----------------------------------------------------------------------------
    void CView::Update( float fDeltaTime )
    {
        RVector4 x, y, z;

        z = m_vNewLook = Normalize( m_vNewLook );
        x = m_vNewRight = Normalize( CrossProduct( m_vNewUp, z ) );
        y = CrossProduct( z, x );

        m_mNewView.r0 = x;
        m_mNewView.r1 = y;
        m_mNewView.r2 = z;
        m_mNewView.r3 = RVector4Zero();
        Transpose( m_mNewView );
        m_mNewView.r3 = RVector4( -DotProduct( x, m_vNewPosition), -DotProduct( y, m_vNewPosition), -DotProduct( z, m_vNewPosition), 1.0f );
    }

    //-----------------------------------------------------------------------------
    //  SetPerspective
    //  Sets the projection matrix
    //-----------------------------------------------------------------------------
    void CView::SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar )
    {
        fFoV = fFoV * gs_DEGTORAD; // (fFoV/180.0f) * gs_PI;
        m_mNewProj = RMatrix4PerspectiveLH( fFoV, fAspectRatio, fNear, fFar );
        //RMatrix4PerspectiveLH
    }


    //-----------------------------------------------------------------------------
    //  GetView/ProjMatrix
    //  Returns the view/proj matrix
    //-----------------------------------------------------------------------------
    const RMatrix4& CView::GetViewMatrix( void )
    {
        return m_mNewView;
    }

    const RMatrix4& CView::GetProjMatrix( void )
    {
        return m_mNewProj;
    }

} // namespace Riot
