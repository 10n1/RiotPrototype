/*********************************************************\
File:       Window.h
Purpose:    Abstration for creating a platform-independent
            window
\*********************************************************/
#include "Common.h"
#include "Window.h"
#include "memory.h"

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

uint CWindow::GetWidth( void )
{
    return m_nWidth;
}

uint CWindow::GetHeight( void )
{
    return m_nHeight;
}
