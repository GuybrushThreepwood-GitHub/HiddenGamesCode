
#ifdef BASE_PLATFORM_MAC

#import <Cocoa/Cocoa.h>
#import <GameKit/GameKit.h>
#import "Core/mac/OpenGLView.h"

@interface ViewController : NSView <GKGameCenterControllerDelegate>
{
	OpenGLView* glView;
    
    GKGameCenterViewController* leaderboardController;
	GKGameCenterViewController* achievements;
}

@property (nonatomic, strong) OpenGLView *glView;
@property (nonatomic, strong) GKGameCenterViewController* leaderboardController;
@property (nonatomic, strong) GKGameCenterViewController* achievements;

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format;

- (void) showLeaderboard: (NSString*) leaderboardIdentifier;
- (void) showAchievements;

@end

#endif // BASE_PLATFORM_MAC
