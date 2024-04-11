
/*===================================================================
 File: AppDelegate.mm
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <OpenGL/OpenGL.h>
//#import <GameController/GameController.h>

#import "Core/mac/OpenGLView.h"
#import "Core/mac/AppDelegate.h"

#include "SupportBase.h"

/// BorderlessWindow - simple inherited class from NSWindow because having a style of NSBorderlessWindowMask causes keyboard input to be ignored
@interface BorderlessWindow : NSWindow
{
}

@end

@implementation BorderlessWindow

/// canBecomeKeyWindow - allows keyboard input on this window
- (BOOL)canBecomeKeyWindow
{
    return YES;
}

/// canBecomeMainWindow - allows keyboard input on this window
- (BOOL)canBecomeMainWindow
{
    return YES;
}

@end

/// AppDelegate - main entry
@implementation AppDelegate

@synthesize window;
@synthesize view;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	NSRect mainDisplayRect = [[NSScreen mainScreen] frame];
	
    NSOpenGLPixelFormatAttribute attr[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy, // Needed if using opengl 3.2 you can comment this line out to use the old version.
        NSOpenGLPFAColorSize,     24,
        NSOpenGLPFAAlphaSize,     8,
		NSOpenGLPFADepthSize,     24,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        0
    };
	
    NSOpenGLPixelFormat *pix = [[NSOpenGLPixelFormat alloc] initWithAttributes:attr];
	//NSOpenGLContext* openGLContext = [[[NSOpenGLContext alloc] initWithFormat:pix shareContext:nil];
	
    view = [[ViewController alloc] initWithFrame:mainDisplayRect pixelFormat:pix];
	
	
    // Below shows how to make the view fullscreen. But you could just add to the contact view of any window.
    window = [[BorderlessWindow alloc] initWithContentRect:mainDisplayRect
                                              styleMask:NSBorderlessWindowMask
                                                backing:NSBackingStoreBuffered
                                                  defer:YES];
	
    window.opaque = YES;
    window.hidesOnDeactivate = YES;
    window.level = NSMainMenuWindowLevel + 1; // Show window above main menu.
    window.contentView = view;
    [window makeKeyAndOrderFront:self]; // Display window.
	
	// need to set up the scores here
	if( support::Scores::IsInitialised() != 0 )
	{
		//support::Scores::GetInstance()->SetViewController( view );
		
		if( core::app::IsNetworkAvailable() )
		{
			support::Scores::GetInstance()->Login();
		}
    }
    
	// Controller
	//[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setupControllers:) name:GCControllerDidConnectNotification object:nil];
	//[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setupControllers:) name:GCControllerDidDisconnectNotification object:nil];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
}

- (void)setupControllers:(NSNotification *)notification
{
    /*if ([[GCController controllers] count] > 0)
	{

		
    }
	else
	{

    }*/
}

@end

#endif // BASE_PLATFORM_MAC
