
/*===================================================================
 File: GameViewController.mm
 Library: Core
 
 (C)Hidden Games
 =====================================================================*/

#import "Core/ios/GameViewController.h"
#import <OpenGLES/EAGLDrawable.h>

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "StateManage/IState.h"
#include "StateManage/StateManager.h"
#include "Cabby.h"
#include "CabbyConsts.h"

const float REMOTE_XAXIS_DEAD_ZONE = 0.35f;
const float REMOTE_YAXIS_DEAD_ZONE = 0.2f;

Cabby app;

@interface GameViewController () {
    GLint currentFBO;
    bool gameCenterActive;
    
    bool pausePressed;
    int pauseTick;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation GameViewController

-(void) loadView
{
    [super loadView];
    gameCenterActive = false;
    
    if( core::app::GetSupportiPodMusic() )
    {
        NSError *myErr;
        AVAudioSession *session = [AVAudioSession sharedInstance];
        
        // going to share the ipod audio (has to be done onload)
        [session setCategory:AVAudioSessionCategoryAmbient error:&myErr];
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    if ([[UIScreen mainScreen] respondsToSelector:@selector(nativeScale)] )
    {
        m_Scale = [UIScreen mainScreen].nativeScale;
        
        if( m_Scale > 1.0f )
            core::app::SetIsRetinaDisplay(true);
    }
    else
    {
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] )
        {
            // retina display
            m_Scale = [UIScreen mainScreen].scale;
            
            if( m_Scale > 1.0f )
                core::app::SetIsRetinaDisplay(true);
        }
        else
        {
            m_Scale = 1.0f;
        }
    }
    
#ifdef BASE_SUPPORT_OPENGL_GLSL
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
#else
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
#endif

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    self.view.multipleTouchEnabled = true;
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    //view.drawableMultisample = GLKViewDrawableMultisample4X;
    self.preferredFramesPerSecond = 60;

    currentFBO = renderer::INVALID_OBJECT;
    pausePressed = false;
    pauseTick = -1;
    
    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    GLKView *view = (GLKView *)self.view;
    [view bindDrawable];
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
    
    app.SetScreenWidth( static_cast<int>(view.drawableWidth) );
    app.SetScreenHeight( static_cast<int>(view.drawableHeight) );
    
    app.Run();
    
    // need to set up the scores here
    if( support::Scores::IsInitialised() != 0 )
    {
        support::Scores::GetInstance()->SetViewController( self );
        
        if( core::app::IsNetworkAvailable() )
        {
            support::Scores::GetInstance()->Login();
        }
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gameControllerDidConnect:) name:GCControllerDidConnectNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(gameControllerDidDisconnect:) name:GCControllerDidDisconnectNotification object:nil];
    
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:^(){
        NSLog(@"Wireless Discovery complete");
    }];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    if( support::Scores::IsInitialised() )
        support::Scores::Destroy();
    
    //
    app.Destroy();
}

- (void) goToPause
{
    if( !self.paused )
    {
        self.paused = true;
        
        // one update to pause the game
        app.FrameMove();
    }
}

- (void) undoPause
{
    if( self.paused )
    {
        self.paused = false;
    }
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    app.SetElapsedTime(static_cast<float>(self.timeSinceLastUpdate) );
    
    bool upHandled = false;
    bool downHandled = false;
    bool leftHandled = false;
    bool rightHandled = false;
    bool aHandled = false;
    //bool bHandled = false;
    bool xHandled = false;
    
    if( ([[GCController controllers] count] > 0) &&
       !gameCenterActive )
    {
        for( GCController* controller in [GCController controllers] )
        {
            if( controller.extendedGamepad )
            {
                if(controller.extendedGamepad.dpad.up.value >= REMOTE_YAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.yAxis.value >= REMOTE_YAXIS_DEAD_ZONE )
                {
                    input::gInputState.nKeyPressCode = input::KEY_UPARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    upHandled = true;
                }
                
                if(controller.extendedGamepad.dpad.down.value >= REMOTE_YAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.yAxis.value <= -REMOTE_YAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_DOWNARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    downHandled = true;
                }
                
                if(controller.extendedGamepad.dpad.left.value >= REMOTE_XAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.xAxis.value <= -REMOTE_XAXIS_DEAD_ZONE )
                {
                    input::gInputState.nKeyPressCode = input::KEY_LEFTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    leftHandled = true;
                }
                
                if(controller.extendedGamepad.dpad.right.value >= REMOTE_XAXIS_DEAD_ZONE ||
                   controller.extendedGamepad.leftThumbstick.xAxis.value >= REMOTE_XAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_RIGHTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    rightHandled = true;
                }
                
                if(controller.extendedGamepad.buttonA.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_ENTER;
                    input::gInputState.KeyStates.bKeys[input::KEY_ENTER] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    input::gInputState.nKeyPressCode = input::KEY_SPACE;
                    input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    aHandled = true;
                }
                
                /*if(controller.extendedGamepad.buttonB.pressed)
                 {
                 input::gInputState.nKeyPressCode = input::KEY_SPACE;
                 input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = true;
                 input::gInputState.bKeyPressed = true;
                 
                 bHandled = true;
                 }*/
                
                /*if(controller.extendedGamepad.buttonX.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_P;
                    input::gInputState.KeyStates.bKeys[input::KEY_P] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    xHandled = true;
                }*/
            }
            else
            if( controller.gamepad )
            {
                if(controller.gamepad.dpad.up.value >= REMOTE_YAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_UPARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    upHandled = true;
                }
                
                if(controller.gamepad.dpad.down.value >= REMOTE_YAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_DOWNARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    downHandled = true;
                }
                
                if(controller.gamepad.dpad.left.value >= REMOTE_XAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_LEFTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    leftHandled = true;
                }
                
                if(controller.gamepad.dpad.right.value >= REMOTE_XAXIS_DEAD_ZONE)
                {
                    input::gInputState.nKeyPressCode = input::KEY_RIGHTARROW;
                    input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    rightHandled = true;
                }
                
                if(controller.gamepad.buttonA.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_ENTER;
                    input::gInputState.KeyStates.bKeys[input::KEY_ENTER] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    input::gInputState.nKeyPressCode = input::KEY_SPACE;
                    input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    aHandled = true;
                }
                
                /*if(controller.gamepad.buttonX.pressed)
                {
                    input::gInputState.nKeyPressCode = input::KEY_P;
                    input::gInputState.KeyStates.bKeys[input::KEY_P] = true;
                    input::gInputState.bKeyPressed = true;
                    
                    xHandled = true;
                }*/
            }
            
            controller.controllerPausedHandler = ^(GCController *controller){
                pauseTick = 1;
            };
        }
    }
    else
    {
        
    }
    
    if( !upHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_UPARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_UPARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !downHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_DOWNARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_DOWNARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !leftHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_LEFTARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_LEFTARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !rightHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_RIGHTARROW] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_RIGHTARROW] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    if( !aHandled &&
       input::gInputState.KeyStates.bKeys[input::KEY_ENTER] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_ENTER] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_ENTER] = false;
        
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_SPACE] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    /*if( !bHandled &&
     input::gInputState.KeyStates.bKeys[input::KEY_SPACE] )
     {
     input::gInputState.KeyStates.bKeyPressTime[input::KEY_SPACE] = 0;
     input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = false;
     
     input::gInputState.nKeyPressCode = -1;
     input::gInputState.bKeyPressed = false;
     }*/
    
    /*if( !xHandled  &&
       input::gInputState.KeyStates.bKeys[input::KEY_P] )
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_P] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_P] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }*/
    
    if( pauseTick == 1 )
    {
        input::gInputState.nKeyPressCode = input::KEY_P;
        input::gInputState.KeyStates.bKeys[input::KEY_P] = true;
        input::gInputState.bKeyPressed = true;
        
        pauseTick = -1;
    }
    else
    {
        input::gInputState.KeyStates.bKeyPressTime[input::KEY_P] = 0;
        input::gInputState.KeyStates.bKeys[input::KEY_P] = false;
        
        input::gInputState.nKeyPressCode = -1;
        input::gInputState.bKeyPressed = false;
    }
    
    // have to update for the second touch
    //input::UpdateTouches(static_cast<float>(self.timeSinceLastUpdate));
    
    app.FrameMove();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if(currentFBO != renderer::INVALID_OBJECT )
        renderer::OpenGL::GetInstance()->BindFrameBufferObject(currentFBO);
    
    // clear the screen
    renderer::OpenGL::GetInstance()->ClearScreen();
    
    // render anything
    app.Render();
    
    // check for AL errors
    snd::OpenAL::GetInstance()->CheckALErrors();
    
    input::UpdateTouches( static_cast<float>(self.timeSinceLastUpdate) );
}

#pragma mark - Game Controllers

-(void)gameControllerDidConnect:(NSNotification *)notification
{
    NSLog( @"%lu Controllers attached", (unsigned long)[[GCController controllers] count] );
    /*if ( [[GCController controllers] count] > 0) {
     
     for( GCController* controller in [GCController controllers] )
     {
     if( tvRemote == nil &&
     controller.microGamepad != nil )
     {
     tvRemote = controller.microGamepad;
     tvRemote.allowsRotation = NO;
     //[GCController stopWirelessControllerDiscovery];
     }
     }
     }*/
}

-(void)gameControllerDidDisconnect:(NSNotification *)notification
{
    if ([[GCController controllers] count] == 0) {
        
        //tvRemote = nil;
    }
}

#pragma mark - Touches

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    CGRect bounds = [self.view bounds];
    
    bounds.size.width *= m_Scale;
    bounds.size.height *= m_Scale;
    
    for (UITouch *touch in touches)
    {
        location = [touch locationInView:self.view];
        
        location.x *= m_Scale;
        location.y *= m_Scale;
        
        input::AddTouch( bounds, touch, location );
    }}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    CGRect bounds = [self.view bounds];
    
    bounds.size.width *= m_Scale;
    bounds.size.height *= m_Scale;
    
    for (UITouch *touch in touches)
    {
        location = [touch locationInView:self.view];
        
        location.x *= m_Scale;
        location.y *= m_Scale;
        
        input::TouchMoved( bounds, touch, location );
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event {
    for (UITouch *touch in touches)
    {
        input::RemoveTouch( touch );
    }
}

// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    // If appropriate, add code necessary to save the state of the application.
    // This application is not saving state.
    
    for (UITouch *touch in touches)
    {
        input::RemoveTouch( touch );
    }	
    
}

// request to show a leaderboard
- (void) showLeaderboard: (NSString*) leaderboardIdentifier
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"gamecenter:"]];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// show the game achievement list
- (void) showAchievements
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"gamecenter:"]];
}

@end
