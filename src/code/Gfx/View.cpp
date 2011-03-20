/*********************************************************\
File:           View.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       3/19/2011 7:31:49 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "View.h"

// CView constructor
CView::CView()
{
    SetPerspective( 60.0f, 1024.0f/768.0f, 0.1f, 10000.0f );
    m_Position = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    m_Look = XMVectorSet( 0.0f, 0.0f, -1.0f, 0.0f );
    m_Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
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
    m_Position += (m_Right * fTrans);
}

void CView::TranslateY( float fTrans )
{
    m_Position += (m_Up * fTrans);
}

void CView::TranslateZ( float fTrans )
{
    m_Position += (m_Look * fTrans);
}

//-----------------------------------------------------------------------------
//  Rotate
//  Rotates the camera
//-----------------------------------------------------------------------------
void CView::RotateX( float fRad )
{
    XMMATRIX rot = XMMatrixRotationAxis( m_Right, fRad );
    m_Look = XMVector4Transform( m_Look, rot );
}

void CView::RotateY( float fRad )
{
    XMMATRIX rot = XMMatrixRotationY( fRad );
    m_Look = XMVector4Transform( m_Look, rot );
}

//-----------------------------------------------------------------------------
//  UpdateViewMatrix
//  Updates the views matrix
//-----------------------------------------------------------------------------
void CView::UpdateViewMatrix( void )
{
    XMVECTOR vX, vY, vZ;

    m_Look = vZ = XMVector4Normalize( m_Look );
    m_Right = vX = XMVector4Normalize( XMVector3Cross( m_Up, vZ ) );
    vY = XMVector3Cross( m_Look, m_Right );

    m_mViewMatrix = XMMatrixIdentity();
    m_mViewMatrix.r[0] = vX;
    m_mViewMatrix.r[1] = vY;
    m_mViewMatrix.r[2] = vY;

    m_mViewMatrix._14 = -XMVectorGetX( XMVector3Dot( vX, m_Position ) );
    m_mViewMatrix._24 = -XMVectorGetX( XMVector3Dot( vY, m_Position ) );
    m_mViewMatrix._34 = -XMVectorGetX( XMVector3Dot( vZ, m_Position ) );
}


//-----------------------------------------------------------------------------
//  SetPerspective
//  Sets the projection matrix
//-----------------------------------------------------------------------------
void CView::SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar )
{
    m_mProjMatrix = XMMatrixPerspectiveFovLH( fFoV, fAspectRatio, fNear, fFar );
}
