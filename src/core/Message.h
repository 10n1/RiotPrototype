/*********************************************************\
File:           Message.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/17/2011 4:24:34 PM
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

        // Hardware Input
        mHardwareKeyboardDown,
        mHardwareKeyboardUp,
        mHardwareMouseMove,
        mHardwareMouseButton,

        // "Processed" input
        mKeyPressed,
        mKeyDown,
        mKeyUp,
        mMouseButtonDown,
        mMouseButtonPressed,
        mMouseMove,

        mNUMMESSAGES,
        mNULLMESSAGE = -1
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

        TMessage() { }
        TMessage( MessageType type )
            : nType( type )
            , nMessage( 0 )
        {
        }
        TMessage( MessageType type, uint msg )
            : nType( type )
            , nMessage( msg )
        {
        }
        TMessage( MessageType type, pvoid msg )
            : nType( type )
            , pMessage( msg )
        {
        }
    };

} // namespace Riot

#endif // #ifndef _MESSAGE_H_
