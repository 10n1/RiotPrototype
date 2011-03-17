/*********************************************************\
File:       Riot.cpp
Purpose:    Definition of the main engine
\*********************************************************/
#include "Riot.h"
#include "Timer.h"
#include <stdio.h> // For printf
#include "Window.h"

#if defined( OS_WINDOWS )
#include "PlatformDependent\Win32Window.h"
#elif defined( OS_OSX )
#include "PlatformDependent\OSXWindow.h"
#elif defined( OS_LINUX )
#include "PlatformDependent\LinuxWindow.h"
#endif

Riot::Riot( void )
    : m_pMainWindow( NULL )
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
    // TODO: Parse command line

    // Create window
    uint nWindowWidth = 1024,
         nWindowHeight = 768; // TODO: Read in from file

    // Create the new window object...
#if defined( OS_WINDOWS )
    m_pMainWindow = new CWin32Window();
#elif defined( OS_OSX )
    m_pMainWindow = new COSXWindow();
#elif defined( OS_LINUX )
    m_pMainWindow = new CLinuxWindow();
#endif
    // ...then create the actual window
    m_pMainWindow->CreateMainWindow( nWindowWidth, nWindowHeight );

    // Create the input system
    m_pInput = new RiotInput();

    //-----------------------------------------------------------------------------

    Timer timer; // TODO: Should the timer be a class member?
    timer.Reset();
    float fFPSTime = 0.0f; // TODO: What's the best way to calculate FPS?
    float fFPS = 0.0f;
    //-----------------------------------------------------------------------------
    while( m_bRunning )
    {
        //---------------------- Start of frame --------------------
        // pMessageSystem->ProcessMessages();
        // pSceneGraph->StartFrame();
        // pRender->StartFrame();
        m_pInput->PollInput();
        if( m_pInput->IsKeyDown( VK_ESCAPE ) )
            m_bRunning = false;

        //-------------------------- Frame -------------------------
        // pSceneGraph->Update();


        //----------------------- End of frame ---------------------
        // pSceneGraph->EndFrame()
        // pRender->EndFrame();



        //----------------------------------------------------------
        // Perform system messaging
        m_pMainWindow->ProcessMessages();

        // Perform timing
        ++m_nNumFrames;
        m_fElapsedTime = (float)timer.GetTime();
        m_fRunningTime += m_fElapsedTime;
        fFPSTime += m_fElapsedTime;
        // Calculate FPS every 16 frames
        if( ( m_nNumFrames & 0xF ) == 0x0 )
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
    SAFE_RELEASE( m_pMainWindow );
    SAFE_RELEASE( m_pInput );
}
