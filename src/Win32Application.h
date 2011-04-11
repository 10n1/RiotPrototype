/*********************************************************\
File:           Win32Application.h
Purpose:        Interface for the Win32 application
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 10:34:18 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _WIN32APPLICATION_H_
#define _WIN32APPLICATION_H_
#include "common.h"
#include <Windows.h>

namespace Riot
{

class CWin32Application
{
public:
    // CWin32Application constructor
    CWin32Application();

    // CWin32Application destructor
    ~CWin32Application();
    /***************************************\
    | class methods                         |
    \***************************************/

    //-----------------------------------------------------------------------------
    //  CreateMainWindow
    //  Creates a window (only 1 for now)
    //-----------------------------------------------------------------------------
    void CreateMainWindow( uint nWidth, uint nHeight );

    //-----------------------------------------------------------------------------
    //  ProcessOSMessages
    //  Handles Windows messages
    //-----------------------------------------------------------------------------
    void ProcessOSMessages( void );

    //-----------------------------------------------------------------------------
    //  _WndProc
    //  Handles Windows messages
    //-----------------------------------------------------------------------------
    static LRESULT CALLBACK _WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
    /***************************************\
    | class members                         |
    \***************************************/
    HWND    m_hWnd;
};


} // namespace Riot

#endif // #ifndef _WIN32APPLICATION_H_
