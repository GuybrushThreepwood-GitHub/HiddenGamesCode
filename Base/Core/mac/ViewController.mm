
/*===================================================================
 File: ViewController.mm
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <OpenGL/OpenGL.h>
#import "Core/mac/OpenGLView.h"
#import "Core/mac/ViewController.h"
#import "Core/mac/AppDelegate.h"
#import "Input/Input.h"

@implementation ViewController

@synthesize glView;
@synthesize leaderboardController;
@synthesize achievements;

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
    self = [super init];
    if (self) {
		
		glView = [[OpenGLView alloc] initWithFrame:frameRect pixelFormat:format];
		[self addSubview:glView];
    }
    return self;
}

- (BOOL) acceptsFirstResponder
{
	// want to accept the mouse move
	[[self window] setAcceptsMouseMovedEvents:YES];
	
    // We want this view to be able to receive key events
    return YES;
}

- (void)mouseUp:(NSEvent *)theEvent
{
	if( theEvent.type == NSLeftMouseUp )
	{
		input::gInputState.LButtonPressed = false;
		input::gInputState.LButtonReleased = true;
		
		input::gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::FIRST_TOUCH].Ticks = 0.0f;
		input::gInputState.TouchesData[input::FIRST_TOUCH].bPress = false;
		input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease = true;
		input::gInputState.TouchesData[input::FIRST_TOUCH].bHeld = false;
	}
}

- (void)mouseDown:(NSEvent *)theEvent
{
	NSPoint mouseLocation = [NSEvent mouseLocation];
	
	if( theEvent.type == NSLeftMouseDown )
	{
		input::gInputState.LButtonPressed = true;
		input::gInputState.LButtonReleased = false;
	
		input::gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::FIRST_TOUCH].bPress = true;
		input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease = false;
		input::gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
		
		// current mouse position
		long nCursorMouseX = static_cast<long>(mouseLocation.x);
		long nCursorMouseY = static_cast<long>(mouseLocation.y);
		
		input::gInputState.nMouseX = nCursorMouseX;
		input::gInputState.nMouseY = nCursorMouseY;
		
		// touch
		input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = input::gInputState.nMouseX;
		input::gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = input::gInputState.nMouseY;
		input::gInputState.TouchesData[input::SECOND_TOUCH].nLastTouchX = input::gInputState.nMouseX;
		input::gInputState.TouchesData[input::SECOND_TOUCH].nLastTouchY = input::gInputState.nMouseY;
		input::gInputState.TouchesData[input::SECOND_TOUCH].nXDelta = 0;
		input::gInputState.TouchesData[input::SECOND_TOUCH].nYDelta = 0;
	}
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
	NSPoint mouseLocation = [NSEvent mouseLocation];
	
	if( theEvent.type == NSRightMouseDown )
	{
		input::gInputState.RButtonPressed = true;
		input::gInputState.RButtonReleased = false;
		
		input::gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
		input::gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
		
		// last mouse position
		input::gInputState.nLastMouseX = input::gInputState.nMouseX;
		input::gInputState.nLastMouseY = input::gInputState.nMouseY;
		
		// current mouse position
		long nCursorMouseX = static_cast<long>(mouseLocation.x);
		long nCursorMouseY = static_cast<long>(mouseLocation.y);
		
		input::gInputState.nMouseX = nCursorMouseX;
		input::gInputState.nMouseY = nCursorMouseY;
		
		// mouse delta
		input::gInputState.nMouseXDelta = 0;
		input::gInputState.nMouseYDelta = 0;
		
		// touch
		input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchX = input::gInputState.nMouseX;
		input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchY = input::gInputState.nMouseY;
		input::gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = input::gInputState.nMouseX;
		input::gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = input::gInputState.nMouseY;
		input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = 0;
		input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = 0;
	}
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
	if( theEvent.type == NSRightMouseUp )
	{
		input::gInputState.RButtonPressed = false;
		input::gInputState.RButtonReleased = true;
		
		input::gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].Ticks = 0.0f;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bPress = false;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease = true;
		input::gInputState.TouchesData[input::SECOND_TOUCH].bHeld = false;
	}
}

- (void)mouseMoved:(NSEvent *)theEvent
{
	NSPoint mouseLocation = [NSEvent mouseLocation];
	
	// last mouse position
	input::gInputState.nLastMouseX = input::gInputState.nMouseX;
	input::gInputState.nLastMouseY = input::gInputState.nMouseY;

	// current mouse position
	long nCursorMouseX = static_cast<long>(mouseLocation.x);
	long nCursorMouseY = static_cast<long>(mouseLocation.y);

	input::gInputState.nMouseX = nCursorMouseX;
	input::gInputState.nMouseY = nCursorMouseY;

	// mouse delta
	input::gInputState.nMouseXDelta = input::gInputState.nMouseX - input::gInputState.nLastMouseX;
	input::gInputState.nMouseYDelta = input::gInputState.nMouseY - input::gInputState.nLastMouseY;

	// touch
	input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchX = input::gInputState.nMouseX;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchY = input::gInputState.nMouseY;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = input::gInputState.nLastMouseX;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = input::gInputState.nLastMouseY;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = input::gInputState.nMouseXDelta;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = input::gInputState.nMouseYDelta;

	input::gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec += math::Vec2( static_cast<float>(input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta), static_cast<float>(input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta) );	
}

- (void)mouseDragged:(NSEvent *)theEvent
{
	NSPoint mouseLocation = [NSEvent mouseLocation];
	
	// last mouse position
	input::gInputState.nLastMouseX = input::gInputState.nMouseX;
	input::gInputState.nLastMouseY = input::gInputState.nMouseY;
	
	// current mouse position
	long nCursorMouseX = static_cast<long>(mouseLocation.x);
	long nCursorMouseY = static_cast<long>(mouseLocation.y);
	
	input::gInputState.nMouseX = nCursorMouseX;
	input::gInputState.nMouseY = nCursorMouseY;
	
	// mouse delta
	input::gInputState.nMouseXDelta = input::gInputState.nMouseX - input::gInputState.nLastMouseX;
	input::gInputState.nMouseYDelta = input::gInputState.nMouseY - input::gInputState.nLastMouseY;
	
	// touch
	input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchX = input::gInputState.nMouseX;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nTouchY = input::gInputState.nMouseY;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = input::gInputState.nLastMouseX;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = input::gInputState.nLastMouseY;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = input::gInputState.nMouseXDelta;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = input::gInputState.nMouseYDelta;
	
	input::gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec += math::Vec2( static_cast<float>(input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta), static_cast<float>(input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta) );
}

- (void) keyDown:(NSEvent *)theEvent
{
    //unichar uniChar = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	unsigned short shortChar = [theEvent keyCode];
	
	input::gInputState.KeyStates.bKeys[shortChar] = true;
	//input::gInputState.KeyStates.bKeyPressTime[shortChar] = 0;
	input::gInputState.nKeyPressCode = static_cast<int>(shortChar);
	input::gInputState.bKeyPressed = true;
	
}

- (void) keyUp:(NSEvent *)theEvent
{
    //unichar uniChar = [[theEvent charactersIgnoringModifiers] characterAtIndex:0];
	unsigned short shortChar = [theEvent keyCode];
	
	input::gInputState.KeyStates.bKeys[shortChar] = false;
	input::gInputState.KeyStates.bKeyPressTime[shortChar] = 0;
	input::gInputState.nKeyPressCode = -1;
	input::gInputState.bKeyPressed = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    GKDialogController *sdc = [GKDialogController sharedDialogController];
    
    [sdc dismiss: self];
}

// request to show a leaderboard
- (void) showLeaderboard: (NSString*) leaderboardIdentifier
{
	leaderboardController = [[GKGameCenterViewController alloc] init];
	if (leaderboardController != 0)
	{
		leaderboardController.viewState = GKGameCenterViewControllerStateLeaderboards;
		leaderboardController.leaderboardCategory = leaderboardIdentifier;
		//leaderboardController.gameCenterDelegate = self;
        
        AppDelegate *appDelegate = (AppDelegate *)[[NSApplication sharedApplication] delegate];
        if(appDelegate)
        {
            GKDialogController *sdc = [GKDialogController sharedDialogController];
            sdc.parentWindow = (NSWindow *)appDelegate.window;
            [sdc presentViewController:leaderboardController];
        }
	}
}

// show the game achievement list
- (void) showAchievements
{
	achievements = [[GKGameCenterViewController alloc] init];
	if (achievements != 0)
	{
		//achievements.achievementDelegate = self;
		achievements.viewState = GKGameCenterViewControllerStateAchievements;
		//achievements.gameCenterDelegate = self;
		
        AppDelegate *appDelegate = (AppDelegate *)[[NSApplication sharedApplication] delegate];
        if(appDelegate)
        {
            GKDialogController *sdc = [GKDialogController sharedDialogController];
            sdc.parentWindow = (NSWindow *)appDelegate.window;
            [sdc presentViewController:achievements];
        }
	}
}

@end

#endif // BASE_PLATFORM_MAC

