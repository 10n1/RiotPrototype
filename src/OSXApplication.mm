/*********************************************************\
File:           OSXApplication.mm
Author:         Kyle Weicht
Created:        4/10/2011
Modified:       4/10/2011 11:14:34 PM
Modified by:    Kyle Weicht
\*********************************************************/
#import "OSXApplication.h"
#include "Window.h"
#include "Input.h"
#include "Engine.h"

#import <OpenGL/OpenGL.h>
#if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_7)
#include <OpenGL/gl3.h>
#else
#include <OpenGL/GL.h>
#endif

using namespace Riot;

@implementation COSXApplication

@synthesize m_pDistantPast;
@synthesize m_pView;

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
    [m_pSystemWindow release];
    
    [m_pRunLoop release];
    
    [super dealloc];
}


-(void) finalize
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [super finalize];
}

//-----------------------------------------------------------------------------
//  CreateWindowWithWidth
//  Creates the main application window
//-----------------------------------------------------------------------------
- (handle) CreateWindowWithWidth:(int)nWidth Height:(int)nHeight Fullscreen:(bool)bFullscreen Window:(Riot::CWindow *)pWindow
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

    return m_pSystemWindow;
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

-(id) init
{
    self = [super init];
    if (self) 
    {
        m_pOwnerApplication = NULL;
        m_pContext = NULL;
        
        // Register to handle resizes
        [[NSNotificationCenter defaultCenter]   addObserver:self 
                                                   selector:@selector(reshape) 
                                                       name:NSViewGlobalFrameDidChangeNotification 
                                                     object:self];
    }
    return self;
}

-(id) initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    
    if (self) 
    {
        m_pOwnerApplication = NULL;
        m_pContext = NULL;
        
        // Register to handle resizes
        [[NSNotificationCenter defaultCenter]   addObserver:self 
                                                   selector:@selector(reshape) 
                                                       name:NSViewGlobalFrameDidChangeNotification 
                                                     object:self];
    }
    
    return self;
}


-(void) dealloc
{     
    // Unregister for resize events    
	[[NSNotificationCenter defaultCenter] removeObserver:self 
													name:NSViewGlobalFrameDidChangeNotification
												  object:self];
    [super dealloc];
}

/////////////////////////////////////////////
//  View functions
-(void) lockFocus
{
    [super lockFocus];
    
    if( [m_pContext view] != self )
    {
        [m_pContext setView:self];
    }
    
    [m_pContext makeCurrentContext];
}

-(void) reshape
{    
    DECLAREPOOL;
    NSRect bounds = [self bounds];
    Engine::PostMsg( TMessage( mResize, (((uint)bounds.size.width << 16 ) | (uint)bounds.size.width) ) );
    RELEASEPOOL;
}

-(void) drawRect:(NSRect)dirtyRect
{
}

/////////////////////////////////////////////
//  Misc
-(BOOL) acceptsFirstResponder
{
    return YES;
}

-(void) m_pOwnerApplication:(NSApplication*)pApplication
{
    m_pOwnerApplication = pApplication;
}

-(void) SetContext:(CGLContextObj)pContext
{
    m_pContext = [[NSOpenGLContext alloc] initWithCGLContextObj:pContext]; 
    [m_pContext setView:self];
}


/////////////////////////////////////////////
//  Event handling
-(void) keyDown:(NSEvent *)theEvent
{
    [ m_pOwnerApplication keyDown:theEvent];
}

-(void) keyUp:(NSEvent *)theEvent
{
    [ m_pOwnerApplication keyUp:theEvent];
}


@end
