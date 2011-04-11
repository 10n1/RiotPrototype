/*********************************************************\
File:           Window.h
Purpose:        Main window
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 5:14:47 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "common.h"
#include "IRefCounted.h"

namespace Riot
{
    class CWindow : public IRefCounted
    {
        friend class System;
    public:
        // CWindow constructor
        CWindow();

        // CWindow destructor
        virtual ~CWindow();
        /***************************************\
        | class methods                         |
        \***************************************/

        //-----------------------------------------------------------------------------
        //  ProcessMessages
        //  Processes system messages
        //-----------------------------------------------------------------------------
        void ProcessMessages( void );

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

} // namespace Riot

#endif // #ifndef _WINDOW_H_
