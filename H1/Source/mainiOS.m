
/*===================================================================
 File: mainiOS.m
 Game: AirCadets
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import <UIKit/UIKit.h>

#import "Core/App.h"
#import "Core/CoreConsts.h"
#import "Core/ios/AppDelegate.h"

int main(int argc, char *argv[])
{
	// aircadets will support ipod music
	core::app::SupportiPodMusic(true);
    core::app::SupportPortrait(false);
    core::app::SupportLandscape(true);
    
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/air-cadets/id298873144?mt=8

	core::app::SetFrameLock( core::FPS30 );
	
    @autoreleasepool {
        int retVal = UIApplicationMain(argc, argv, nil, @"AppDelegate");
        return retVal;
    }
}

#endif // BASE_PLATFORM_iOS