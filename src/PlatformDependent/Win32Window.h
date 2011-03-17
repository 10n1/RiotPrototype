/*********************************************************\
File:       Win32Window.h
Purpose:    Creating windows on Windows
\*********************************************************/
#ifndef _WIN32WINDOW_H_
#define _WIN32WINDOW_H_
#include "code\Window.h"

class CWin32Window : public CWindow
{
public:
    // CWin32Window constructor
    CWin32Window();

    // CWin32Window destructor
    ~CWin32Window();
    /***************************************\
    | class methods                         |
    \***************************************/    
    //-----------------------------------------------------------------------------
    //  CreateMainWindow
    //  Creates the main window
    //  TODO: Support multiple windows/views. Eg, the editor
    //-----------------------------------------------------------------------------
    uint CreateMainWindow( uint nWidth, uint nHeight );

    //-----------------------------------------------------------------------------
    //  ProcessMessages
    //  Processes system messages
    //-----------------------------------------------------------------------------
    void ProcessMessages( void );

private:
    /***************************************\
    | class members                         |
    \***************************************/

};


#endif // #ifndef _WIN32WINDOW_H_
