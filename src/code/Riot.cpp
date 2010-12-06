/*********************************************************\
File:       Riot.cpp
Purpose:    Definition of the main engine
\*********************************************************/
#include "Riot.h"
#include "RiotTimer.h"

Riot::Riot( void )
    : m_hWnd( NULL )
    , m_bRunning( true )
    , m_nNumFrames( 0 )
    , m_fElapsedTime( 0.0f )
    , m_fRunningTime( 0.0f )
{
}

Riot::~Riot( void )
{

    ////////////////////////////////////////////
    // NO CODE BELOW THIS LINE
    DumpMemoryLeaks();
}
    
//-----------------------------------------------------------------------------
//  Run
//  Starts the engine/game. All variables are set programatically
//-----------------------------------------------------------------------------
void Riot::Run( void )
{
    uint nResult = 0;
    //-----------------------------------------------------------------------------
    // Initialization
    // Parse command line

    // Create window
    uint nWindowWidth = 1024,
         nWindowHeight = 768; // TODO: Read in from file
    nResult = _CreateWindow( nWindowWidth, nWindowHeight );

    if( nResult != 0 )
    {
        // TODO: Handle/retry
        return;
    }
    //-----------------------------------------------------------------------------

    Timer timer; // TODO: Should the timer be a class member?
    timer.Reset();
    float fFPSTime = 0.0f; // TODO: What's the best way to calculate FPS?
    float fFPS = 0.0f;
    //-----------------------------------------------------------------------------
    for( ;; )
    {
        //---------------------- Start of frame --------------------
        // pMessageSystem->ProcessMessages();
        // pSceneGraph->StartFrame();
        // pRender->StartFrame();


        //-------------------------- Frame -------------------------
        // pSceneGraph->Update();


        //----------------------- End of frame ---------------------
        // pSceneGraph->EndFrame()
        // pRender->EndFrame();



        //----------------------------------------------------------
        // Perform Windows messaging
        MSG msg = { 0 };
        if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
                m_bRunning = false;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        // Make sure its still running
        if( m_bRunning != true )
            break;

        ++m_nNumFrames;
        m_fElapsedTime = (float)timer.GetTime();
        m_fRunningTime += m_fElapsedTime;
        fFPSTime += m_fElapsedTime;
        // Calculate FPS every 16 frames
        if( ( m_nNumFrames & 0xF ) == 0xF )
        {
            fFPS = 16.0f / fFPSTime;
            fFPSTime = 0.0f;
            printf( "FPS: %f\n", fFPS );
        }
    }
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    // Cleanup
    //-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
//  _CreateWindow
//  Creates the main window
//  TODO: Support multiple windows/views. Eg, the editor
//-----------------------------------------------------------------------------
uint Riot::_CreateWindow( uint nWidth, uint nHeight )
{
    char szName[] = "Riot"; // TODO: Don't hardcode
    /////////////////////////////////////
    // Create and register the class
    HINSTANCE hInst = GetModuleHandle( NULL );
    WNDCLASS wndClass;
    memset(&wndClass, 0, sizeof(wndClass));
    wndClass.lpfnWndProc    = Riot::_WndProc;
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
        0, 0,                // X, Y position
        nWidth, nHeight,     // Width and height (border width 16 and 38)
        0, 0,                // Parent window, Menu handle
        hInst,               // Instance, void misc
        0 );

    if( m_hWnd == NULL )
    {
        // TODO: Handle correctly
        MessageBox( 0, "CreateWindow failed", "Error", 0 );
        return 1;
    }

    ShowWindow( m_hWnd, SW_SHOWNORMAL ); // TODO: Don't hardcode the SW_SHOWNORMAL

    return 0;
}

//-----------------------------------------------------------------------------
//  _WndProc
//  Windows message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK Riot::_WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch(nMsg)
    {
    case WM_CREATE:
        {
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
            
            return 0;
        }
    case WM_KEYDOWN: 
    case WM_SYSKEYDOWN:
        {
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
