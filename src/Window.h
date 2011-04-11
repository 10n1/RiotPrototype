/*********************************************************\
File:           Window.h
Purpose:        Main window
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 10:50:17 PM
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
        //  Accessors/Mutators
        //-----------------------------------------------------------------------------
        handle GetWindow( void );
        uint   GetWidth( void );
        uint   GetHeight( void );
    protected:
        /***************************************\
        | class members                         |
        \***************************************/
        handle  m_pWindow;  // Win32:   Win32Application
                            // OS X:    NSApplication*
                            // Linux:   LinuxWindow*??????? <-- 98% sure its called LinuxWindow
        uint    m_nWidth;
        uint    m_nHeight;
    };

} // namespace Riot

#endif // #ifndef _WINDOW_H_
