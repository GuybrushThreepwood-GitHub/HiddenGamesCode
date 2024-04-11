
/*===================================================================
 File: mainiOS.cpp
 Game: H4
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import <UIKit/UIKit.h>

#import "Core/App.h"
#import "Core/ios/AppDelegate.h"

int main(int argc, char *argv[])
{	
	core::app::SupportiPodMusic(false);
    core::app::SupportPortrait(false);
    core::app::SupportLandscape(true);
    
	core::app::SetAppRateURL("itms-apps://itunes.apple.com/app/id380980096?at=10l6dK" );
	//core::app::SetAppRateURL("itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=380980096"); // http://itunes.apple.com/us/app/prisoner84/id380980096?mt=8
	
    @autoreleasepool {
        int retVal = UIApplicationMain(argc, argv, nil, @"AppDelegate");
        return retVal;
    }
}

#endif // BASE_PLATFORM_iOS
