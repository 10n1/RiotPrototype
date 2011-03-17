/*********************************************************\
File:       Window.h
Purpose:    Abstration for creating a platform-independent
            window
\*********************************************************/
#include "Window.h"

CWindow::CWindow()
    : m_pSystemWindow( NULL )
    , m_nHeight( 0 )
    , m_nWidth( 0 )
{
}

CWindow::~CWindow()
{
} 

handle CWindow::GetSystemWindow( void )
{
    return m_pSystemWindow;
}
