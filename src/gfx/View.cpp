/*********************************************************\
File:           View.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       5/10/2011 6:21:30 PM
Modified by:    Kyle Weicht
 \*********************************************************/
#include <string>
#include "View.h"
#include "memory.h"
#include "Renderer.h"
#include "Engine.h"

namespace Riot
{

    // CView constructor
    CView::CView()
        //: m_Transform( RQuaternionZero(), RVector3( 0.0f, 90, -100.0f ) )
    {
        SetPerspective( 60.0f, 1024.0f/768.0f, 0.1f, 10000.0f );

        m_vPosition = RVector4( 10.0f, 100.0f, 10.0f, 0.0f );
        m_vLook =     RVector4( 0.0f, -4.0f, 10.0f, 0.0f );
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

        //m_Transform.TranslateLocalX( fTrans );
    }

    void CView::TranslateY( float fTrans )
    {
        m_vPosition += (m_vUp * fTrans);
        //m_Transform.TranslateLocalY( fTrans );
    }

    void CView::TranslateZ( float fTrans )
    {
        m_vPosition += (m_vLook * fTrans);
        //m_Transform.TranslateLocalZ( fTrans );
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

        //RQuaternion q = RQuatFromAxisAngle( RQuatGetXAxis( m_Transform.orientation ), fRad ) * m_Transform.orientation;
        //RQuaternion q = m_Transform.orientation * RQuatFromAxisAngle( RQuatGetXAxis( m_Transform.orientation ), fRad );
        //RVector3 z = RQuatGetZAxis( q );
        //if( z.y < 0.98f && z.y > -0.98f )
        //{
        //    m_Transform.RotateLocalX( fRad );
        //}
        //else
        //{
        //    int d = 0;
        //}
    }

    void CView::RotateY( float fRad )
    {
        m_vLook = m_vLook * RQuatRotationY( fRad );

        //m_Transform.RotateWorldY( fRad );
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

        
        //z = Normalize( RQuatGetZAxis(m_Transform.orientation) );
        //x = Normalize( CrossProduct( RVector3(0.0f,1.0f,0.0f), z ) );
        //y = CrossProduct( z, x );
        //
        //m_mView.r0 = Homogonize( x );
        //m_mView.r1 = Homogonize( y );
        //m_mView.r2 = Homogonize( z );
        //m_mView.r3 = RVector4Zero();
        //m_mView = Transpose( m_mView );
        //
        //m_mView.r3 = RVector4( -DotProduct( x, m_Transform.position), -DotProduct( y, m_Transform.position), -DotProduct( z, m_Transform.position), 1.0f );
    
        char szCameraData[256] = { 0 };

        sprintf( szCameraData, "Pos:  %f, %f, %f", m_vPosition.x, m_vPosition.y, m_vPosition.z );
        Engine::GetRenderer()->DrawString( 0, 16, szCameraData );
        sprintf( szCameraData, "Look: %f, %f, %f", m_vLook.x, m_vLook.y, m_vLook.z );
        Engine::GetRenderer()->DrawString( 0, 32, szCameraData );
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
