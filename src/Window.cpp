/*********************************************************\
File:           Window.cpp
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:14:34 PM
Modified by:    Kyle Weicht
\*********************************************************/
#include "Window.h"
#include "System.h"
#include "Engine.h"
#include "TaskManager.h"

namespace Riot
{

    // CWindow constructor
    CWindow::CWindow()
        : m_pWindow( NULL )
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
    handle CWindow::GetWindow( void )
    {
        return m_pWindow;
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
