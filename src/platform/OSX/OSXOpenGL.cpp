//
//  OSXOpenGL.cpp
//  riot
//
//  Created by Kyle Weicht on 4/11/11.
//  Copyright 2011 none. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#include "SystemOpenGL.h"
#import <OpenGL/OpenGL.h>
#if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_7)
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else
#include <OpenGL/gl.h>
#endif
// Include these for the function lookup:
#import <mach-o/dyld.h>
#import <stdlib.h>
#import <string.h>
#include "OSXApplication.h"

namespace Riot 
{
    namespace SystemOpenGL
    {
        //-----------------------------------------------------------------------------
        //  GL extensions
        //-----------------------------------------------------------------------------        
#define GET_OPENGL_EXTENSION( ext, type ) ext = (type)NSGLGetProcAddress( #ext )
        // Function typedefs
        typedef void (*GLBLENDCOLORPROC)( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
        typedef void (*GLBLENDEQUATIONPROC)( GLenum mode );
        typedef void (*GLDRAWRANGEELEMENTSPROC)( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices );
        
        // Functions
        static GLBLENDCOLORPROC         glBlendColorProc    = NULL;
        static GLBLENDEQUATIONPROC      glBlendEquationProc = NULL;
        static GLDRAWRANGEELEMENTSPROC  glDrawRangeElements = NULL;
        
        // Variables
        static COSXDefaultView*         gs_pView = NULL;
        
        //-----------------------------------------------------------------------------
        //  NSGLGetProcAddress
        //  Looks up the address of the function
        //  Source: OpenGL Programming Guide for OS X
        //-----------------------------------------------------------------------------
        void * NSGLGetProcAddress (const char *name)
        {
            DECLAREPOOL;
            
            NSSymbol symbol;
            char *symbolName;
            symbolName = new char[strlen (name) + 2]; // 1
            strcpy(symbolName + 1, name); // 2
            symbolName[0] = '_'; // 3
            symbol = NULL;
            if (NSIsSymbolNameDefined (symbolName)) // 4
                symbol = NSLookupAndBindSymbol (symbolName);
            
            delete symbolName; // 5
            return symbol ? NSAddressOfSymbol (symbol) : NULL; // 6
            
            RELEASEPOOL;
        }
        
        //-----------------------------------------------------------------------------
        //  LoadOpenGLExtensions
        //  Loads the platform specific OpenGL extentions (wgl, etc)
        //-----------------------------------------------------------------------------
        void LoadOpenGLExtensions( void )
        {
            GET_OPENGL_EXTENSION( glBlendColorProc, GLBLENDCOLORPROC );
            GET_OPENGL_EXTENSION( glBlendEquationProc, GLBLENDEQUATIONPROC );
            GET_OPENGL_EXTENSION( glDrawRangeElements, GLDRAWRANGEELEMENTSPROC );
        }
        
        //-----------------------------------------------------------------------------
        //  CreateOpenGLDevice
        //  Creates the device
        //-----------------------------------------------------------------------------
        Result CreateOpenGLDevice( TOpenGLDevice* pDevice, CWindow* pWindow )
        {
            DECLAREPOOL;
            
            Result nResult = rResultSuccess;
            
            CGLContextObj       pContext = NULL;
            CGLPixelFormatObj   pPixelFormat = NULL;
            
            // Pixel format attributes
            // TODO: Set/change programatically
            CGLPixelFormatAttribute pAttributes[] = 
            {
#if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_7)
                kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
#endif
                kCGLPFAAccelerated,
                kCGLPFANoRecovery,
                kCGLPFADoubleBuffer,   
                kCGLPFAColorSize, (CGLPixelFormatAttribute)kCGL32Bit,
                kCGLPFADepthSize, (CGLPixelFormatAttribute)kCGL24Bit,
                (CGLPixelFormatAttribute)0,
            };
            
            // Obtain the requested pixel format
            GLint nPix = 0;
            CGLChoosePixelFormat( pAttributes, &pPixelFormat, &nPix);
            
            ASSERT( pPixelFormat );
            
            // Create the OpenGL context
            CGLCreateContext( pPixelFormat, NULL, &pContext);
            ASSERT( pContext );
            
            // Turn off VSync
            GLint nSwapInterval = 0;
            CGLSetParameter( pContext, kCGLCPSwapInterval, &nSwapInterval );
            
            // Activate the context
            CGLSetCurrentContext( pContext );
            
            // Load the function pointers
            LoadOpenGLExtensions();
            
            // Let the view know its now OpenGL
            ASSERT(gs_pView);
            [gs_pView SetContext:pContext];
            
            pDevice->m_pGLContext = pContext;
            pDevice->m_pGLPixelFormat = pPixelFormat;
            return nResult;
            
            RELEASEPOOL;
        }
        
        //-----------------------------------------------------------------------------
        //  Present
        //  Flips the buffers
        //-----------------------------------------------------------------------------
        void Present( TOpenGLDevice* pDevice )
        {
            DECLAREPOOL;
            CGLContextObj pContext = (CGLContextObj)pDevice->m_pGLContext;
            
            CGLLockContext( pContext );
            CGLFlushDrawable( pContext );
            CGLUnlockContext( pContext );
            
            RELEASEPOOL;
        }
        
        //-----------------------------------------------------------------------------
        //  SetView
        //  Sets the current view
        //-----------------------------------------------------------------------------
        void SetView( handle pView )
        {
            DECLAREPOOL;
            
            gs_pView = (COSXDefaultView*)pView;
            
            RELEASEPOOL;
        }
    }
}
