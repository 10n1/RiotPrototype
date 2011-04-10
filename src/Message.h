/*********************************************************\
File:           Message.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 3:51:11 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _MESSAGE_H_
#define _MESSAGE_H_

namespace Riot
{

    enum MessageType
    {
        // Main
        mShutdown,

        // Graphics/Window
        mFullscreen,
        mResize,

        // Input
        mKeyboard,
        mMouseMove,
        mMouseButton,

        mNUMMESSAGES
    };

    class IListener;
    
    // Message definition
    struct TMessage
    {
        MessageType nType;
        union
        {
            pvoid       pMessage;
            nativeuint  nMessage;
        };
    };

} // namespace Riot

#endif // #ifndef _MESSAGE_H_
