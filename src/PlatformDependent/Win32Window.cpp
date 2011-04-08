/*********************************************************\
File:       Win32Window.h
Purpose:    Creating windows on Windows
\*********************************************************/
#include "Win32Window.h"
#include <Windows.h>
#include "Memory.h"
#include "Riot.h"
#include "Gfx\Graphics.h"

//////////////////////////////////////////
// Define this here rather than the header
//  so Windows.h isn't included in a the header
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

CWin32Window::CWin32Window()
{
}

CWin32Window::~CWin32Window()
{
}

uint CWin32Window::CreateMainWindow( uint nWidth, uint nHeight )
{
    uint nResult = 0;

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
    m_pSystemWindow = CreateWindow(
        szName, szName,      // Class and window name
        WS_OVERLAPPEDWINDOW, // Window style
        CW_USEDEFAULT, 0,    // X, Y position
        nWidth, nHeight,     // Width and height (border width 16 and 38)
        0, 0,                // Parent window, Menu handle
        hInst,               // Instance
        this );              // void misc

    if( m_pSystemWindow == NULL )
    {
        // TODO: Handle correctly
        MessageBox( 0, L"CreateWindow failed", L"Error", 0 );
        return 1;
    }

    ShowWindow( (HWND)m_pSystemWindow, SW_SHOWNORMAL ); // TODO: Don't hardcode the SW_SHOWNORMAL

    return nResult;
}

void CWin32Window::ProcessMessages( void )
{
    //----------------------------------------------------------
    // Perform Windows messaging
    MSG msg = { 0 };
    if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
    {
        if( msg.message == WM_QUIT )
        {
            // TODO: Pass to the engine
        }

        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
}

//-----------------------------------------------------------------------------
//  _WndProc
//  Windows message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK _WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{    
    static CWindow*     pWindow = NULL;
    switch(nMsg)
    {
    case WM_CREATE:
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pWindow = (CWindow*)pCreate->lpCreateParams;
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
    case WM_SIZE:
        {
            RECT rcClient;
            GetClientRect( hWnd, &rcClient );
            unsigned int nWidth = rcClient.right - rcClient.left;
            unsigned int nHeight = rcClient.bottom - rcClient.top;
            Riot::GetGraphics()->Resize( nWidth, nHeight );
            return 0;
        }
    case WM_KEYDOWN: 
    case WM_SYSKEYDOWN:
        {
            // TODO: Use this or the GetKeyboardState method?
            //       This should be faster, no extra overhead from GetKeyboardState
            switch( wParam )
            {
            // Quit
            case VK_ESCAPE:
                {
                    PostQuitMessage(0);
                }
            }
            return 0;
        }
    case WM_MOUSEMOVE:
        {
            uint8 nMouse = 0;
            nMouse |= ( (wParam & MK_LBUTTON) ? eLMouseButton : 0);
            nMouse |= ( (wParam & MK_RBUTTON) ? eRMouseButton : 0);
            Riot::GetInput()->SetMouseButtonState( nMouse );

            return 0;
        }
    case WM_KEYUP:
    case WM_SYSKEYUP:
        {
            break;
        }
    default:
        return DefWindowProc(hWnd, nMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}
