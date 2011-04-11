/*********************************************************\
File:           Window.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 5:14:47 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Window.h"
#include "System.h"

namespace Riot
{

    // CWindow constructor
    CWindow::CWindow()
        : m_pSystemWindow( NULL )
        , m_nHeight( 0 )
        , m_nWidth( 0 )
    {
    }

    // CWindow destructor
    CWindow::~CWindow()
    {
    } 

    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  ProcessMessages
    //  Processes system messages
    //-----------------------------------------------------------------------------
    void CWindow::ProcessMessages( void )
    {
        System::ProcessOSMessages();
    }

    //-----------------------------------------------------------------------------
    //  Accessors/Mutators
    //-----------------------------------------------------------------------------
    handle CWindow::GetSystemWindow( void )
    {
        return m_pSystemWindow;
    }

    uint CWindow::GetWidth( void )
    {
        return m_nWidth;
    }

    uint CWindow::GetHeight( void )
    {
        return m_nHeight;
    }

} // namespace Riot
