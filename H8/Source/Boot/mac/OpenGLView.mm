
/*===================================================================
 File: OpenGLView.mm
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import "Core/mac/OpenGLView.h"

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "StateManage/IState.h"
#include "StateManage/StateManager.h"
#include "H8.h"
#include "H8Consts.h"

H8 app;

namespace
{
	bool useDisplayLink = false;
	
	CFAbsoluteTime prevTime;
	CFAbsoluteTime curTime;
	CVDisplayLinkRef displayLink;
	NSTimer* renderTimer;
}

// This is the callback function for the display link.
static CVReturn DisplayLinkCallback(CVDisplayLinkRef displayLink,
									const CVTimeStamp* now,
									const CVTimeStamp* outputTime,
									CVOptionFlags flagsIn,
									CVOptionFlags *flagsOut,
									void *displayLinkContext)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	OpenGLView *view = (OpenGLView*)displayLinkContext;
	
	[view.openGLContext makeCurrentContext];
	
	CGLLockContext((CGLContextObj)view.openGLContext.CGLContextObj); // This is needed because this isn't running on the main thread.
	
	curTime = CFAbsoluteTimeGetCurrent();
	//CFTimeInterval delta = curTime - prevTime;
	
	// Draw the scene. This doesn't need to be in the drawRect method.
	[view drawRect:view.bounds];
	
	CGLUnlockContext((CGLContextObj)view.openGLContext.CGLContextObj);
	
	CGLFlushDrawable((CGLContextObj)view.openGLContext.CGLContextObj); // This does glFlush() for you.
	
	prevTime = curTime;
	
	[pool release];

	return kCVReturnSuccess;
}

/// OpenGLView - OpenGL handler
@implementation OpenGLView

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
	app.SetScreenWidth(frameRect.size.width);
	app.SetScreenHeight(frameRect.size.height);
	
	return [super initWithFrame:frameRect pixelFormat:format];
}

- (void) dealloc
{
	[super dealloc];
	
	app.Destroy();
	
	if( useDisplayLink )
	{
		// Stop and release the display link
		CVDisplayLinkStop(displayLink);
		CVDisplayLinkRelease(displayLink);
	}
}

- (void)reshape
{
	//[super reshape];
    
	CGLLockContext((CGLContextObj)self.openGLContext.CGLContextObj);
	
	[self.openGLContext flushBuffer];
	
    CGLUnlockContext((CGLContextObj)self.openGLContext.CGLContextObj);
}

- (void) drawRect:(NSRect)dirtyRect
{
	//[super drawRect:dirtyRect];
	
	[self.openGLContext makeCurrentContext];
	CGLLockContext((CGLContextObj)self.openGLContext.CGLContextObj);
		
	app.Execute();
	
	[self.openGLContext flushBuffer];
	
    CGLUnlockContext((CGLContextObj)self.openGLContext.CGLContextObj);	
}

- (void)prepareOpenGL
{
    [super prepareOpenGL];
    [self.openGLContext makeCurrentContext];
	
	curTime = CFAbsoluteTimeGetCurrent();
	prevTime = curTime;
	
	GLint swapInt = 1;
    [self.openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
    CGLLockContext((CGLContextObj)self.openGLContext.CGLContextObj);
	
	app.Run();
	
	[self.openGLContext flushBuffer];
	
    CGLUnlockContext((CGLContextObj)self.openGLContext.CGLContextObj);
	
	if( useDisplayLink )
	{
		// Below creates the display link and tell it what function to call when it needs to draw a frame.
		CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
		displayLink = CVDisplayLinkRetain(displayLink);
		CVDisplayLinkSetOutputCallback(displayLink, &DisplayLinkCallback, ( void *)self);
		CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,
														  (CGLContextObj)self.openGLContext.CGLContextObj,
														  (CGLPixelFormatObj)self.pixelFormat.CGLPixelFormatObj);
		CVDisplayLinkStart(displayLink);
	}
	else
	{
		/*renderTimer = [[[NSTimer alloc] initWithFireDate:0.0
		 interval:0.0016f // must use with vbsynch on, or you waste lots of CPU!
		 target:self
		 selector:@selector(renderTimerCallback:)
		 userInfo:nil
		 repeats:YES] retain];*/
		
		renderTimer = [NSTimer scheduledTimerWithTimeInterval:core::app::GetFrameLock()
													   target:self
													 selector:@selector(renderTimerCallback:)
													 userInfo:nil
													  repeats:YES];
		
		[[NSRunLoop mainRunLoop] addTimer:renderTimer forMode:NSEventTrackingRunLoopMode];
		[[NSRunLoop mainRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
		[[NSRunLoop mainRunLoop] addTimer:renderTimer forMode:NSModalPanelRunLoopMode];
		
		//[renderTimer release];
	}
	
}

// similar to above, except called by regular timer, not display link
- (void)renderTimerCallback:(NSTimer*)theTimer
{
    // lets the OS call drawRect for best window system synchronization
    [self setNeedsDisplay:YES];
}

@end

#endif // BASE_PLATFORM_MAC

