
/*===================================================================
 File: mainMac.m
 Game: h4
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <Cocoa/Cocoa.h>

#import "Core/App.h"
#import "Core/CoreConsts.h"
#import "Core/mac/AppDelegate.h"

MAINFUNC
{
	core::app::SupportiPodMusic(false);
	
	core::app::SetAppRateURL("itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=380980096"); // http://itunes.apple.com/us/app/prisoner84/id380980096?mt=8
	
	// flurry
	core::app::SetFlurryAPIKey( "3RVFMQXZZ8Q4QBPT75VX" );
	
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
