/*********************************************************\
File:           OSXApplication.mm
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 10:38:42 PM
Modified by:    Kyle Weicht
\*********************************************************/
#import "OSXApplication.h"
#include "Window.h"
#include "Input.h"
#include "Engine.h"

using namespace Riot;

@implementation COSXApplication

@synthesize m_pDistantPast;

- (id)init
{
    self = [super init];
    if (self) 
    {
        m_pDistantFuture    = NULL;
        m_pDistantPast      = NULL;    
        m_pSystemWindow     = NULL;
        m_pRunLoop          = NULL;
        
        m_pView         = NULL;        
        m_pMainWindow   = NULL;

        // Initalize distant dates
        m_pDistantFuture = [[NSDate distantFuture] retain];
        m_pDistantPast = [[NSDate distantPast] retain];    
        
        // Get the run loop
        m_pRunLoop = [[NSRunLoop currentRunLoop] retain];
        
        // Finish launching
        [self finishLaunching];
        _running = 1;
    }
    
    return self;
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [m_pDistantPast release];
    [m_pDistantFuture release];
    
    [m_pRunLoop release];
    
    [super dealloc];
}

//-----------------------------------------------------------------------------
//  CreateWindowWithWidth
//  Creates the main application window
//-----------------------------------------------------------------------------
- (void) CreateWindowWithWidth:(int)nWidth Height:(int)nHeight Fullscreen:(bool)bFullscreen Window:(Riot::CWindow *)pWindow
{
    ASSERT( m_pSystemWindow == NULL );

    DECLAREPOOL;    
    
    NSRect windowRect;
    NSUInteger nWindowStyleMask;
    
    if( bFullscreen )
    {
        nWindowStyleMask = NSBorderlessWindowMask;  // <-- For fullscreen
        windowRect = [[NSScreen mainScreen] frame]; // <-- for fullscreen
    }
    else
    {
        // Do some math to move the window to the top right corner
        NSRect fullscreenRect = [[NSScreen mainScreen] frame];
        int nOriginX = 0;
        int nOriginY = fullscreenRect.origin.x + (int)fullscreenRect.size.height;
        windowRect = NSMakeRect(nOriginX, nOriginY, nWidth, nHeight);
        nWindowStyleMask = NSTitledWindowMask | NSResizableWindowMask;
    }
    
    // Create the window
    m_pSystemWindow = [[NSWindow alloc] initWithContentRect:windowRect
                                                  styleMask:nWindowStyleMask 
                                                    backing:NSBackingStoreBuffered 
                                                      defer:YES];
    
    m_pMainWindow = pWindow;
    
    m_pView = [[COSXDefaultView alloc] initWithFrame:NSMakeRect(0, 0, nWidth, nHeight)];
    [m_pView m_pOwnerApplication:self];
    [m_pSystemWindow setContentView:m_pView];
    [m_pSystemWindow makeKeyAndOrderFront:self];
    
    RELEASEPOOL;
}

//-----------------------------------------------------------------------------
//  ProcessOSMessages
//  Handles Windows messages
//-----------------------------------------------------------------------------
- (void) ProcessOSMessages
{
    DECLAREPOOL;
    for( ;; )
    {
        NSEvent* pEvent = [NSApp nextEventMatchingMask:NSAnyEventMask 
                                             untilDate:[NSApp m_pDistantPast] 
                                                inMode:NSDefaultRunLoopMode 
                                               dequeue:YES];

        if( pEvent == nil )
            break;

        [self sendEvent:pEvent];

        if( ![self isRunning] )
            break;

        [pAPool release];
        pAPool = [[NSAutoreleasePool alloc] init];
    }
    RELEASEPOOL;
}

//-----------------------------------------------------------------------------
//  KeyDown/Up
//  Handles key down/up messages
//-----------------------------------------------------------------------------
-(void) keyDown:(NSEvent *)theEvent
{    
    char c = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
    
    Engine::PostMsg( TMessage( mHardwareKeyboardDown, c ) );
}

-(void) keyUp:(NSEvent *)theEvent
{
    char c = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
    
    Engine::PostMsg( TMessage( mHardwareKeyboardUp, c ) );
}

@end

@implementation COSXDefaultView
     
@synthesize m_pOwnerApplication;

-(BOOL) acceptsFirstResponder
{
    return YES;
}

-(void) m_pOwnerApplication:(NSApplication*)pApplication
{
    m_pOwnerApplication = pApplication;
}

-(void) keyDown:(NSEvent *)theEvent
{
    [ m_pOwnerApplication keyDown:theEvent];
}

-(void) keyUp:(NSEvent *)theEvent
{
    [ m_pOwnerApplication keyUp:theEvent];
}


@end