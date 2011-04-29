/*********************************************************\
File:           View.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/29/2011 12:37:37 PM
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

        m_vPosition = RVector4( 0.0f, 90, -100.0f, 0.0f );
        m_vLook =     RVector4( 0.0f, -4.0f, 5.0f, 0.0f );
        m_vUp =       RVector4( 0.0f, 1.0f, 0.0f, 0.0f );
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
        m_vPosition += (m_vRight * fTrans);
    }

    void CView::TranslateY( float fTrans )
    {
        m_vPosition += (m_vUp * fTrans);
    }

    void CView::TranslateZ( float fTrans )
    {
        m_vPosition += (m_vLook * fTrans);
    }

    //-----------------------------------------------------------------------------
    //  Rotate
    //  Rotates the camera
    //-----------------------------------------------------------------------------
    void CView::RotateX( float fRad )
    {
        RVector4 newLook = m_vLook * RQuatFromAxisAngle( m_vRight.xyz(), fRad ) ;

        float newY = newLook.y;
        if( newY < 0.99f && newY > -0.99f )
        {
            m_vLook = newLook;
        }
    }

    void CView::RotateY( float fRad )
    {
        m_vLook = m_vLook * RQuatRotationY( fRad );
    }


    //-----------------------------------------------------------------------------
    //  Update
    //  Updates the object
    //  TODO: Pre- and Post- updates?
    //-----------------------------------------------------------------------------
    void CView::Update( void )
    {
        RVector4 x, y, z;

        z = m_vLook = Normalize( m_vLook );
        x = m_vRight = Normalize( CrossProduct( m_vUp, z ) );
        y = CrossProduct( z, x );

        m_mView.r0 = x;
        m_mView.r1 = y;
        m_mView.r2 = z;
        m_mView.r3 = RVector4Zero();
        m_mView = Transpose( m_mView );

        m_mView.r3 = RVector4( -DotProduct( x, m_vPosition), -DotProduct( y, m_vPosition), -DotProduct( z, m_vPosition), 1.0f );
    }

    //-----------------------------------------------------------------------------
    //  SetPerspective
    //  Sets the projection matrix
    //-----------------------------------------------------------------------------
    void CView::SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar )
    {
        fFoV = fFoV * gs_DEGTORAD; // (fFoV/180.0f) * gs_PI;
        m_mProj = RMatrix4PerspectiveLH( fFoV, fAspectRatio, fNear, fFar );
        //RMatrix4PerspectiveLH
    }


    //-----------------------------------------------------------------------------
    //  GetView/ProjMatrix
    //  Returns the view/proj matrix
    //-----------------------------------------------------------------------------
    const RMatrix4& CView::GetViewMatrix( void )
    {
        return m_mView;
    }

    const RMatrix4& CView::GetProjMatrix( void )
    {
        return m_mProj;
    }

} // namespace Riot
