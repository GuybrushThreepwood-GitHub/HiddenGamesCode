
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
#include "H1.h"
#include "H1Consts.h"

H1 app;

@interface GameViewController () {
    GLint currentFBO;
    bool gameCenterActive;
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
    view.drawableColorFormat = GLKViewDrawableColorFormatRGB565;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    //view.drawableMultisample = GLKViewDrawableMultisample4X;
    self.preferredFramesPerSecond = 30;

    currentFBO = renderer::INVALID_OBJECT;
    
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
