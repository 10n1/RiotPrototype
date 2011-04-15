/*********************************************************\
File:           View.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/14/2011 10:57:25 PM
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

        m_vNewPosition = RVector4( 0.0f, 2.0f, -5.0f, 0.0f );
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
        RMatrix4 newRot = RMatrix4( RQuatGetMatrix( RQuatFromAxisAngle( m_vNewRight, -fRad ) ) );
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


        //m_mNewView.m11 = x.x;  m_mNewView.m21 = x.y;  m_mNewView.m31 = x.z;  m_mNewView.m41 = -DotProduct( x, m_vNewPosition);
        //m_mNewView.m12 = y.x;  m_mNewView.m22 = y.y;  m_mNewView.m32 = y.z;  m_mNewView.m42 = -DotProduct( y, m_vNewPosition);
        //m_mNewView.m13 = z.x;  m_mNewView.m23 = z.y;  m_mNewView.m33 = z.z;  m_mNewView.m43 = -DotProduct( z, m_vNewPosition);
        //m_mNewView.m14 = 0.0f; m_mNewView.m24 = 0.0f; m_mNewView.m34 = 0.0f; m_mNewView.m44 = 1.0f;

        m_mNewView = RMatrix4Identity();
        m_mNewView.r0 = x;
        m_mNewView.r1 = y;
        m_mNewView.r2 = z;
        Transpose( m_mNewView );

        //m_mNewView.r0 = RVector4( x.x, y.x, z.x, 0.0f );
        //m_mNewView.r0 = RVector4( x.y, y.y, z.y, 0.0f );
        //m_mNewView.r0 = RVector4( x.z, y.z, z.z, 0.0f );

        m_mNewView.r4 = RVector4( -DotProduct( x, m_vNewPosition), -DotProduct( x, m_vNewPosition), -DotProduct( x, m_vNewPosition), 1.0f );
    }

    //-----------------------------------------------------------------------------
    //  SetPerspective
    //  Sets the projection matrix
    //-----------------------------------------------------------------------------
    void CView::SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar )
    {
        fFoV = (fFoV/180.0f) * XM_PI;
        m_mNewProj = RMatrix4PerspectiveLH( fFoV, fAspectRatio, fNear, fFar );
        rmatrix4p
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
