
/*===================================================================
 File: mainMac.m
 Game: H8
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <Cocoa/Cocoa.h>

#import "Core/App.h"
#import "Core/CoreConsts.h"
#import "Core/mac/AppDelegate.h"

MAINFUNC
{
	// firewall will support ipod music
	core::app::SupportiPodMusic(false);
	
	core::app::SetAppRateURL("itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=603552348"); // http://itunes.apple.com/us/app/firewall/id603552348?mt=8
	
	core::app::SetFrameLock( core::FPS60 );
	
	core::app::SetFlurryAPIKey( "SY9NJJN5SKGVBK4KN7B7" );
	
	// launch without nib
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSApplication * application = [NSApplication sharedApplication];
	
    AppDelegate * appDelegate = [ [[AppDelegate alloc] init] autorelease];
	
    [application setDelegate:appDelegate];
    [application run];
	
    //int retVal = NSApplicationMain(argc, argv, nil, @"AppDelegate");
    [pool release];
    //return retVal;
	
	return NSApplicationMain(argc, (const char **)argv);
}

#endif // BASE_PLATFORM_MAC
