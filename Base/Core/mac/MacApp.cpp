
/*===================================================================
	File: MacApp.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_MAC

#include "CoreBase.h"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <cstdlib>

#include "Core/mac/AppDelegate.h"
#include "Math/RandomTables.h"

#include "Input/Input.h"
#include "Input/InputManager.h"
#include "Math/Vectors.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Sound/OpenAL.h"
#include "Network/NetworkFunctions.h"

using core::mac::MacApp;
using input::gInputState;

static int nUserScreenWidth = core::WINDOW_WIDTH;
static int nUserScreenHeight = core::WINDOW_HEIGHT;

bool bIsAppRunning = false;
bool accelerometerState = false;

struct timeval frameStartTime, frameEndTime;
uint64_t start = 0, end = 0;

/////////////////////////////////////////////////////
/// Function: QuitEventHandler
/// Params: None
///
/////////////////////////////////////////////////////
bool core::mac::QuitEventHandler()
{
	//[[NSApplication sharedApplication] terminate:nil];
	//[NSApp terminate: nil];
	[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
	
	return false;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
MacApp::MacApp()
{
	m_FPS = 0.0f;
			
	m_LockFramerate = false;
	m_FramerateLock = core::FPS60;
			
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
MacApp::~MacApp()
{

}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int MacApp::Create( void )
{
	core::app::SetPlatform( core::PLATFORM_MAC );
	
	math::InitRandomSeed();
			
	core::EndianCheck();
			
	DBGLOG( "FOPEN_MAX = %d\n", FOPEN_MAX );
			
	std::memset( &gInputState, 0, sizeof(input::TInputState) );
	gInputState.nKeyPressCode = -1;
			
	std::memset( gInputState.KeyStates.bKeys, false, sizeof( bool )*input::MAX_KEYS );
	std::memset( gInputState.KeyStates.bKeyPressTime, 0, sizeof( int )*input::MAX_KEYS );
	std::memset( gInputState.KeyStates.vkKeys, false, sizeof( bool )*input::MAX_KEYS );
	std::memset( gInputState.KeyStates.vkKeyPressTime, 0, sizeof( int )*input::MAX_KEYS );

	input::InitialiseInput();
    
#ifdef BASE_SUPPORT_SDL
    if (!SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC))
    {
        
    }
#endif // BASE_SUPPORT_SDL
    
    input::InputManager::Initialise();
    input::InputManager::GetInstance()->Init();
    
	// initialise OpenGL
	renderer::OpenGL::Initialise();

	// context
			
	// some default GL values
	renderer::OpenGL::GetInstance()->Init();
	
	renderer::OpenGL::GetInstance()->SetupPerspectiveView( m_ScreenWidth, m_ScreenHeight );
	renderer::OpenGL::GetInstance()->SetNearFarClip( 1.0f, 10000.0f );
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.282f, 0.415f, 1.0f );

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	dbg::DebugCreateFont();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

	// initialise OpenAL
	snd::OpenAL::Initialise();

#ifdef BASE_SUPPORT_NETWORKING
	network::Initialise();
#endif // BASE_SUPPORT_NETWORKING

	if( Initialise() )
	{
		DBGLOG( "MACAPP: Call to Initialise() failed!\n" );
		return(1);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Execute
/// Params: None
///
/////////////////////////////////////////////////////
int MacApp::Execute( void )
{
	gettimeofday( &frameEndTime, 0 );
			
	m_ElapsedTime = frameEndTime.tv_sec - frameStartTime.tv_sec + ((frameEndTime.tv_usec - frameStartTime.tv_usec)/1.0E6);
			
	// if there's a really large time step then it's probably been from debugging
	if( m_ElapsedTime > 0.5f )
		m_ElapsedTime = 0.5f;
			
	m_FPS = 1.0f/m_ElapsedTime;
			
	std::sprintf(m_szFrameRate, "%0.02f fps", m_FPS);
			
	gettimeofday( &frameStartTime, 0 );
	
	if( gInputState.LButtonPressed )
	{
		gInputState.TouchesData[input::FIRST_TOUCH].Ticks += m_ElapsedTime;
		
		if( gInputState.TouchesData[input::FIRST_TOUCH].Ticks >= input::HOLD_TIME )
			gInputState.TouchesData[input::FIRST_TOUCH].bHeld = true;
	}
	
	if( gInputState.RButtonPressed )
	{
		gInputState.TouchesData[input::SECOND_TOUCH].Ticks += m_ElapsedTime;
		
		if( gInputState.TouchesData[input::SECOND_TOUCH].Ticks >= input::HOLD_TIME )
			gInputState.TouchesData[input::SECOND_TOUCH].bHeld = true;
	}
	
#ifdef BASE_SUPPORT_SDL
    SDL_Event e;
    
    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
                //User requests quit
            case SDL_QUIT:
            {
                //quit = true;
            }break;
            case SDL_KEYDOWN:
            {
                // key press
                switch (e.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                    {
                        //quit = true;
                    }break;
                        
                    default:
                        break;
                }
            }break;
                /*case SDL_CONTROLLERBUTTONDOWN:
                 {
                 switch (e.cbutton.button)
                 {
                 case SDL_CONTROLLER_BUTTON_A:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_A\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_B:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_B\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_X:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_X\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_Y:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_Y\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_BACK:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_BACK\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_GUIDE:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_GUIDE\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_START:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_START\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_LEFTSHOULDER\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_RIGHTSHOULDER\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_LEFTSTICK\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_RIGHTSTICK\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_DPAD_UP:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_DPAD_UP\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_DPAD_DOWN\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_DPAD_LEFT\n");
                 }break;
                 case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                 {
                 DBGLOG("SDL_CONTROLLER_BUTTON_DPAD_RIGHT\n");
                 }break;
                 default:
                 break;
                 }
                 }break;
                 case SDL_CONTROLLERAXISMOTION:
                 {
                 switch (e.caxis.axis)
                 {
                 default:
                 break;
                 }
                 }break;
                 case SDL_JOYAXISMOTION:
                 {
                 // Handle Joystick Motion
                 if ((e.jaxis.value < -3200) || (e.jaxis.value > 3200))
                 {
                 if (e.jaxis.axis == 0)
                 {
                 // Left-right movement code goes here
                 DBGLOG("axis 0 left-right (%d)\n", e.jaxis.value);
                 }
                 
                 if (e.jaxis.axis == 1)
                 {
                 // Up-Down movement code goes here
                 DBGLOG("axis 1 up-down (%d)\n", e.jaxis.value);
                 }
                 
                 if (e.jaxis.axis == 2)
                 {
                 // Left-right movement code goes here
                 DBGLOG("axis 2 left-right(%d)\n", e.jaxis.value);
                 }
                 
                 if (e.jaxis.axis == 3)
                 {
                 // Up-Down movement code goes here
                 DBGLOG("axis 3 up-down(%d)\n", e.jaxis.value);
                 }
                 }
                 }break;*/
            case SDL_JOYHATMOTION:
            {
                if (input::InputManager::IsInitialised())
                    input::InputManager::GetInstance()->HatEvent(e.jhat.which, e.jhat.value);
                 
            }break;
            case SDL_JOYAXISMOTION:
            {
                if (input::InputManager::IsInitialised())
                    input::InputManager::GetInstance()->AxisEvent(e.jaxis.which, e.jaxis.axis, e.jaxis.value);
            }break;
            case SDL_JOYBUTTONDOWN:
            {
                if (input::InputManager::IsInitialised())
                    input::InputManager::GetInstance()->ButtonEvent(e.jbutton.which, e.jbutton.button, true);
            }break;
            case SDL_JOYBUTTONUP:
            {
                if (input::InputManager::IsInitialised())
                    input::InputManager::GetInstance()->ButtonEvent(e.jbutton.which, e.jbutton.button, false);
            }break;
            case SDL_JOYDEVICEADDED:
            {
                if( input::InputManager::IsInitialised() )
                    input::InputManager::GetInstance()->AddDevice(e.jdevice.which);
            }break;
                
            case SDL_JOYDEVICEREMOVED:
            {
                if( input::InputManager::IsInitialised() )
                    input::InputManager::GetInstance()->RemoveDevice(e.jdevice.which);
            }break;
                
            case SDL_CONTROLLERDEVICEADDED:         // A new Game controller has been inserted into the system
            {
                if( input::InputManager::IsInitialised() )
                    input::InputManager::GetInstance()->AddDevice(e.cdevice.which);
            }break;
            case SDL_CONTROLLERDEVICEREMOVED:       // An opened Game controller has been removed
            {
                if( input::InputManager::IsInitialised() )
                    input::InputManager::GetInstance()->RemoveDevice(e.cdevice.which);
            }break;
                
            default:
                break;
        }
    }
#endif // BASE_SUPPORT_SDL
    
	// call the update method
	FrameMove();

	// clear the screen
	renderer::OpenGL::GetInstance()->ClearScreen();

	// render anything
	Render();

	// check for AL errors
	snd::OpenAL::GetInstance()->CheckALErrors();
	
	// displayLink auto flips

	// clear release flags
	if( gInputState.LButtonReleased )
	{
		gInputState.LButtonReleased = false;
		gInputState.TouchesData[input::FIRST_TOUCH].bActive = false;
		gInputState.TouchesData[input::FIRST_TOUCH].bRelease = false;
		gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
	}
	
	if( gInputState.RButtonReleased )
	{
		gInputState.RButtonReleased = false;
		gInputState.TouchesData[input::SECOND_TOUCH].bActive = false;
		gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
		gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );
	}
	
	input::gInputState.nMouseXDelta = 0;
	input::gInputState.nMouseYDelta = 0;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = input::gInputState.nMouseXDelta;
	input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = input::gInputState.nMouseYDelta;
	
	// mouse wheel
	gInputState.bMouseWheel = false;
	gInputState.nMouseWheelDelta = 0;
	
	sleep(0);
	
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void MacApp::Destroy( void )
{
	Cleanup();
			
#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	dbg::DebugDestroyFont();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

#ifdef BASE_SUPPORT_NETWORKING
	network::Shutdown();
#endif // BASE_SUPPORT_NETWORKING

	// release OpenAL
	snd::OpenAL::Shutdown();	
			
	// release OpenGL
	renderer::OpenGL::Shutdown();
    
    input::InputManager::Shutdown();
    
#ifdef BASE_SUPPORT_SDL
    SDL_Quit();
#endif // BASE_SUPPORT_SDL
}

/////////////////////////////////////////////////////
/// Method: Run
/// Params: [in]EngineProc, [in]context
///
/////////////////////////////////////////////////////
int MacApp::Run( void (*EngineProc)(void *context), void *context )
{
	/*NSBundle* mainbundle = [NSBundle mainBundle];
	NSString* pathToBundle = [ mainbundle bundlePath ];
	if( pathToBundle )
	{
		NSLog( @"%@", pathToBundle );
	}
			
	[ pathToBundle getCString: (char *)m_szRootPath maxLength: (MAX_PATH+MAX_PATH) encoding: NSASCIIStringEncoding ];
			
	DBGLOG( "Current Path: %s\n", m_szRootPath );
			
	// make sure the lower level directory is also this direcory
	chdir( m_szRootPath );*/
	
	// set the resources folder as the root path
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    if (!CFURLGetFileSystemRepresentation(resourcesURL, true, (UInt8 *)m_szRootPath, (core::MAX_PATH+core::MAX_PATH)))
    {
        // error!
    }
	
	DBGLOG( "Current Path: %s\n", m_szRootPath );
	
    CFRelease(resourcesURL);
	
    chdir(m_szRootPath);
			
	//////
		
	NSError *error;
	NSURL *appSupportDir = [[NSFileManager defaultManager] URLForDirectory:NSApplicationSupportDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:YES error:&error];
	
	NSString *executableName = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleExecutable"];
	NSString* dir = [appSupportDir path];
	NSString* saveDir = [NSString stringWithFormat:@"%@/%@", dir, executableName];
	
	[[NSFileManager defaultManager] createDirectoryAtPath:saveDir withIntermediateDirectories:NO attributes:nil error:&error];

	[ saveDir getCString: (char *)m_szSaveFilePath maxLength: (core::MAX_PATH+core::MAX_PATH) encoding: NSASCIIStringEncoding ];
	
	DBGLOG( "Save Path: %s\n", m_szSaveFilePath );
	
	// find out where the Documents directory is for this app and use it as the save directory
	//NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	//NSString *documentsDirectory = [paths objectAtIndex:0];
			
	//[ documentsDirectory getCString: (char *)m_szSaveFilePath maxLength: (core::MAX_PATH+core::MAX_PATH) encoding: NSASCIIStringEncoding ];
	//DBGLOG( "Docs Path: %s\n", m_szSaveFilePath );

    // language
    NSString *language = [[NSLocale preferredLanguages] objectAtIndex:0];
    char languageString[core::MAX_PATH];
    [ language getCString: (char *)languageString maxLength: (core::MAX_PATH) encoding: NSASCIIStringEncoding ];
    DBGLOG( "Device language is %s\n", languageString);
    
    FilterLanguage( languageString );
						
	if( Create() )
	{
		DBGLOG( "MACAPP: *ERROR* Create() failed\n" );
		return(1);
	}	
					
	return(0);
}

/////////////////////////////////////////////////////
/// Method: FilterLanguage
/// Params: [in]languageString
///
/////////////////////////////////////////////////////
void MacApp::FilterLanguage( const char* languageString )
{
    if( strncmp( languageString, "en", core::MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_ENGLISH);
    }
    else if( strncmp( languageString, "fr", core::MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_FRENCH);
    }
    else if( strncmp( languageString, "it", core::MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_ITALIAN);
    }
    else if( strncmp( languageString, "de", core::MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_GERMAN);
    }
    else if( strncmp( languageString, "es", core::MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_SPANISH);
        
    }  
    else 
    {
        core::app::SetLanguage(LANGUAGE_ENGLISH);
    }
}

// useful functions remapped for cross-platform
/////////////////////////////////////////////////////
/// Function: GetScreenCenter
/// Params: [in/out]pScreenX, [in/out]pScreenY
///
/////////////////////////////////////////////////////
void core::GetScreenCenter( int *pScreenX, int *pScreenY )
{
	*pScreenX = nUserScreenWidth/2;
	*pScreenY = nUserScreenHeight/2;
}

/////////////////////////////////////////////////////
/// Function: SetCursorPosition
/// Params: [in]nPosX, [in]nPosY
///
/////////////////////////////////////////////////////
void core::SetCursorPosition( int nPosX, int nPosY )
{
	CGPoint point;
	point.x = nPosX;
	point.y = nPosY;
}

/////////////////////////////////////////////////////
/// Function: SetAccelerometerState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SetAccelerometerState( bool state, float frequency )
{
	if( state )
	{
		accelerometerState = true;
	}
	else
	{
		accelerometerState = false;
	}
}

/////////////////////////////////////////////////////
/// Function: GetAccelerometerState
/// Params: None
///
/////////////////////////////////////////////////////
bool core::app::GetAccelerometerState()
{
	return accelerometerState;
}

/////////////////////////////////////////////////////
/// Function: SetDimScreenTimer
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SetDimScreenTimer( bool state )
{

}

/////////////////////////////////////////////////////
/// Function: OpenWebLink
/// Params: [in]url
///
/////////////////////////////////////////////////////
void core::app::OpenWebLink( const char* url )
{
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:[NSString defaultCStringEncoding]]]];
}

#endif // BASE_PLATFORM_MAC

