
/*===================================================================
 File: AppDelegate.h
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#if !defined(BASE_PLATFORM_tvOS)
    #import <CoreMotion/CoreMotion.h>
    #define APP_DELEGATES ,UIAccelerometerDelegate, UIAlertViewDelegate
#else
    #define APP_DELEGATES
#endif// !BASE_PLATFORM_tvOS

#include "CoreBase.h"
#include "SoundBase.h"

namespace snd { class MusicCallback; }

@interface AppDelegate : UIResponder <UIApplicationDelegate APP_DELEGATES>
{
#if !defined(BASE_PLATFORM_tvOS)
    CMMotionManager *motionManager;
    UIAlertView *mAlertView;
    UIAccelerationValue		accelerometerValues[3];
#endif // !BASE_PLATFORM_tvOS
    
    CGAffineTransform viewRotation;
    snd::MusicPauseCall* m_MusicPauseCall;
}

@property (strong, nonatomic) UIWindow *window;

#if !defined(BASE_PLATFORM_tvOS)
@property (nonatomic, strong) CMMotionManager *motionManager;
-(void) setAccelerometerState:(bool)state frequency:(float)frequency;
#endif // !BASE_PLATFORM_tvOS

@end

#endif // BASE_PLATFORM_iOS
