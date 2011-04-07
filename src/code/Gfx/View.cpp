/*********************************************************\
File:           View.cpp
Author:         Kyle Weicht
Created:        3/19/2011
Modified:       4/6/2011 9:34:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "View.h"
#include "memory.h"

// CView constructor
CView::CView()
{
    SetPerspective( 60.0f, 1024.0f/768.0f, 0.1f, 10000.0f );
    m_vPosition = XMVectorSet( 0.0f, 2.0f, -5.0f, 0.0f );
    m_vLook = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
    m_vUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
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
    XMMATRIX rot = XMMatrixRotationAxis( m_vRight, fRad );
    m_vLook = XMVector4Transform( m_vLook, rot );
}

void CView::RotateY( float fRad )
{
    XMMATRIX rot = XMMatrixRotationY( fRad );
    m_vLook = XMVector4Transform( m_vLook, rot );
}


//-----------------------------------------------------------------------------
//  Update
//  Updates the object
//  TODO: Pre- and Post- updates?
//-----------------------------------------------------------------------------
void CView::Update( float fDeltaTime )
{
    XMVECTOR vX, vY, vZ;

    m_vLook = vZ = XMVector4Normalize( m_vLook );
    m_vRight = vX = XMVector4Normalize( XMVector3Cross( m_vUp, vZ ) );
    vY = XMVector3Cross( vZ, vX );
    
    m_mViewMatrix._11 = XMVectorGetX(vX); m_mViewMatrix._12 = XMVectorGetY(vX); m_mViewMatrix._13 = XMVectorGetZ(vX); m_mViewMatrix._14 = -XMVectorGetX( XMVector3Dot(vX, m_vPosition) );
    m_mViewMatrix._21 = XMVectorGetX(vY); m_mViewMatrix._22 = XMVectorGetY(vY); m_mViewMatrix._23 = XMVectorGetZ(vY); m_mViewMatrix._24 = -XMVectorGetX( XMVector3Dot(vY, m_vPosition) );
    m_mViewMatrix._31 = XMVectorGetX(vZ); m_mViewMatrix._32 = XMVectorGetY(vZ); m_mViewMatrix._33 = XMVectorGetZ(vZ); m_mViewMatrix._34 = -XMVectorGetX( XMVector3Dot(vZ, m_vPosition) );
    m_mViewMatrix._41 = 0.0f;             m_mViewMatrix._42 = 0.0f;             m_mViewMatrix._43 = 0.0f;             m_mViewMatrix._44 = 1.0f;

    //m_mViewMatrix = XMMatrixIdentity();
    //m_mViewMatrix.r[0] = vX;
    //m_mViewMatrix.r[1] = vY;
    //m_mViewMatrix.r[2] = vY;
    //
    //m_mViewMatrix._14 = -XMVectorGetX( XMVector3Dot( vX, m_vPosition ) );
    //m_mViewMatrix._24 = -XMVectorGetX( XMVector3Dot( vY, m_vPosition ) );
    //m_mViewMatrix._34 = -XMVectorGetX( XMVector3Dot( vZ, m_vPosition ) );

    m_mViewMatrix = XMMatrixTranspose( m_mViewMatrix );
}

//-----------------------------------------------------------------------------
//  SetPerspective
//  Sets the projection matrix
//-----------------------------------------------------------------------------
void CView::SetPerspective( float fFoV, float fAspectRatio, float fNear, float fFar )
{
    fFoV = (fFoV/180.0f) * XM_PI;
    m_mProjMatrix = XMMatrixPerspectiveFovLH( fFoV, fAspectRatio, fNear, fFar );
}


//-----------------------------------------------------------------------------
//  GetView/ProjMatrix
//  Returns the view/proj matrix
//-----------------------------------------------------------------------------
const XMMATRIX& CView::GetViewMatrix( void )
{
    return m_mViewMatrix;
}

const XMMATRIX& CView::GetProjMatrix( void )
{
    return m_mProjMatrix;
}
