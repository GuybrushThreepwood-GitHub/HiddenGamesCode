
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
#include "Cabby.h"
#include "CabbyConsts.h"

const float REMOTE_XAXIS_DEAD_ZONE = 0.35f;
const float REMOTE_YAXIS_DEAD_ZONE = 0.2f;

Cabby app;

namespace
{
	bool useDisplayLink = false;
	
	CFAbsoluteTime prevTime;
	CFAbsoluteTime curTime;
	CVDisplayLinkRef displayLink;
	NSTimer* renderTimer;
    
    bool gameCenterActive;
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
    gameCenterActive = false;
    
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
	
    bool upHandled = false;
    bool downHandled = false;
    bool leftHandled = false;
    bool rightHandled = false;
    bool aHandled = false;
    bool bHandled = false;
    bool xHandled = false;
    
    if( ([[GCController controllers] count] > 0) &&
       !gameCenterActive )
    {
        for( GCController* controller in [GCController controllers] )
        {
            if( controller.extendedGamepad )
            {
                if(controller.extendedGamepad.dpad.up.value >= REMOTE_YAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.yAxis.value >= REMOTE_YAXIS_DEAD_ZONE )
                {
                    input::gInputState.nKeyPressCode = input::KEY_UPARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    upHandled = true;
                }
                
                if(controller.extendedGamepad.dpad.down.value >= REMOTE_YAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.yAxis.value <= -REMOTE_YAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_DOWNARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    downHandled = true;
                }
                
                if(controller.extendedGamepad.dpad.left.value >= REMOTE_XAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.xAxis.value <= -REMOTE_XAXIS_DEAD_ZONE )
                {
                    input::gInputState.nKeyPressCode = input::KEY_LEFTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    leftHandled = true;
                }
                
                if(controller.extendedGamepad.dpad.right.value >= REMOTE_XAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.xAxis.value >= REMOTE_XAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_RIGHTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    rightHandled = true;
                }
                
                if(controller.extendedGamepad.buttonA.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_ENTER;
                    input::gInputState.KeyStates.bKeys[input::KEY_ENTER] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    input::gInputState.nKeyPressCode = input::KEY_SPACE;
                    input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    aHandled = true;
                }
                
                /*if(controller.extendedGamepad.buttonB.pressed)
                 {
                 input::gInputState.nKeyPressCode = input::KEY_SPACE;
                 input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = true;
                 input::gInputState.bKeyPressed = true;
                 
                 bHandled = true;
                 }*/
                
                if(controller.extendedGamepad.buttonX.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_P;
                    input::gInputState.KeyStates.bKeys[input::KEY_P] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    xHandled = true;
                }
            }
            else
            if( controller.gamepad )
            {
                if(controller.gamepad.dpad.up.value >= REMOTE_YAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_UPARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    upHandled = true;
                }
                
                if(controller.gamepad.dpad.down.value >= REMOTE_YAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_DOWNARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    downHandled = true;
                }
                
                if(controller.gamepad.dpad.left.value >= REMOTE_XAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_LEFTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    leftHandled = true;
                }
                
                if(controller.gamepad.dpad.right.value >= REMOTE_XAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_RIGHTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    rightHandled = true;
                }
                
                if(controller.gamepad.buttonA.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_ENTER;
                    input::gInputState.KeyStates.bKeys[input::KEY_ENTER] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    input::gInputState.nKeyPressCode = input::KEY_SPACE;
                    input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    aHandled = true;
                }
                
                if(controller.gamepad.buttonX.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_P;
                    input::gInputState.KeyStates.bKeys[input::KEY_P] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    xHandled = true;
                }
            }
        }
    }
    else
    {
        
    }
    
    if( !upHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_UPARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !downHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_DOWNARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !leftHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_LEFTARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !rightHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_RIGHTARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !aHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_ENTER] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_ENTER] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_ENTER] = false;
        
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_SPACE] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    /*if( !bHandled &&
     input::gInputState.KeyStates.bKeys[input::KEY_SPACE] )
     {
     input::gInputState.KeyStates.bKeyPressTime[input::KEY_SPACE] = 0;
     input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = false;
     
     input::gInputState.nKeyPressCode = -1;
     input::gInputState.bKeyPressed = false;
     }*/
    
    if( !xHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_P] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_P] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_P] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
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
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gameControllerDidConnect:) name:GCControllerDidConnectNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gameControllerDidDisconnect:) name:GCControllerDidDisconnectNotification object:nil];
    
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:^(){
        NSLog(@"Wireless Discovery complete");
    }];
}

// similar to above, except called by regular timer, not display link
- (void)renderTimerCallback:(NSTimer*)theTimer
{
    // lets the OS call drawRect for best window system synchronization
    [self setNeedsDisplay:YES];
}

-(void)gameControllerDidConnect:(NSNotification *)notification
{
    NSLog( @"%lu Controllers attached", (unsigned long)[[GCController controllers] count] );
    /*if ( [[GCController controllers] count] > 0) {
     
     for( GCController* controller in [GCController controllers] )
     {
     if( tvRemote == nil &&
     controller.microGamepad != nil )
     {
     tvRemote = controller.microGamepad;
     tvRemote.allowsRotation = NO;
     //[GCController stopWirelessControllerDiscovery];
     }
     }
     }*/
}

-(void)gameControllerDidDisconnect:(NSNotification *)notification
{
    if ([[GCController controllers] count] == 0) {
        
        //tvRemote = nil;
    }
}

@end

#endif // BASE_PLATFORM_MAC

