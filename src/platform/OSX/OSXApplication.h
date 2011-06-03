/*********************************************************\
File:           OSXApplication.h
Purpose:        Interface for the Cocoa application
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:14:33 PM
Modified by:    Kyle Weicht
\*********************************************************/
#ifndef _OSXAPPLICATION_H_
#define _OSXAPPLICATION_H_
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#include "common.h"
#include "SystemOpenGL.h"

namespace Riot
{
    class CWindow;
} // namespace Riot

@class COSXDefaultView;

//////////////////////////////////////////
// The default OS X application
@interface COSXApplication : NSApplication 
{
@private
    NSDate*     m_pDistantFuture;
    NSDate*     m_pDistantPast;    
    NSWindow*   m_pSystemWindow;
    NSRunLoop*  m_pRunLoop;
    
    COSXDefaultView*    m_pView;
    
    Riot::CWindow*      m_pMainWindow;
}

@property (retain) NSDate* m_pDistantPast;
@property (retain) COSXDefaultView* m_pView;

//-----------------------------------------------------------------------------
//  CreateWindowWithWidth
//  Creates the main application window
//-----------------------------------------------------------------------------
- (handle) CreateWindowWithWidth:(int)nWidth Height:(int)nHeight Fullscreen:(bool)bFullscreen Window:(Riot::CWindow *)pWindow;

//-----------------------------------------------------------------------------
//  ProcessOSMessages
//  Handles Windows messages
//-----------------------------------------------------------------------------
- (void) ProcessOSMessages;

//-----------------------------------------------------------------------------
//  KeyDown/Up
//  Handles key down/up messages
//-----------------------------------------------------------------------------
-(void) keyDown:(NSEvent *)theEvent;
-(void) keyUp:(NSEvent *)theEvent;
-(void) mouseDown:(NSEvent *)theEvent;
-(void) mouseUp:(NSEvent *)theEvent;
-(void) mouseMoved:(NSEvent *)theEvent;


@end


//////////////////////////////////////////
//  Our view
@interface COSXDefaultView : NSView 
{
@private
    NSApplication*      m_pOwnerApplication;
    NSOpenGLContext*    m_pContext;    
}

@property (retain) NSApplication* m_pOwnerApplication;

-(void) m_pOwnerApplication:(NSApplication*)pApplication;
-(void) SetContext:(CGLContextObj)pContext;

@end


#endif // #ifndef _OSXAPPLICATION_H_
