
/*===================================================================
 File: mainiOS.m
 Game: Cabby
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import <UIKit/UIKit.h>

#import "Core/App.h"
#import "Core/ios/AppDelegate.h"

int main(int argc, char *argv[])
{
	// cabby will support ipod music
	core::app::SupportiPodMusic(true);
    core::app::SupportPortrait(false);
    core::app::SupportLandscape(true);
    
#ifdef CABBY_LITE
	// lite
	core::app::SetAppRateURL("" ); // http://itunes.apple.com/us/app/cabby-lite/id339386175?mt=8
	
#elif CABBY_FREE
	// free
	core::app::SetAppRateURL("" ); // http://itunes.apple.com/us/app/cabby-free/id563600742?mt=8
	
#else
	// full
	core::app::SetAppRateURL("" ); // http://itunes.apple.com/us/app/cabby/id339383913?mt=8
	
	// kiip support
	//core::app::SetKiipKeys("", "");
	
#endif

	core::app::SetFrameLock( core::FPS30 );

    @autoreleasepool {
        int retVal = UIApplicationMain(argc, argv, nil, @"AppDelegate");
        return retVal;
    }
}

#endif // BASE_PLATFORM_iOS