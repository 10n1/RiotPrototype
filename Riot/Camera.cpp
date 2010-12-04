#include "Camera.h"

static const float gs_fSpeed = 100.0f;

Camera::Camera( void )
    : m_mView( XMMatrixIdentity() )
    , m_mProj( XMMatrixIdentity() )
    , m_vEye( XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ) )
    , m_vLookAt( XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f ) )
    , m_fDirectionZ( 0.0f )
    , m_fDirectionX( 0.0f )
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
    XMVECTOR vPosDelta = XMVectorSet( fElapsedTime * gs_fSpeed * m_fDirectionX, 
                                      0.0f, 
                                      fElapsedTime * gs_fSpeed * m_fDirectionZ, 
                                      0.0f );
    m_vEye += vPosDelta;
    m_vLookAt += vPosDelta;
    m_mView = XMMatrixLookAtLH( m_vEye, m_vLookAt, XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) );
    m_fDirectionX = 0.0f;
    m_fDirectionZ = 0.0f;
}

void Camera::KeyInput( CameraKeys Key )
{
    switch( Key )
    {
    case MOVE_FORWARD:
        {
            m_fDirectionZ = 1.0f;
            break;
        }
    case MOVE_BACKWARD:
        {
            m_fDirectionZ = -1.0f;
            break;
        }
    case MOVE_LEFT:
        {
            m_fDirectionX = -1.0f;
            break;
        }
    case MOVE_RIGHT:
        {
            m_fDirectionX = 1.0f;
            break;
        }
    }
}
