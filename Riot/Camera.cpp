#include "Camera.h"

Camera::Camera( void )
    : m_mWorld( XMMatrixIdentity() )
    , m_mView( XMMatrixIdentity() )
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
    m_mView = XMMatrixLookAtLH( vEye, vLookAt, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
    m_mProj = XMMatrixPerspectiveFovLH( fFovAngleY, fAspectRatio, fNearZ, fFarZ );
}
