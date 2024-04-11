
/*===================================================================
	File: GameCenterManager.mm
	Library: Support

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <GameKit/GameKit.h>
#include "Core/mac/AppDelegate.h"

#include "CoreBase.h"
#include "Support/Scores.h"

#import "GameCenterManager.h"

@implementation GameCenterManager

@synthesize earnedAchievementCache;
@synthesize delegate;

/////////////////////////////////////////////////////
/// Method: init
/// Returns: None 
///
/////////////////////////////////////////////////////
- (id) init
{
	self = [super init];
	if(self != nil)
	{
		earnedAchievementCache=nil;
	}
    
	return self;
}

// NOTE:  GameCenter does not guarantee that callback blocks will be execute on the main thread. 
// As such, your application needs to be very careful in how it handles references to view
// controllers.  If a view controller is referenced in a block that executes on a secondary queue,
// that view controller may be released (and dealloc'd) outside the main queue.  This is true
// even if the actual block is scheduled on the main thread.  In concrete terms, this code
// snippet is not safe, even though viewController is dispatching to the main queue:
//
//	[object doSomethingWithCallback:  ^()
//	{
//		dispatch_async(dispatch_get_main_queue(), ^(void)
//		{
//			[viewController doSomething];
//		});
//	}];
//
// UIKit view controllers should only be accessed on the main thread, so the snippet above may
// lead to subtle and hard to trace bugs.  Many solutions to this problem exist.  In this sample,
// I'm bottlenecking everything through  "callDelegateOnMainThread" which calls "callDelegate". 
// Because "callDelegate" is the only method to access the delegate, I can ensure that delegate
// is not visible in any of my block callbacks.

- (void) callDelegate: (SEL) selector withArg: (id) arg error: (NSError*) err
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Warc-performSelector-leaks"
	assert([NSThread isMainThread]);
	if([delegate respondsToSelector: selector])
	{
		if(arg != nil)
		{
			[delegate performSelector: selector withObject: arg withObject: err];
		}
		else
		{
			[delegate performSelector: selector withObject: err];
		}
	}
	else
	{
		NSLog(@"Missed Method");
	}
#pragma clang diagnostic pop
}

/////////////////////////////////////////////////////
/// Method: callDelegateOnMainThread
/// Returns: None 
///
/////////////////////////////////////////////////////
- (void) callDelegateOnMainThread: (SEL) selector withArg: (id) arg error: (NSError*) err
{
	dispatch_async(dispatch_get_main_queue(), ^(void)
	{
	   [self callDelegate: selector withArg: arg error: err];
	});
}

/////////////////////////////////////////////////////
/// Method: clearCache
/// Returns: None 
///
/////////////////////////////////////////////////////
- (void) clearCache
{
	m_Leaderboards.clear();
}

/////////////////////////////////////////////////////
/// Method: isPlayerAuthenticated
/// Returns: boolean 
///
/////////////////////////////////////////////////////
- (bool) isPlayerAuthenticated
{
    if( IsGCAvailable() )
        return [GKLocalPlayer localPlayer].authenticated;
    else
        return false;
}

/////////////////////////////////////////////////////
/// Method: authenticateLocalUser
/// Params: None 
///
/////////////////////////////////////////////////////
- (void) authenticateLocalUser
{    
	if([GKLocalPlayer localPlayer].authenticated == NO)
	{
        
        [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error){
            if( error == nil )
            {
                DBGLOG( "GAMECENTERMANANGER: GameCenter authenticated\n" );
            }
            else
            {
                DBGLOG( "GAMECENTERMANANGER: GameCenter could not authenticate local user\n" );
            }
        }];
        
		/*[[GKLocalPlayer localPlayer] setAuthenticateHandler:(^(NSViewController* viewController, NSError *error) {

			if(error == nil)
			{
				// send back to the game
				support::Scores::GetInstance()->SetLoggedIn();
				
				if( viewController )
				{
					AppDelegate *appDelegate = (AppDelegate *)[[NSApplication sharedApplication] delegate];
					if(appDelegate)
					{
                        GKDialogController *sdc = [GKDialogController sharedDialogController];
                        
                        sdc.parentWindow = (NSWindow *)appDelegate.window;
                        
                        [sdc presentViewController:(NSViewController<GKViewController>*)viewController];
					}
				}
				
				[self reloadAchievements];
				
				//[self resetAchievements];
				
				// player is authenticated
				//[self reloadHighScoresForCategory: self.currentLeaderBoard];
			}
			else
			{
                DBGLOG( "GAMECENTERMANANGER: GameCenter could not authenticate local user\n" );
			}
		})];*/
	}
}

/////////////////////////////////////////////////////
/// Method: reloadHighScoresForLeaderboardIdentity
/// Params: category 
///
/////////////////////////////////////////////////////
- (void) reloadHighScoresForLeaderboardIdentifier: (NSString*) leaderboardIdentifier
{
	GKLeaderboard* leaderBoard= [[GKLeaderboard alloc] init];
	leaderBoard.category = leaderboardIdentifier;
	leaderBoard.timeScope = GKLeaderboardTimeScopeAllTime;
	leaderBoard.range = NSMakeRange(1, 1);
	
	[leaderBoard loadScoresWithCompletionHandler:  ^(NSArray *scores, NSError *error)
	{
		[self callDelegateOnMainThread: @selector(reloadScoresComplete:error:) withArg: leaderBoard error: error];
	}];
}

/////////////////////////////////////////////////////
/// Method: reloadAchievements
/// Params: None 
///
/////////////////////////////////////////////////////
- (void) reloadAchievements
{    
	[GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *error) 
	{
		[self callDelegateOnMainThread: @selector(reloadAchievementsComplete:error:) withArg: achievements error: error];
	}];
}

/////////////////////////////////////////////////////
/// Method: submitScore
/// Params: [in]score, [in]leaderboardIdentifier
///
/////////////////////////////////////////////////////
- (void) submitScore: (int64_t) score forLeaderboardIdentifier: (NSString*) leaderboardIdentifier
{
	GKScore *scoreReporter = [[GKScore alloc] initWithCategory:leaderboardIdentifier];
	scoreReporter.value = score;
	
	[GKScore reportScores:@[scoreReporter] withCompletionHandler:^(NSError *error) {
		[self callDelegateOnMainThread: @selector(scoreSubmitted:) withArg:nil error: error];
	}];
}

/////////////////////////////////////////////////////
/// Method: submitAchievement
/// Params: [in]identifier, [in]percentComplete 
///
/////////////////////////////////////////////////////
- (void) submitAchievement: (NSString*) identifier percentComplete: (double) percentComplete
{
	//GameCenter check for duplicate achievements when the achievement is submitted, but if you only want to report 
	// new achievements to the user, then you need to check if it's been earned 
	// before you submit.  Otherwise you'll end up with a race condition between loadAchievementsWithCompletionHandler
	// and reportAchievementWithCompletionHandler.  To avoid this, we fetch the current achievement list once,
	// then cache it and keep it updated with any new achievements.
	if(self.earnedAchievementCache == nil)
	{
		[GKAchievement loadAchievementsWithCompletionHandler: ^(NSArray *scores, NSError *error)
		{
			if(error == nil)
			{
				NSMutableDictionary* tempCache= [NSMutableDictionary dictionaryWithCapacity: [scores count]];
				for (GKAchievement* score in scores)
				{
					[tempCache setObject: score forKey: score.identifier];
				}
				self.earnedAchievementCache= tempCache;
				[self submitAchievement: identifier percentComplete: percentComplete];
			}
			else
			{
				//Something broke loading the achievement list.  Error out, and we'll try again the next time achievements submit.
				[self callDelegateOnMainThread: @selector(achievementSubmitted:error:) withArg: nil error: error];
			}

		}];
	}
	else
	{
		 //Search the list for the ID we're using...
		GKAchievement* achievement = [self.earnedAchievementCache objectForKey: identifier];
		
		if(achievement != nil)
		{
			if((achievement.percentComplete >= 100.0) || (achievement.percentComplete >= percentComplete))
			{
				//Achievement has already been earned so we're done.
				achievement=nil; // can comment this out if it should always re-unlock
			}
			achievement.percentComplete = percentComplete;
		}
		else
		{
			achievement= [[GKAchievement alloc] initWithIdentifier: identifier];
			achievement.percentComplete= percentComplete;
			//Add achievement to achievement cache...
			[self.earnedAchievementCache setObject: achievement forKey: achievement.identifier];
		}
		
		if(achievement != nil)
		{
			//Submit the Achievement...
			
			[GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError *error) {
				[self callDelegateOnMainThread: @selector(achievementSubmitted:error:) withArg: achievement error: error];
			}];
		}
	}
}

/////////////////////////////////////////////////////
/// Method: resetAchievements
/// Params: None
///
/////////////////////////////////////////////////////
- (void) resetAchievements
{
	self.earnedAchievementCache = nil;
	[GKAchievement resetAchievementsWithCompletionHandler: ^(NSError *error) 
	{
		 [self callDelegateOnMainThread: @selector(achievementResetResult:) withArg:nil error: error];
	}];
}

/////////////////////////////////////////////////////
/// Method: mapPlayerIDtoPlayer
/// Params: [in]playerID
///
/////////////////////////////////////////////////////
- (void) mapPlayerIDtoPlayer: (NSString*) playerID
{
	[GKPlayer loadPlayersForIdentifiers: [NSArray arrayWithObject: playerID] withCompletionHandler:^(NSArray *playerArray, NSError *error)
	{
		GKPlayer* player = nil;
		for (GKPlayer* tempPlayer in playerArray)
		{
			if([tempPlayer.playerID isEqualToString: playerID])
			{
				player= tempPlayer;
				break;
			}
		}
		[self callDelegateOnMainThread: @selector(mappedPlayerIDToPlayer:error:) withArg: player error: error];
	}];
	
}

/////////////////////////////////////////////////////
/// Method: getLeaderboardCache
/// Params: [in]playerID
///
/////////////////////////////////////////////////////
- (std::vector<LeaderboardCache>&) getLeaderboardCache
{
	return m_Leaderboards;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// Method: mappedPlayerIDToPlayer
/// Params: [in]player, [in]error
///
/////////////////////////////////////////////////////
- (void) mappedPlayerIDToPlayer: (GKPlayer*) player error: (NSError*) error
{

}

/////////////////////////////////////////////////////
/// Method: reloadScoresComplete
/// Params: [in]leaderBoard, [in]error
///
/////////////////////////////////////////////////////
- (void) reloadScoresComplete: (GKLeaderboard*) leaderBoard error: (NSError*) error
{
	if(error == nil)
	{
		// send back to the game
		support::Scores::GetInstance()->ScoreRecieved( [leaderBoard.category cStringUsingEncoding:NSASCIIStringEncoding], leaderBoard.localPlayerScore.value );
	}
	else
	{
	}
}

/////////////////////////////////////////////////////
/// Method: reloadAchievementsComplete
/// Params: [in]leaderBoard, [in]error
///
/////////////////////////////////////////////////////
- (void) reloadAchievementsComplete: (NSArray*) achievements error: (NSError*) error
{
	if (error != nil)
	{
		// handle errors
		return;
	}

	if (achievements != nil)
	{
		for (GKAchievement* achievement in achievements)
		{
			// process the array of achievements.
			if( support::Scores::IsInitialised() )
			{
				if( achievement != nil )
					support::Scores::GetInstance()->AchievementReloaded( achievement.percentComplete, [achievement.identifier cStringUsingEncoding:NSASCIIStringEncoding]);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: scoreSubmitted
/// Params: [in]error
///
/////////////////////////////////////////////////////
- (void) scoreSubmitted: (NSError*) error
{
	if(error == nil)
	{

	}
	else
	{

	}
}

/////////////////////////////////////////////////////
/// Method: achievementSubmitted
/// Params: [in]ach, [in]error
///
/////////////////////////////////////////////////////
- (void) achievementSubmitted: (GKAchievement*)ach error:(NSError*) error
{
	if((error == nil) &&
	   (ach != nil) )
	{
		support::Scores::GetInstance()->AchievementSubmitted( [ach.identifier cStringUsingEncoding:NSASCIIStringEncoding], 0 );
	}
	else
	{

	}
}

/////////////////////////////////////////////////////
/// Method: achievementResetResult
/// Params: [in]error
///
/////////////////////////////////////////////////////
- (void) achievementResetResult: (NSError*) error
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    GKDialogController *sdc = [GKDialogController sharedDialogController];
    
    [sdc dismiss: self];
}

@end

/////////////////////////////////////////////////////
/// Function: IsGCAvailable
/// Params: None
///
/////////////////////////////////////////////////////
bool IsGCAvailable()
{
	return true;
}


#endif // BASE_PLATFORM_MAC
