
/*===================================================================
 File: mainMac.m
 Game: AirCadets
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_MAC

#import <Cocoa/Cocoa.h>

#import "Core/App.h"
#import "Core/CoreConsts.h"
#import "Core/mac/AppDelegate.h"

MAINFUNC
{
	// aircadets will support ipod music
	core::app::SupportiPodMusic(true);
	
	core::app::SetAppRateURL(""); // http://itunes.apple.com/us/app/air-cadets/id298873144?mt=8
	
	core::app::SetFrameLock( core::FPS30 );
		
	
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
