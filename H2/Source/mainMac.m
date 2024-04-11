
/*===================================================================
 File: mainMac.m
 Game: Cabby
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <Cocoa/Cocoa.h>

#import "Core/App.h"
#import "Core/CoreConsts.h"
#import "Core/mac/AppDelegate.h"

MAINFUNC
{
	// cabby will support ipod music
	core::app::SupportiPodMusic(true);
	
#ifdef CABBY_LITE
	// lite
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/cabby-lite/id339386175?mt=8
	
#elif CABBY_FREE
	// free
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/cabby-free/id563600742?mt=8
		
#else
	// full
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/cabby/id339383913?mt=8
	
	// kiip support
	//core::app::SetKiipKeys("", "");

#endif
	
	core::app::SetFrameLock( core::FPS60 );
	
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
