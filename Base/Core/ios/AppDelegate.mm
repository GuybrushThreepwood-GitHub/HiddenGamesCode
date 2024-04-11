
/*===================================================================
 File: AppDelegate.mm
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#ifdef BASE_PLATFORM_iOS

#import "AppDelegate.h"
#import "Core/ios/GameViewController.h"

#import <GameKit/GameKit.h>

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "InputBase.h"
#include "SoundBase.h"
#include "SupportBase.h"

#include "Audio/AudioSystem.h"

@interface AppDelegate ()

@end

@implementation AppDelegate

#if !defined(BASE_PLATFORM_tvOS)
@synthesize motionManager;
#endif // !BASE_PLATFORM_tvOS

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.

    // match the default loading orientation
    m_MusicPauseCall = 0;
    
#if !defined(BASE_PLATFORM_tvOS)
    mAlertView = nil;
    
    [self didRotate: nil]; // set up initial rotation
    
    // rotation event
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(didRotate:)
                                                 name:@"UIDeviceOrientationDidChangeNotification" object:nil];
    
    // accelerometer
    motionManager = [[CMMotionManager alloc] init];
#endif // !BASE_PLATFORM_tvOS
    
    return YES;
}

#if !defined(BASE_PLATFORM_tvOS)

///////////////////////////////////////////////////////////////////////////////////////////////////////
// rotate notification
- (void) didRotate:(NSNotification *)notification
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    
    if( orientation == UIDeviceOrientationUnknown ||
       orientation == UIDeviceOrientationFaceUp ||
       orientation == UIDeviceOrientationFaceDown )
    {
        if( core::app::SupportsLandscape() &&
           !core::app::SupportsPortrait() )
        {
            orientation = UIDeviceOrientationLandscapeLeft;
        }
        else if( !core::app::SupportsLandscape() &&
                core::app::SupportsPortrait() )
        {
            orientation = UIDeviceOrientationPortrait;
        }
        else
        {
            orientation = UIDeviceOrientationLandscapeLeft;
        }
    }
    
    if (orientation == UIDeviceOrientationPortrait) // Device oriented vertically, home button on the bottom
    {
        if( core::app::SupportsPortrait() )
        {
            // keeps alert views the correct orientation
            [[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortrait animated:NO];
            
            core::app::SetPortrait(true);
            
            if( renderer::OpenGL::IsInitialised() )
                renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM);
            
            /*viewRotation = CGAffineTransformMakeRotation(0.0f);*/
        }
    }
    else if( orientation == UIDeviceOrientationPortraitUpsideDown ) // Device oriented vertically, home button on the top
    {
        if( core::app::SupportsPortrait() )
        {
            // keeps alert views the correct orientation
            [[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationPortraitUpsideDown animated:NO];
            
            core::app::SetPortrait(true);
            
            if( renderer::OpenGL::IsInitialised() )
                renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_PORTRAIT_BUTTON_TOP);
            
            /*//viewRotation = CGAffineTransformMakeRotation(math::DegToRad(180.0f));*/
        }
    }
    else if( orientation == UIDeviceOrientationLandscapeLeft ) // Device oriented horizontally, home button on the right
    {
        if( core::app::SupportsLandscape() )
        {
            // keeps alert views the correct orientation
            [[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight animated:NO];
            
            core::app::SetLandscape(true);
            
            if( renderer::OpenGL::IsInitialised() )
                renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT);
            
            /*viewRotation = CGAffineTransformMakeRotation(math::DegToRad(0.0));
            if( mAlertView != nil )
            {
                [mAlertView setTransform: viewRotation];
            }*/
        }
    }
    else if( orientation == UIDeviceOrientationLandscapeRight ) // Device oriented horizontally, home button on the left
    {
        if( core::app::SupportsLandscape() )
        {
            // keeps alert views the correct orientation
            [[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeLeft animated:NO];
            
            core::app::SetLandscape(true);
            
            if( renderer::OpenGL::IsInitialised() )
                renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT);
            
            /*viewRotation = CGAffineTransformMakeRotation(math::DegToRad(-90.0f));
            if( mAlertView != nil )
            {
                [mAlertView setTransform: viewRotation];
            }*/
        }
    }
    else if( orientation == UIDeviceOrientationFaceUp ) // Device oriented flat, face up
    {
        //NSLog(@"UIDeviceOrientationFaceUp");
    }
    else if( orientation == UIDeviceOrientationFaceDown ) // Device oriented flat, face down
    {
        //NSLog(@"UIDeviceOrientationFaceDown");
    }
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
/*- (NSUInteger)application:(UIApplication *)application supportedInterfaceOrientationsForWindow:(UIWindow *)window
{
    NSUInteger validOrientations = 0;
    
    // Because your app is only landscape, your view controller for the view in your
    // popover needs to support only landscape
    if( core::app::SupportsLandscape() )
    {
        validOrientations |= UIInterfaceOrientationMaskLandscape;
    }
    if( core::app::SupportsPortrait() )
    {
        validOrientations |= UIInterfaceOrientationMaskPortrait;
    }
    
    // has to be set on phones
    //if( core::app::IsGameCenterAvailable() )
    //    validOrientations |= UIInterfaceOrientationMaskPortrait;
    
    return validOrientations;
}*/

///////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    // clicked to rate it
    if (buttonIndex == 1)
    {
        NSString *stringURL = [NSString stringWithCString:core::app::GetAppRateURL() encoding:[NSString defaultCStringEncoding]];
        
        if( stringURL != nil )
        {
            NSURL *url = [NSURL URLWithString:stringURL];
            
            if( url != nil )
                [[UIApplication sharedApplication] openURL:url];
        }
    }
    
    mAlertView = nil;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
-(void) setAccelerometerState:(bool)state frequency:(float)frequency
{
    if( state )
    {
        motionManager.accelerometerUpdateInterval = frequency;
        [motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue currentQueue] withHandler:^(CMAccelerometerData  *accelerometerData, NSError *error){
            // Use a basic low-pass filter to only keep the gravity in the accelerometer values
            const float kFilteringFactor = 0.1f; // For filtering out gravitational affects
            self->accelerometerValues[0] = accelerometerData.acceleration.x * kFilteringFactor + self->accelerometerValues[0] * (1.0 - kFilteringFactor);
            self->accelerometerValues[1] = accelerometerData.acceleration.y * kFilteringFactor + self->accelerometerValues[1] * (1.0 - kFilteringFactor);
            self->accelerometerValues[2] = accelerometerData.acceleration.z * kFilteringFactor + self->accelerometerValues[2] * (1.0 - kFilteringFactor);
            
            
            /*UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
            if( core::app::SupportsLandscape() )
            {
                if( orientation == UIDeviceOrientationLandscapeRight )
                {
                    accelerometerValues[1] = -accelerometerValues[1];
                }
            }*/
            input::gInputState.Accelerometers[0] = self->accelerometerValues[0];
            input::gInputState.Accelerometers[1] = self->accelerometerValues[1];
            input::gInputState.Accelerometers[2] = self->accelerometerValues[2];
        }];
    }
    else
    {
        [motionManager stopAccelerometerUpdates];
    }
}

#endif // !BASE_PLATFORM_tvOS

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url {
    return YES;
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.

    m_MusicPauseCall = snd::GetMusicPauseCall();
    
    if( m_MusicPauseCall != 0 )
        m_MusicPauseCall->PauseMusic();
    
    core::app::SetInBackground(true);
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.

    m_MusicPauseCall = snd::GetMusicPauseCall();
    
    if( m_MusicPauseCall != 0 )
        m_MusicPauseCall->UnPauseMusic();
    
    core::app::SetInBackground(false);
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    
    m_MusicPauseCall = snd::GetMusicPauseCall();
    
    if( m_MusicPauseCall != 0 )
        m_MusicPauseCall->PauseMusic();
    
    //if( core::app::IstvOS() )
    {
        core::app::SetInBackground(true);
        GameViewController* c = (GameViewController *)[application.keyWindow rootViewController];
        if( c )
        {
            [c goToPause];
        }
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    
    m_MusicPauseCall = snd::GetMusicPauseCall();
    
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    if( m_MusicPauseCall != 0 )
        m_MusicPauseCall->UnPauseMusic();
    
    //if( core::app::IstvOS() )
    {
        core::app::SetInBackground(false);
        GameViewController* c = (GameViewController *)[application.keyWindow rootViewController];
        if( c )
        {
            [c undoPause];
        }
    }
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end

#endif // BASE_PLATFORM_iOS
