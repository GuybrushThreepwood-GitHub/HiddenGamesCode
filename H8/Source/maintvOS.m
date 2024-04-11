
/*===================================================================
 File: maintvOS.m
 Game: Cabby
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_tvOS

#import <UIKit/UIKit.h>

#import "Core/App.h"
#import "Core/ios/AppDelegate.h"

int main(int argc, char *argv[])
{
	// cabby will support ipod music
	core::app::SupportiPodMusic(false);
    core::app::SupportPortrait(false);
    core::app::SupportLandscape(true);

	core::app::SetFrameLock( core::FPS60 );

    @autoreleasepool {
        int retVal = UIApplicationMain(argc, argv, nil, @"AppDelegate");
        return retVal;
    }
}

#endif // BASE_PLATFORM_tvOS