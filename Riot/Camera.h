#pragma once
#include <D3D11.h>
#include <xnamath.h>

enum CameraKeys
{
    MOVE_FORWARD = 'W',
    MOVE_BACKWARD = 'S',
    MOVE_LEFT = 'A',
    MOVE_RIGHT = 'D',
};

class Camera
{
public:
    Camera(void);
    ~Camera(void);
    
    void Init( XMVECTOR vEye,
            XMVECTOR vLookAt,
            float fFovAngleY,
            float fAspectRatio,
            float fNearZ,
            float fFarZ );

    void Update( float fElapsedTime );

    //-----------------------------------------------------------------------------
    //  Accessors
    //-----------------------------------------------------------------------------
    XMMATRIX GetView( void ) { return m_mView; }
    XMMATRIX GetProj( void ) { return m_mProj; }
    XMMATRIX GetViewProj( void ) { return m_mView * m_mProj; }
    XMVECTOR GetEye( void ) { return m_vEye; }
    XMVECTOR GetLookAt( void ) { return m_vLookAt; }

private:
    XMMATRIX m_mWorld;
    XMMATRIX m_mView;
    XMMATRIX m_mProj;
    XMVECTOR m_vEye;
    XMVECTOR m_vLookAt;

};
