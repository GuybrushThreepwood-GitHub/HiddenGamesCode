

#ifdef BASE_PLATFORM_MAC

#import <Cocoa/Cocoa.h>
#import "Core/mac/ViewController.h"

@class BorderlessWindow;

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
	BorderlessWindow *window;
	ViewController *view;
}

@property (nonatomic, strong) /*IBOutlet*/ BorderlessWindow *window;
@property (nonatomic, strong) /*IBOutlet*/ ViewController *view;

@end

#endif // BASE_PLATFORM_MAC