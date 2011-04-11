/*********************************************************\
File:           OSXApplication.h
Purpose:        Interface for the Cocoa application
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 10:38:41 PM
Modified by:    Kyle Weicht
\*********************************************************/
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#include "common.h"

namespace Riot
{
    class CWindow;
} // namespace Riot

@class COSXDefaultView;

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

//-----------------------------------------------------------------------------
//  CreateWindowWithWidth
//  Creates the main application window
//-----------------------------------------------------------------------------
- (void) CreateWindowWithWidth:(int)nWidth Height:(int)nHeight Fullscreen:(bool)bFullscreen Window:(Riot::CWindow *)pWindow;

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


@end

@interface COSXDefaultView : NSView 
{
@private
    NSApplication*  m_pOwnerApplication;
}

@property (retain) NSApplication* m_pOwnerApplication;

-(void) m_pOwnerApplication:(NSApplication*)pApplication;

@end