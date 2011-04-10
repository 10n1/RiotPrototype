/*********************************************************\
File:           Message.h
Purpose:        
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 12:45:29 PM
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
    mFullscreen
    mResize,

    // Input
    mKeyboard,
    mMouseMove,
    mMouseButton,

    mNUMMESSAGES
};

} // namespace Riot

#endif // #ifndef _MESSAGE_H_
