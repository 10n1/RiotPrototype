/*********************************************************\
File:           Window.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 4:36:29 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Window.h"

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
