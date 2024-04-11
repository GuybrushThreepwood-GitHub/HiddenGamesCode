
/*===================================================================
 File: GameViewController.h
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#ifndef __GAMEVIEWCONTROLLER_H__
#define __GAMEVIEWCONTROLLER_H__

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@interface GameViewController : GLKViewController
{
    @private
        // touch
        CGPoint location;
        float m_Scale;
}

- (void) goToPause;
- (void) undoPause;

- (void) showLeaderboard: (NSString*) leaderboardIdentifier;
- (void) showAchievements;

- (void) showAlert:(NSString*)title message:(NSString*)message;

@end

#endif // __GAMEVIEWCONTROLLER_H__
