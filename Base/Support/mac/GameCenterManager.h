
#ifdef BASE_PLATFORM_MAC

#import <GameKit/GameKit.h>
//#import <Foundation/Foundation.h>

#include <vector>

// forward declare
@class GKLeaderboard, GKAchievement, GKPlayer;

// delegate
@protocol GameCenterManagerDelegate <NSObject>
@optional
- (void) processGameCenterAuth: (NSError*) error;
- (void) scoreSubmitted: (NSError*) error;
- (void) reloadScoresComplete: (GKLeaderboard*) leaderBoard error: (NSError*) error;
- (void) reloadAchievementsComplete: (NSArray *) achievements error: (NSError*) error;
- (void) achievementSubmitted: (GKAchievement*) ach error:(NSError*) error;
- (void) achievementResetResult: (NSError*) error;
- (void) mappedPlayerIDToPlayer: (GKPlayer*) player error: (NSError*) error;
@end

// main class
@interface GameCenterManager : NSObject <GameCenterManagerDelegate, GKGameCenterControllerDelegate>
{
	NSMutableDictionary* earnedAchievementCache;
	
    bool gameCenterAvailable;
    
	id <GameCenterManagerDelegate, NSObject> __unsafe_unretained delegate;
	
	struct LeaderboardCache
	{
		bool validCache;
		int64_t personalBest;
		NSString* leaderboardCategory;
	};
	
	std::vector<LeaderboardCache> m_Leaderboards;
}

// This property must be attomic to ensure that the cache is always in a viable state...
@property (strong) NSMutableDictionary* earnedAchievementCache;

@property (nonatomic, unsafe_unretained)  id <GameCenterManagerDelegate> delegate;

- (void) clearCache;
- (bool) isPlayerAuthenticated;

- (void) authenticateLocalUser;
- (void) submitScore: (int64_t) score forLeaderboardIdentifier: (NSString*) leaderboardIdentifier;
- (void) reloadHighScoresForLeaderboardIdentifier: (NSString*) leaderboardIdentifier;
- (void) reloadAchievements;
- (void) submitAchievement: (NSString*) identifier percentComplete: (double) percentComplete;
- (void) resetAchievements;
- (void) mapPlayerIDtoPlayer: (NSString*) playerID;

- (std::vector<LeaderboardCache>&) getLeaderboardCache;

@end

bool IsGCAvailable();

#endif // BASE_PLATFORM_MAC
