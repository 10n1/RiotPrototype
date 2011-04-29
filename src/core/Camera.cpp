/*********************************************************\
File:           Camera.cpp
Author:         Kyle Weicht
Created:        4/17/2011
Modified:       4/28/2011 10:37:26 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Camera.h"
#include "Input.h"
#include "View.h"
#include "Engine.h"

namespace Riot
{

    /***************************************\
    | class members                         |
    \***************************************/        
    const MessageType    CCamera::MessagesReceived[] = 
    {
        mKeyDown,
        mKeyUp,
        mKeyPressed,
        mMouseButtonDown,
        mMouseButtonUp,
        mMouseMove,
        mResize,
    };
    const uint           CCamera::NumMessagesReceived = ARRAY_LENGTH( MessagesReceived );

    // CCamera constructor
    CCamera::CCamera()
        : m_pView( NULL )
        , m_fMovementSpeed( 5.0f )
        , m_bMouseControlled( false )
    {
        Engine::RegisterListener( this, MessagesReceived, NumMessagesReceived );
    }

    // CCamera destructor
    CCamera::~CCamera()
    {
        SAFE_RELEASE( m_pView );
    }
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  Update
    //  Updates the camera
    //-----------------------------------------------------------------------------
    void CCamera::Update( void )
    {
        m_pView->Update( );
    }

    //-----------------------------------------------------------------------------
    //  ProcessMessage
    //  Processes the input message
    //-----------------------------------------------------------------------------
    void CCamera::ProcessMessage( const TMessage& msg )
    {
        float fElapsedTime = Engine::m_fElapsedTime;
        switch( msg.nType )
        {
        case mKeyDown:
            {
                const float fLookSpeed = gs_PI;
                switch( msg.nMessage )
                {
                case KEY_W:
                    {
                        m_pView->TranslateZ( fElapsedTime * m_fMovementSpeed );
                        break;
                    }
                case KEY_S:
                    {
                        m_pView->TranslateZ( -fElapsedTime * m_fMovementSpeed );
                        break;
                    }
                case KEY_A:
                    {
                        m_pView->TranslateX( -fElapsedTime * m_fMovementSpeed );
                        break;
                    }
                case KEY_D:
                    {
                        m_pView->TranslateX( fElapsedTime * m_fMovementSpeed );
                        break;
                    }
                case KEY_UP:
                    {
                        m_pView->RotateX( fElapsedTime * fLookSpeed );
                        break;
                    }
                case KEY_DOWN:
                    {
                        m_pView->RotateX( fElapsedTime * -fLookSpeed );
                        break;
                    }
                case KEY_LEFT:
                    {
                        m_pView->RotateY( fElapsedTime * -fLookSpeed );
                        break;
                    }
                case KEY_RIGHT:
                    {
                        m_pView->RotateY( fElapsedTime * fLookSpeed );
                        break;
                    }
                case KEY_SPACE:
                    {
                        m_pView->TranslateY( fElapsedTime * m_fMovementSpeed );
                        break;
                    }
                case KEY_C:
                    {
                        m_pView->TranslateY( -fElapsedTime * m_fMovementSpeed );
                        break;
                    }
                }
                break;
            }
        case mKeyPressed:
            {
                switch( msg.nMessage )
                {
                case KEY_SHIFT:
                    {
                        m_fMovementSpeed *= 2.5f;
                        break;
                    }
                }
                break;
            }
        case mKeyUp:
            {
                switch( msg.nMessage )
                {
                case KEY_SHIFT:
                    {
                        m_fMovementSpeed = 5.0f;
                        break;
                    }
                }
                break;
            }
        case mResize:
            {
                uint nWidth = (uint)(msg.nMessage >> 16);
                uint nHeight = msg.nMessage & 0x0000FFFF;

                m_pView->SetPerspective( 60.0f, ((float)nWidth)/nHeight, 0.1f, 10000.0f );                
                break;
            }
        case mMouseButtonDown:
            {
                switch( msg.nMessage )
                {
                case MOUSE_L_BUTTON:
                    {
                        m_bMouseControlled = true;
                        break;
                    }
                }
                break;
            }
        case mMouseButtonUp:
            {
                switch( msg.nMessage )
                {
                case MOUSE_L_BUTTON:
                    {
                        m_bMouseControlled = false;
                        break;
                    }
                }
                break;
            }
        case mMouseMove:
            {
                if( !m_bMouseControlled )
                    break;

                sint16 nDeltaX = (sint16)(msg.nMessage >> 16);
                sint16 nDeltaY = msg.nMessage & 0xFFFF;
                const float fLookSpeed = gs_PI;
                
                if( nDeltaX )
                {
                    m_pView->RotateY( fElapsedTime * fLookSpeed * nDeltaX );
                }
                if( nDeltaY )
                {
                    m_pView->RotateX( fElapsedTime * fLookSpeed * nDeltaY );
                }

                break;
            }
        default:
            {
                break;
            }
        }
    }

    //-----------------------------------------------------------------------------
    //  Accessors/mutators
    //-----------------------------------------------------------------------------
    CView* CCamera::GetView( void )
    {
        return m_pView;
    }
    void CCamera::SetView( CView* pView )
    {
        pView->AddRef();
        m_pView = pView;
    }

} // namespace Riot
