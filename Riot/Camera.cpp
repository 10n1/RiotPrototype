#include "Camera.h"

static const float gs_fSpeed = 0.5f; // 0.5 m/s

Camera::Camera( void )
    : m_mView( XMMatrixIdentity() )
    , m_mProj( XMMatrixIdentity() )
    , m_vEye( XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ) )
    , m_vLookAt( XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ) )
{
}

Camera::~Camera( void )
{
}

void Camera::Init( XMVECTOR vEye, 
                XMVECTOR vLookAt,
                float fFovAngleY,
                float fAspectRatio,
                float fNearZ,
                float fFarZ )
{
    m_vEye = vEye;
    m_vLookAt = vLookAt;
    m_mView = XMMatrixLookAtLH( vEye, vLookAt, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
    m_mProj = XMMatrixPerspectiveFovLH( fFovAngleY, fAspectRatio, fNearZ, fFarZ );
}

void Camera::Update( float fElapsedTime )
{
    XMVECTOR vMovement = XMVectorSet( fElapsedTime * gs_fSpeed, 0.0f, 0.0f, 0.0f );
    m_vEye += vMovement;
    m_vLookAt += vMovement;
    m_mView = XMMatrixLookAtLH( m_vEye, m_vLookAt, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
}

void Camera::KeyInput( CameraKeys Key )
{
}