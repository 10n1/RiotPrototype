/*********************************************************\
File:           Win32Application.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 10:38:56 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Win32Application.h"
#include "Window.h"
#include "Input.h"
#include "Engine.h"

namespace Riot
{    
    // CWin32Application constructor
    CWin32Application::CWin32Application()
        : m_hWnd( NULL )
    {
    }

    // CWin32Application destructor
    CWin32Application::~CWin32Application()
    {
    }
    /***************************************\
    | class methods                         |
    \***************************************/
    
    //-----------------------------------------------------------------------------
    //  CreateMainWindow
    //  Creates a window (only 1 for now)
    //-----------------------------------------------------------------------------
    void CWin32Application::CreateMainWindow( uint nWidth, uint nHeight )
    {
        ASSERT( m_hWnd == NULL );

        wchar_t szName[] = L"Riot"; // TODO: Don't hardcode
        /////////////////////////////////////
        // Create and register the class
        HINSTANCE hInst = GetModuleHandle( NULL );
        WNDCLASS wndClass;
        memset(&wndClass, 0, sizeof(wndClass));
        wndClass.lpfnWndProc    = _WndProc;
        wndClass.hInstance      = hInst;
        wndClass.hCursor        = LoadCursor(0, IDC_ARROW);
        wndClass.lpszClassName  = szName;
        wndClass.style          = CS_HREDRAW | CS_VREDRAW;

        RegisterClass(&wndClass);

        /////////////////////////////////////
        // Create and show the window
        m_hWnd = CreateWindow(
            szName, szName,      // Class and window name
            WS_OVERLAPPEDWINDOW, // Window style
            CW_USEDEFAULT, 0,    // X, Y position
            nWidth, nHeight,     // Width and height (border width 16 and 38)
            0, 0,                // Parent window, Menu handle
            hInst,               // Instance
            this );    // void misc

        ASSERT( m_hWnd );

        ShowWindow( m_hWnd, SW_SHOWNORMAL ); // TODO: Don't hardcode the SW_SHOWNORMAL
    }

    //-----------------------------------------------------------------------------
    //  ProcessOSMessages
    //  Handles Windows messages
    //-----------------------------------------------------------------------------
    void CWin32Application::ProcessOSMessages( void )
    {
        //----------------------------------------------------------
        // Perform Windows messaging
        MSG msg = { 0 };
        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
            {
                // TODO: Pass to the engine
                Engine::SendMsg( TMessage( mShutdown ) );
            }

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    //-----------------------------------------------------------------------------
    //  _WndProc
    //  Handles Windows messages
    //-----------------------------------------------------------------------------
    LRESULT CALLBACK CWin32Application::_WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
    {    
        static CWin32Application*     pApp = NULL;
        switch(nMsg)
        {
        case WM_CREATE:
            {
                CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
                pApp = (CWin32Application*)pCreate->lpCreateParams;
                return 0;
            }
        case WM_CLOSE:
            {
                PostQuitMessage(0);
                return 0;
            }
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
        case WM_SIZING:
        case WM_SIZE:
            {
                RECT rcClient;
                GetClientRect( hWnd, &rcClient );
                unsigned int nWidth = rcClient.right - rcClient.left;
                unsigned int nHeight = rcClient.bottom - rcClient.top;
                Engine::PostMsg( TMessage( mResize, ((nWidth << 16 ) | nHeight) ) );
                return 0;
            }
        case WM_KEYDOWN: 
        case WM_SYSKEYDOWN:
            {
                // TODO: Use this or the GetKeyboardState method?
                //       This should be faster, no extra overhead from GetKeyboardState
                Engine::PostMsg( TMessage( mHardwareKeyboardDown, wParam ) );
                return 0;
            }
        case WM_MOUSEMOVE:
            {
                uint8 nMouse = 0;

                return 0;
            }
        case WM_KEYUP:
        case WM_SYSKEYUP:
            {
                Engine::PostMsg( TMessage( mHardwareKeyboardUp, wParam ) );
                return 0;
            }
        case WM_PAINT:
            {
                return 0;
            }
        default:
            return DefWindowProc(hWnd, nMsg, wParam, lParam);
        }

        return DefWindowProc(hWnd, nMsg, wParam, lParam);
    }

} // namespace Riot
