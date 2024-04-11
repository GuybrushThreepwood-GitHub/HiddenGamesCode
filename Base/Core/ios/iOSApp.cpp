
/*===================================================================
	File: iOSApp.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_iOS

#include "CoreBase.h"

#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <cstdlib>

#include "Core/ios/AppDelegate.h"
#include "Math/RandomTables.h"

#include "Input/Input.h"
#include "Input/InputManager.h"
#include "Math/Vectors.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Sound/OpenAL.h"
#include "Network/NetworkFunctions.h"

using core::ios::iOSApp;
using input::gInputState;

static int nUserScreenWidth = core::WINDOW_WIDTH;
static int nUserScreenHeight = core::WINDOW_HEIGHT;

bool bIsAppRunning = false;
bool accelerometerState = false;

struct timeval frameStartTime, frameEndTime;

uint64_t start = 0, end = 0;

Rect titleBarRect;

/////////////////////////////////////////////////////
double GetTime();

/////////////////////////////////////////////////////
/// Function: QuitEventHandler
/// Params: None
///
/////////////////////////////////////////////////////
double GetTime()
{
	static mach_timebase_info_data_t sTimebaseInfo;
	uint64_t time = mach_absolute_time();
	uint64_t nanos;
	
	// If this is the first time we've run, get the timebase.
	// We can use denom == 0 to indicate that sTimebaseInfo is
	// uninitialised because it makes no sense to have a zero
	// denominator is a fraction.
	if ( sTimebaseInfo.denom == 0 ) 
	{
		(void) mach_timebase_info(&sTimebaseInfo);
	}
	
	// Do the maths.  We hope that the multiplication doesn't
	// overflow; the price you pay for working in fixed point.
	nanos = time * sTimebaseInfo.numer / sTimebaseInfo.denom;
	return( static_cast<float>( (double)nanos / 1000000000.0 ) );
}

/////////////////////////////////////////////////////
/// Function: QuitEventHandler
/// Params: None
///
/////////////////////////////////////////////////////
bool core::ios::QuitEventHandler()
{
	bIsAppRunning = false;
	return false;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
iOSApp::iOSApp()
{
	m_FPS = 0.0f;
			
	m_LockFramerate = false;
	m_FramerateLock = core::FPS60;
			
	m_SmoothUpdates = true;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
iOSApp::~iOSApp()
{

}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int iOSApp::Create( void )
{
#ifdef BASE_PLATFORM_tvOS
	core::app::SetPlatform( core::PLATFORM_tvOS );
#else
    core::app::SetPlatform( core::PLATFORM_iOS );
#endif //
    
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
		DBGLOG( "IOSAPP: Call to Initialise() failed!\n" );
		return(1);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Execute
/// Params: None
///
/////////////////////////////////////////////////////
int iOSApp::Execute( void )
{
	gettimeofday( &frameEndTime, 0 );
			
	m_ElapsedTime = frameEndTime.tv_sec - frameStartTime.tv_sec + ((frameEndTime.tv_usec - frameStartTime.tv_usec)/1.0E6);
			
	// if there's a really large time step then it's probably been from debugging
	if( m_ElapsedTime > 0.5f )
		m_ElapsedTime = 0.5f;
			
	m_FPS = 1.0f/m_ElapsedTime;
			
	std::sprintf(m_szFrameRate, "%0.02f fps", m_FPS);
			
	gettimeofday( &frameStartTime, 0 );

	// call the update method
	FrameMove();

	// clear the screen
	renderer::OpenGL::GetInstance()->ClearScreen();

	// render anything
	Render();

	// check for AL errors
	snd::OpenAL::GetInstance()->CheckALErrors();

	// iphone EAGLView should take care of the flip (may not be needed on a FBO)
	// flip the buffer
	//renderer::OpenGL::GetInstance()->Flip();
			
	//start = mach_absolute_time();

    input::UpdateTouches( m_ElapsedTime );
    
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void iOSApp::Destroy( void )
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
int iOSApp::Run( void (*EngineProc)(void *context), void *context )
{
    // hardware info
    size_t size;
    
    // Set 'oldp' parameter to NULL to get the size of the data
    // returned so we can allocate appropriate amount of space
    sysctlbyname("hw.machine", 0, &size, 0, 0); 
    
    // Allocate the space to store name
    char deviceVersion[MAX_PATH];
    
    // Get the platform name
    sysctlbyname("hw.machine", deviceVersion, &size, 0, 0);
    
    DBGLOG( "Device version: %s\n", deviceVersion );
	
    FilterHardware( deviceVersion );
    
	NSBundle* mainbundle = [NSBundle mainBundle];
	NSString* pathToBundle = [ mainbundle bundlePath ];
	if( pathToBundle )
	{
		NSLog( @"%@", pathToBundle );
	}
			
	[ pathToBundle getCString: (char *)m_szRootPath maxLength: (MAX_PATH+MAX_PATH) encoding: NSASCIIStringEncoding ];
			
	DBGLOG( "Current Path: %s\n", m_szRootPath );			
			
	// make sure the lower level directory is also this direcory
	chdir( m_szRootPath );
			
	// find out where the Documents directory is for this app and use it as the save directory
#ifdef BASE_PLATFORM_tvOS
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
#else
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
#endif
    
	NSString *documentsDirectory = [paths objectAtIndex:0];
			
	[ documentsDirectory getCString: (char *)m_szSaveFilePath maxLength: (MAX_PATH+MAX_PATH) encoding: NSASCIIStringEncoding ];
	DBGLOG( "Docs Path: %s\n", m_szSaveFilePath );	

    // language
    NSString *language = [[NSLocale preferredLanguages] objectAtIndex:0];
    char languageString[MAX_PATH];
    [ language getCString: (char *)languageString maxLength: (MAX_PATH) encoding: NSASCIIStringEncoding ];
    DBGLOG( "Device language is %s\n", languageString);
    
    FilterLanguage( languageString ); 
    
	// PIRACY CHECK
	// info.plist should be binarised and NOT contain and key-value pair
	m_PListCheck = true;
	NSDictionary *info = [mainbundle infoDictionary ];
	if( [info objectForKey:@"SignerIdentity"] != nil)
	{
		// naughty version
		m_PListCheck = false;
	}
			
						   
	if( Create() )
	{
		DBGLOG( "IOSAPP: *ERROR* Create() failed\n" );
		return(1);
	}	
					
	return(0);
}

/////////////////////////////////////////////////////
/// Method: FilterHardware
/// Params: [in]deviceVersion
///
/////////////////////////////////////////////////////
void iOSApp::FilterHardware( const char* deviceVersion )
{
    m_IsSimulator = false;
    m_IsiPod = false;
    m_IsiPhone = false;
    m_IsiPad = false;
    m_IsAppleTV = false;
	m_UnknownDevice = false;

    m_HardwareMajorNum = 0;
    m_HardwareMinorNum = 0;
	
    if (strncmp( deviceVersion, "i386", MAX_PATH ) == 0 ||
		strncmp( deviceVersion, "x86_64", MAX_PATH ) == 0 ) // "Simulator"
        m_IsSimulator = true;
    else 
	{
		std::size_t len = std::strlen( deviceVersion );
		bool nameComplete = false;
		std::memset( m_DeviceName, 0, sizeof(char)*core::MAX_PATH );
		
		if( len != 0 )
		{
			std::size_t i=0;
			for( i=0; i < len; i++ )
			{
				if( !nameComplete &&
				   isalpha(deviceVersion[i]) )
				   m_DeviceName[i] = deviceVersion[i];
				else 
				{
					if( !nameComplete )
						nameComplete = true;
						
					if( isalpha(deviceVersion[i]) ) // handle the comma
						continue;
					else if( isnumber(deviceVersion[i]) )
					{
						if( m_HardwareMajorNum == 0 )
						{
							sscanf( &deviceVersion[i], "%d", &m_HardwareMajorNum);
						}
						else if( m_HardwareMinorNum == 0)
						{
							sscanf( &deviceVersion[i], "%d", &m_HardwareMinorNum);
						}
					}
				}
			}
		}
		
		if( strcmp( m_DeviceName, "iPhone" ) == 0 )
		{
			m_IsiPhone = true;
		}
		else
		if( strcmp( m_DeviceName, "iPod" ) == 0 )
		{
			m_IsiPod = true;
		}
		else
		if( strcmp( m_DeviceName, "iPad" ) == 0 )
		{
			m_IsiPad = true;

			core::app::SetIsTablet( true );
		}
		else
		if( strcmp( m_DeviceName, "AppleTV" ) == 0 )
		{
			m_IsAppleTV = true;
		}
		else 
		{
			//unknown
			m_UnknownDevice = true;
		}
		
		/*if (strncmp( deviceVersion, "iPhone1,1", MAX_PATH ) == 0 )  // "iPhone 1G"
		{
			m_IsiPhone = true;
			m_HardwareMajorNum = 1;
			m_HardwareMinorNum = 1;
		}
		else
		if (strncmp( deviceVersion, "iPhone1,2", MAX_PATH ) == 0 )  // "iPhone 3G (China, no WiFi possibly)"
		{
			m_IsiPhone = true;
			m_HardwareMajorNum = 1;
			m_HardwareMinorNum = 2;
		}
		else
		if (strncmp( deviceVersion, "iPhone2,1", MAX_PATH ) == 0 )  // "iPhone 3GS"
		{
			m_IsiPhone = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 1;        
		}
		else
		if (strncmp( deviceVersion, "iPhone3,1", MAX_PATH ) == 0 )  // "iPhone 4 )"
		{
			m_IsiPhone = true;
			m_HardwareMajorNum = 3;
			m_HardwareMinorNum = 1;        
		}
		else
		if (strncmp( deviceVersion, "iPhone3,2", MAX_PATH ) == 0 )  // "iPhone 4 (CDMA/Verizon)"
		{
			m_IsiPhone = true;
			m_HardwareMajorNum = 3;
			m_HardwareMinorNum = 2;        
		}
		else
		if (strncmp( deviceVersion, "iPod1,1", MAX_PATH ) == 0 )    // "iPod Touch 1G"
		{        
			m_IsiPod = true;
			m_HardwareMajorNum = 1;
			m_HardwareMinorNum = 1;         
		}   
		else
		if (strncmp( deviceVersion, "iPod2,1", MAX_PATH ) == 0 )    // "iPod Touch 2G"
		{        
			m_IsiPod = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 1;           
		}
		else
		if (strncmp( deviceVersion, "iPod2,2", MAX_PATH ) == 0 )    // "iPod Touch 2.5G"
		{        
			m_IsiPod = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 2;           
		}
		else
		if (strncmp( deviceVersion, "iPod3,1", MAX_PATH ) == 0 )    // "iPod Touch 3G"
		{        
			m_IsiPod = true;
			m_HardwareMajorNum = 3;
			m_HardwareMinorNum = 1;           
		}
		else
		if (strncmp( deviceVersion, "iPod4,1", MAX_PATH ) == 0 )    // "iPod Touch 4G"
		{        
			m_IsiPod = true;
			m_HardwareMajorNum = 4;
			m_HardwareMinorNum = 1;           
		}
		else
		if (strncmp( deviceVersion, "iPad1,1", MAX_PATH ) == 0 )    // "iPad 1G (wifi)"
		{    
			m_IsiPad = true;
			m_HardwareMajorNum = 1;
			m_HardwareMinorNum = 1;         
		}
		else
		if (strncmp( deviceVersion, "iPad1,2", MAX_PATH ) == 0 )    // "iPad 1G (3G/GSM)"
		{    
			m_IsiPad = true;
			m_HardwareMajorNum = 1;
			m_HardwareMinorNum = 2;         
		}
		else
		if (strncmp( deviceVersion, "iPad2,1", MAX_PATH ) == 0 )    // "iPad 2G (wifi)"
		{    
			m_IsiPad = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 1;         
		}
		else
		if (strncmp( deviceVersion, "iPad2,2", MAX_PATH ) == 0 )    // "iPad 2G (GSM)"
		{    
			m_IsiPad = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 2;         
		}
		else
		if (strncmp( deviceVersion, "iPad2,3", MAX_PATH ) == 0 )    // "iPad 2G (CDMA)"
		{    
			m_IsiPad = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 3;         
		}
		else
		if (strncmp( deviceVersion, "AppleTV2,1", MAX_PATH ) == 0 ) // "Apple TV 2G"; 
		{    
			m_IsAppleTV = true;
			m_HardwareMajorNum = 2;
			m_HardwareMinorNum = 1;  
		}*/
	}
}

/////////////////////////////////////////////////////
/// Method: FilterLanguage
/// Params: [in]languageString
///
/////////////////////////////////////////////////////
void iOSApp::FilterLanguage( const char* languageString )
{
    if( strncmp( languageString, "en", MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_ENGLISH);
    }
    else if( strncmp( languageString, "fr", MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_FRENCH);
    }
    else if( strncmp( languageString, "it", MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_ITALIAN);
    }
    else if( strncmp( languageString, "de", MAX_PATH ) == 0 )
    {
        core::app::SetLanguage(LANGUAGE_GERMAN);
    }
    else if( strncmp( languageString, "es", MAX_PATH ) == 0 )
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
	AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
	
	if( state )
	{
#if !defined(BASE_PLATFORM_tvOS)
		// turn on
		[appDelegate setAccelerometerState:true frequency:1.0f/frequency];
#endif // !BASE_PLATFORM_tvOS
		accelerometerState = true;
	}
	else
	{
#if !defined(BASE_PLATFORM_tvOS)
		// turn off accelerometer
		[appDelegate setAccelerometerState:false frequency:0.0f];
#endif // !BASE_PLATFORM_tvOS
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
	if( state )
		[[UIApplication sharedApplication] setIdleTimerDisabled:NO];
	else
		[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
}

/////////////////////////////////////////////////////
/// Function: OpenWebLink
/// Params: [in]url
///
/////////////////////////////////////////////////////
void core::app::OpenWebLink( const char* url )
{
    if( url != nil )
    {
        NSString *stringURL = [NSString stringWithCString:url encoding:[NSString defaultCStringEncoding]];
        
        if( stringURL != nil )
        {
            NSURL *finalURL = [NSURL URLWithString:stringURL];
            
            if( finalURL != nil )
                [[UIApplication sharedApplication] openURL:finalURL];
        }
    }
}

#endif // BASE_PLATFORM_iOS

