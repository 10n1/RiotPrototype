/*********************************************************\
File:       Window.h
Purpose:    Abstration for creating a platform-independent
            window
\*********************************************************/
#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "Types.h"
#include "IRefCounted.h"

class CWindow : public IRefCounted
{
public:
    // CWindow constructor
    CWindow();

    // CWindow destructor
    virtual ~CWindow();
    /***************************************\
    | class methods                         |
    \***************************************/
    //-----------------------------------------------------------------------------
    //  CreateMainWindow
    //  Creates the main window
    //  TODO: Support multiple windows/views. Eg, the editor
    //-----------------------------------------------------------------------------
    virtual uint CreateMainWindow( uint nWidth, uint nHeight ) = 0;

    //-----------------------------------------------------------------------------
    //  ProcessMessages
    //  Processes system messages
    //-----------------------------------------------------------------------------
    virtual void ProcessMessages( void ) = 0;

    //-----------------------------------------------------------------------------
    //  Accessors/Mutators
    //-----------------------------------------------------------------------------
    handle GetSystemWindow( void );
    uint   GetWidth( void );
    uint   GetHeight( void );
protected:
    /***************************************\
    | class members                         |
    \***************************************/
    handle  m_pSystemWindow;    // Win32:   HWND
                                // OS X:    NSWindow*
                                // Linux:   LinuxWindow*??????? <-- 98% sure its called LinuxWindow
    uint    m_nWidth;
    uint    m_nHeight;
};


#endif // #ifndef _WINDOW_H_
