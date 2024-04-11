
/*===================================================================
 File: mainiOS.m
 Game: H8
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import <UIKit/UIKit.h>

#import "Core/App.h"
#import "Core/CoreConsts.h"
#import "Core/ios/AppDelegate.h"

int main(int argc, char *argv[])
{
	// firewall will support ipod music
	core::app::SupportiPodMusic(false);
    
    core::app::SupportLandscape(true);
    core::app::SupportPortrait(false);
    
	core::app::SetAppRateURL("itms-apps://itunes.apple.com/app/id603552348?at=10l6dK" );
	//core::app::SetAppRateURL("itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=603552348"); // http://itunes.apple.com/us/app/firewall/id603552348?mt=8

	core::app::SetFrameLock( core::FPS60 );
	
    @autoreleasepool {
        int retVal = UIApplicationMain(argc, argv, nil, @"AppDelegate");
        return retVal;
    }
}

#endif // BASE_PLATFORM_iOS