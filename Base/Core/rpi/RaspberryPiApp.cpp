
/*===================================================================
	File: RaspberryPiApp.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "CoreBase.h"
#include <linux/input.h>

#include "Math/RandomTables.h"

#include "Input/Input.h"
#include "Math/Vectors.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Render/FF/Primitives.h"
#include "Render/GLSL/PrimitivesGLSL.h"
#include "Sound/OpenAL.h"
#include "Network/NetworkFunctions.h"

using core::rpi::RaspberryPiApp;
using input::gInputState;

static int nUserScreenWidth = core::WINDOW_WIDTH;
static int nUserScreenHeight = core::WINDOW_HEIGHT;
	
namespace
{
	bool bIsAppRunning = false;
	bool accelerometerState = false;
	bool bIsFullscreen = false;
	struct timeval frameStartTime, frameEndTime;

	struct input_event keyboardInputEvent;
	struct input_event mouseInputEvent;	
}

/////////////////////////////////////////////////////
/// Function: QuitEventHandler
/// Params: None
///
/////////////////////////////////////////////////////
bool core::rpi::QuitEventHandler()
{
	bIsAppRunning = false;
	return false;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
RaspberryPiApp::RaspberryPiApp()
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
RaspberryPiApp::~RaspberryPiApp()
{

}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int RaspberryPiApp::Create( void )
{
	bcm_host_init();
   
	core::app::SetPlatform( core::PLATFORM_RASPBERRYPI );
	core::app::SetLoadFilesFromZip( false );

	// save file is set externally
	//std::sprintf( m_szSaveFilePath, "%s", core::app::GetRootZipFile() );
	//DBGLOG( "Save Path: %s\n", m_szSaveFilePath );

	math::InitRandomSeed();
			
	core::EndianCheck();
			
	DBGLOG( "FOPEN_MAX = %d\n", FOPEN_MAX );
			
	std::memset( &gInputState, 0, sizeof(input::TInputState) );
	gInputState.nKeyPressCode = -1;
			
	std::memset( gInputState.KeyStates.bKeys, false, sizeof( bool )*input::MAX_KEYS );
	std::memset( gInputState.KeyStates.bKeyPressTime, 0, sizeof( int )*input::MAX_KEYS );
	std::memset( gInputState.KeyStates.vkKeys, false, sizeof( bool )*input::MAX_KEYS );
	std::memset( gInputState.KeyStates.vkKeyPressTime, 0, sizeof( int )*input::MAX_KEYS );

	// language setup	
	// get C string
	const char* languageCode = "en";

	FilterLanguage( languageCode ); 

	input::InitialiseInput();

	m_KeyboardStream = -1;
	m_MouseStream = -1;		

	// keyboard
	m_KeyboardStream = open("/dev/input/event1",O_RDONLY|O_NONBLOCK);
		
	if( m_KeyboardStream < 0 )
		DBGLOG( "RASPBERRYPIAPP: *ERROR* could not open event1 stream\n" );
	else
		DBGLOG( "RASPBERRYPIAPP: Keyboard event stream open\n" );

	// mouse
	m_MouseStream = open("/dev/input/event0",O_RDONLY|O_NONBLOCK);
		
	if( m_MouseStream < 0 )
		DBGLOG( "RASPBERRYPIAPP: *ERROR* could not load mouse\n" );
	else
		DBGLOG( "RASPBERRYPIAPP: Mouse event stream open\n" );

	// initialise OpenGL
	renderer::OpenGL::Initialise();

	// context
	renderer::OpenGL::GetInstance()->SetupDisplay( m_ScreenWidth, m_ScreenHeight );
	renderer::OpenGL::GetInstance()->GetWidthHeight( &m_ScreenWidth, &m_ScreenHeight );
				
	// some default GL values
	renderer::OpenGL::GetInstance()->Init();
	renderer::OpenGL::GetInstance()->SetupPerspectiveView( m_ScreenWidth, m_ScreenHeight );
	renderer::OpenGL::GetInstance()->SetNearFarClip( 1.0f, 10000.0f );
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.282f, 0.415f, 1.0f );
	
	GL_CHECK;

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
		DBGLOG( "RASPBERRYPIAPP: Call to Initialise() failed!\n" );
		return(1);
	}
	
	gInputState.nMouseX = m_ScreenWidth/2;
	gInputState.nMouseY = m_ScreenHeight/2;
	
	gInputState.nLastMouseX = gInputState.nMouseX;
	gInputState.nLastMouseY = gInputState.nMouseY;
						
	GL_CHECK;	

	return(0);
}

/////////////////////////////////////////////////////
/// Method: ReadKeyboard
/// Params: None
///
/////////////////////////////////////////////////////
bool RaspberryPiApp::ReadKeyboard()
{
	if( m_KeyboardStream > 0 )
	{
		int bytes = read(m_KeyboardStream, &keyboardInputEvent, sizeof(struct input_event));
	
		if( bytes == -1 )
			return false;
		
		if (bytes == sizeof(struct input_event) ) 
		{
			return true;
		}
	}
	
	return false;
}

/////////////////////////////////////////////////////
/// Method: ProcessKeyboard
/// Params: None
///
/////////////////////////////////////////////////////
void RaspberryPiApp::ProcessKeyboard()
{
	if (m_KeyboardStream>=0) 
	{
		while( ReadKeyboard() )
		{
			switch( keyboardInputEvent.type)
			{
				case EV_KEY:
				{
					if( keyboardInputEvent.value == 1 ) // key down
					{
						gInputState.nKeyPressCode = static_cast<int>(keyboardInputEvent.code);
		
						gInputState.KeyStates.bKeys[gInputState.nKeyPressCode] = true;

						// if capital, set lower, if lower, set captial too
						/*if( (gInputState.nKeyPressCode >= 65 && gInputState.nKeyPressCode <= 90) )
						{
							gInputState.KeyStates.bKeys[gInputState.nKeyPressCode+32] = true;
						}
						else if( (gInputState.nKeyPressCode >= 97 && gInputState.nKeyPressCode <= 122) )
						{
							gInputState.KeyStates.bKeys[gInputState.nKeyPressCode-32] = true;
						}*/
						gInputState.bKeyPressed = true;

						// do shift handling here
						//if( gInputState.nKeyPressCode == input::KEY_RIGHTSHIFT )
						//{
						//	gInputState.KeyStates.vkKeys[gInputState.nKeyPressCode] = true;
						//}
					}
					else if( keyboardInputEvent.value == 0 ) // key up
					{
						int keyCode = static_cast<int>(keyboardInputEvent.code);

						// disable capital codes
						gInputState.KeyStates.bKeyPressTime[keyCode] = 0;
						gInputState.KeyStates.bKeys[keyCode] = false;

						/*if( (wParam >= 65 && wParam <= 90) )
						{
							gInputState.KeyStates.bKeyPressTime[wParam+32] = 0;
							gInputState.KeyStates.bKeys[wParam+32] = false;
						}
						else if( (wParam >= 97 && wParam <= 122) )
						{
							gInputState.KeyStates.bKeyPressTime[wParam-32] = 0;
							gInputState.KeyStates.bKeys[wParam-32] = false;
						}*/

						// do shift handling here
						//if( wParam == VK_SHIFT )
						//{
						//	gInputState.KeyStates.vkKeys[wParam] = false;
						//	gInputState.KeyStates.vkKeyPressTime[wParam] = 0;
						//}

						gInputState.nKeyPressCode = -1;
						gInputState.bKeyPressed = false;
					}
				}break;
				default:
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: ReadMouse
/// Params: None
///
/////////////////////////////////////////////////////
bool RaspberryPiApp::ReadMouse()
{
	if( m_MouseStream > 0 )
	{
		int bytes = read(m_MouseStream, &mouseInputEvent, sizeof(struct input_event));
	
		if( bytes == -1 )
			return false;
		
		if (bytes == sizeof(struct input_event) ) 
		{
			return true;
		}
	}
	
	return false;
}

/////////////////////////////////////////////////////
/// Method: ProcessMouse
/// Params: None
///
/////////////////////////////////////////////////////
void RaspberryPiApp::ProcessMouse()
{
	if (m_MouseStream>=0) 
	{
		while( ReadMouse() )
		{
			switch( mouseInputEvent.type)
			{
				case EV_KEY:
				{
					if( mouseInputEvent.code == BTN_LEFT )
					{
						if( mouseInputEvent.value == 1 ) // button down
						{
							//DBGLOG( "RASPBERRYPIAPP: LEFT_DOWN\n" );
							
							gInputState.LButtonPressed = true;
							gInputState.LButtonReleased = false;

							gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;
							gInputState.TouchesData[input::FIRST_TOUCH].bPress = true;
							gInputState.TouchesData[input::FIRST_TOUCH].bRelease = false;
							gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );

							/*
							// last mouse position
							gInputState.nLastMouseX = gInputState.nMouseX;
							gInputState.nLastMouseY = gInputState.nMouseY;

							// current mouse position
							/*long nCursorMouseX = 0;
							long nCursorMouseY = 0;

							gInputState.nMouseX = nCursorMouseX;
							gInputState.nMouseY = nCursorMouseY;;*/

							// mouse delta
							gInputState.nMouseXDelta = 0;
							gInputState.nMouseYDelta = 0;

							// touch
							gInputState.TouchesData[input::FIRST_TOUCH].nTouchX = gInputState.nMouseX;
							gInputState.TouchesData[input::FIRST_TOUCH].nTouchY = gInputState.nMouseY;
							gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = gInputState.nMouseX;
							gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = gInputState.nMouseY;
							gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = 0;
							gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = 0;						
						}
						else
						if( mouseInputEvent.value == 0 ) // button release
						{
							//DBGLOG( "RASPBERRYPIAPP: LEFT_UP\n" );
							
							gInputState.LButtonPressed = false;
							gInputState.LButtonReleased = true;

							gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;
							gInputState.TouchesData[input::FIRST_TOUCH].Ticks = 0.0f;
							gInputState.TouchesData[input::FIRST_TOUCH].bPress = false;
							gInputState.TouchesData[input::FIRST_TOUCH].bRelease = true;
							gInputState.TouchesData[input::FIRST_TOUCH].bHeld = false;						
						}
					}
					else
					if( mouseInputEvent.code == BTN_RIGHT )
					{
						if( mouseInputEvent.value == 1 ) // button down
						{
							//DBGLOG( "RASPBERRYPIAPP: RIGHT_DOWN\n" );
						
							gInputState.RButtonPressed = true;
							gInputState.RButtonReleased = false;

							gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
							gInputState.TouchesData[input::SECOND_TOUCH].bPress = true;
							gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
							gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2( 0.0f, 0.0f );

							/*
							// current mouse position
							long nCursorMouseX = gInputState.nMouseX;
							long nCursorMouseY = gInputState.nMouseX;

							gInputState.nMouseX = nCursorMouseX;
							gInputState.nMouseY = nCursorMouseY;*/


							// touch
							gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = gInputState.nMouseX;
							gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = gInputState.nMouseY;
							gInputState.TouchesData[input::SECOND_TOUCH].nLastTouchX = gInputState.nMouseX;
							gInputState.TouchesData[input::SECOND_TOUCH].nLastTouchY = gInputState.nMouseY;
							gInputState.TouchesData[input::SECOND_TOUCH].nXDelta = 0;
							gInputState.TouchesData[input::SECOND_TOUCH].nYDelta = 0;						
						}
						else
						if( mouseInputEvent.value == 0 )
						{
							//DBGLOG( "RASPBERRYPIAPP: RIGHT_UP\n" );
						
							gInputState.RButtonPressed = false;
							gInputState.RButtonReleased = true;

							gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
							gInputState.TouchesData[input::SECOND_TOUCH].Ticks = 0.0f;
							gInputState.TouchesData[input::SECOND_TOUCH].bPress = false;
							gInputState.TouchesData[input::SECOND_TOUCH].bRelease = true;
							gInputState.TouchesData[input::SECOND_TOUCH].bHeld = false;						
						}
					}	
				}break;
				case EV_REL:
				{
					if( mouseInputEvent.code == 0 )
					{
							
						// last mouse position
						gInputState.nLastMouseX = gInputState.nMouseX;
						gInputState.nLastMouseY = gInputState.nMouseY;

						// current mouse position
						long nCursorMouseX = gInputState.nMouseX;
						long nCursorMouseY = gInputState.nMouseY;
					
						if(mouseInputEvent.value < 0)
						{
							if(nCursorMouseX + mouseInputEvent.value > 0)
								nCursorMouseX += mouseInputEvent.value;
							else
								nCursorMouseX = 0;
						}
						if(mouseInputEvent.value > 0)
						{
							if(nCursorMouseX+ mouseInputEvent.value < m_ScreenWidth)
								nCursorMouseX+= mouseInputEvent.value;
							else
								nCursorMouseX= m_ScreenWidth;
						}	

						gInputState.nMouseX = nCursorMouseX;
						gInputState.nMouseY = nCursorMouseY;

						// mouse delta
						gInputState.nMouseXDelta = gInputState.nMouseX - gInputState.nLastMouseX;
						gInputState.nMouseYDelta = gInputState.nMouseY - gInputState.nLastMouseY;

						// touch
						gInputState.TouchesData[input::FIRST_TOUCH].nTouchX = gInputState.nMouseX;
						gInputState.TouchesData[input::FIRST_TOUCH].nTouchY = gInputState.nMouseY;
						gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = gInputState.nLastMouseX;
						gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = gInputState.nLastMouseY;
						gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = gInputState.nMouseXDelta;
						gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = gInputState.nMouseYDelta;

						gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec += math::Vec2( static_cast<float>(gInputState.TouchesData[input::FIRST_TOUCH].nXDelta), static_cast<float>(gInputState.TouchesData[input::FIRST_TOUCH].nYDelta) );

					}
					else 
					if( mouseInputEvent.code == 1 )
					{
						// last mouse position
						gInputState.nLastMouseX = gInputState.nMouseX;
						gInputState.nLastMouseY = gInputState.nMouseY;

						// current mouse position
						long nCursorMouseX = gInputState.nMouseX;
						long nCursorMouseY = gInputState.nMouseY;
							
						mouseInputEvent.value = mouseInputEvent.value * -1;
						if(mouseInputEvent.value < 0)
						{
							if(nCursorMouseY+ mouseInputEvent.value > 0)
								nCursorMouseY+= mouseInputEvent.value;
							else
								nCursorMouseY= 0;
						}
						if(mouseInputEvent.value > 0)
						{
							if(nCursorMouseY + mouseInputEvent.value < m_ScreenHeight)
								nCursorMouseY+= mouseInputEvent.value;
							else
								nCursorMouseY= m_ScreenHeight;
						}
						
						gInputState.nMouseX = nCursorMouseX;
						gInputState.nMouseY = nCursorMouseY;

						// mouse delta
						gInputState.nMouseXDelta = gInputState.nMouseX - gInputState.nLastMouseX;
						gInputState.nMouseYDelta = gInputState.nMouseY - gInputState.nLastMouseY;

						// touch
						gInputState.TouchesData[input::FIRST_TOUCH].nTouchX = gInputState.nMouseX;
						gInputState.TouchesData[input::FIRST_TOUCH].nTouchY = gInputState.nMouseY;
						gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = gInputState.nLastMouseX;
						gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = gInputState.nLastMouseY;
						gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = gInputState.nMouseXDelta;
						gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = gInputState.nMouseYDelta;

						gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec += math::Vec2( static_cast<float>(gInputState.TouchesData[input::FIRST_TOUCH].nXDelta), static_cast<float>(gInputState.TouchesData[input::FIRST_TOUCH].nYDelta) );
						
					}
					
				}break;
				case EV_ABS:
				{
									
				}break;
				default:
					break;
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Execute
/// Params: None
///
/////////////////////////////////////////////////////
int RaspberryPiApp::Execute( void )
{
	int i=0;
	gettimeofday( &frameEndTime, 0 );
			
	m_ElapsedTime = frameEndTime.tv_sec - frameStartTime.tv_sec + ((frameEndTime.tv_usec - frameStartTime.tv_usec)/1.0E6);
			
	// if there's a really large time step then it's probably been from debugging
	if( m_ElapsedTime > 0.5f )
		m_ElapsedTime = 0.5f;
			
	m_FPS = 1.0f/m_ElapsedTime;
			
	std::sprintf(m_szFrameRate, "%0.02f fps", m_FPS);
			
	gettimeofday( &frameStartTime, 0 );
	
	ProcessKeyboard();

	ProcessMouse();

	// call the update method
	FrameMove();

	// render anything
	DrawFrame();
	
	// mouse delta
	gInputState.nMouseXDelta = gInputState.nMouseX - gInputState.nLastMouseX;
	gInputState.nMouseYDelta = gInputState.nMouseY - gInputState.nLastMouseY;

	// last mouse position
	gInputState.nLastMouseX = gInputState.nMouseX;
	gInputState.nLastMouseY = gInputState.nMouseY;

	// same for first touch
	gInputState.TouchesData[input::FIRST_TOUCH].nXDelta = gInputState.nMouseXDelta;
	gInputState.TouchesData[input::FIRST_TOUCH].nYDelta = gInputState.nMouseYDelta;

	gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchX = gInputState.nLastMouseX;
	gInputState.TouchesData[input::FIRST_TOUCH].nLastTouchY = gInputState.nLastMouseY;
				
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void RaspberryPiApp::Destroy( void )
{
	if( m_KeyboardStream > 0 )
		close(m_KeyboardStream);

	if( m_MouseStream > 0 )
		close(m_MouseStream);

	Cleanup();
}

/////////////////////////////////////////////////////
/// Method: Run
/// Params: [in]EngineProc, [in]context
///
/////////////////////////////////////////////////////
int RaspberryPiApp::Run( int nWidth, int nHeight, bool bFullscreen )
{						
	m_Width = nWidth;
	m_Height = nHeight;
	m_ScreenWidth = nWidth;
	m_ScreenHeight = nHeight;

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	if( !dbg::Debug::IsInitialised() )
	{
		dbg::Debug::Initialise();
		dbg::Debug::GetInstance()->CreateLogFile();
	}
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	
	// store the path the app was run in
	//GetCurrentDirectory( core::MAX_PATH+core::MAX_PATH, m_szRootPath );
	getcwd( m_szRootPath, sizeof( m_szRootPath ) );
	DBGLOG( "Current Path: %s\n", m_szRootPath );
	chdir(m_szRootPath);

	// set the save path
	std::sprintf( m_szSaveFilePath, "%s", m_szRootPath );
	DBGLOG( "Save Path: %s\n", m_szSaveFilePath );

	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0 , sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/save", m_szSaveFilePath );

	// copy back
	snprintf( m_szSaveFilePath, core::MAX_PATH+core::MAX_PATH, savePath);

	// copy from parameter
	bIsFullscreen = bFullscreen;

	// try and create the window
	if( !bIsFullscreen )
	{

	}
	else
	{

	}	

	// call the create method, which sets up the rendering and sound
	if( Create() )
	{
		DBGLOG( "RASPBERRYPIAPP: *ERROR* App creation failed\n" );
		return(1);
	}

	bIsAppRunning = true;

	// enter the application loop
	while( bIsAppRunning )
	{
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

		// call the Execute method
		Execute();

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

		// mouse wheel
		gInputState.bMouseWheel = false;
		gInputState.nMouseWheelDelta = 0;

	}

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	dbg::DebugDestroyFont();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

	// user shutdown
	Destroy();

#ifdef BASE_SUPPORT_NETWORKING
	network::Shutdown();
#endif // BASE_SUPPORT_NETWORKING

	// release OpenAL
	snd::OpenAL::Shutdown();	
	
	// release OpenGL
	renderer::OpenGL::Shutdown();

	// destory window

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	file::ReportFileCounts();
	
	if( dbg::Debug::IsInitialised() )
		dbg::Debug::Shutdown();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

	return 0;

}

/////////////////////////////////////////////////////
/// Method: DrawFrame
/// Params: None
///
/////////////////////////////////////////////////////
void RaspberryPiApp::DrawFrame()
{
	math::Vec3 clearColour;
	renderer::OpenGL::GetInstance()->GetClearColour( &clearColour.R, &clearColour.G, &clearColour.B );

	glClearColor( clearColour.R, clearColour.G, clearColour.B, 1.0f );

	// if not using the RTT do this
	renderer::OpenGL::GetInstance()->ClearScreen();

	// render anything
	Render();

	// check for AL errors
	snd::OpenAL::GetInstance()->CheckALErrors();

	// draw mouse pos as cross
	if( renderer::OpenGL::IsInitialised() )
	{
		bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();
	
		renderer::OpenGL::GetInstance()->SetNearFarClip( -10.0f, 10.0f );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
		renderer::OpenGL::GetInstance()->DisableLighting();
		renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
		if( textureState )
			renderer::OpenGL::GetInstance()->DisableTexturing();
	
		math::Vec4 colour = math::Vec4(1.0f, 1.0f,1.0f, 1.0f);

		// top to bottom
		math::Vec3 start = math::Vec3(gInputState.nMouseX, 0.0f, 0.0f);
		math::Vec3 end = math::Vec3(gInputState.nMouseX, core::app::GetOrientationHeight(), 0.0f);

		renderer::DrawLine( start, end, colour );		
		
		// left to right
		start = math::Vec3(0.0f, gInputState.nMouseY, 0.0f);
		end = math::Vec3(core::app::GetOrientationWidth(), gInputState.nMouseY, 0.0f);

		renderer::DrawLine( start, end, colour );	

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

		if( textureState )
			renderer::OpenGL::GetInstance()->EnableTexturing();
	}
	
	// flip the buffer
	renderer::OpenGL::GetInstance()->Flip();
}


/////////////////////////////////////////////////////
/// Method: FilterLanguage
/// Params: [in]languageString
///
/////////////////////////////////////////////////////
void RaspberryPiApp::FilterLanguage( const char* languageString )
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

}

/////////////////////////////////////////////////////
/// Function: SetAccelerometerState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SetAccelerometerState( bool state, float frequency )
{

	accelerometerState = state;
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

}

#endif // BASE_PLATFORM_RASPBERRYPI

