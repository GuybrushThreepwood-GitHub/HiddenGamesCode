
/*===================================================================
	File: WinApp.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_WINDOWS

#include "CoreBase.h"

#include "Math/RandomTables.h"

#include "Input/Input.h"
#include "Input/InputManager.h"

#include "Math/Vectors.h"
#include "Render/FF/OpenGL/OpenGL.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Sound/OpenAL.h"
#include "Network/NetworkFunctions.h"

using core::win::WinApp;
using input::gInputState;

void SetStdOutToNewConsole()
{
	int hConHandle;
	long lStdHandle;
	FILE *fp;

	// allocate a console for this app
	AllocConsole();

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;

	setvbuf( stdout, 0, _IONBF, 0 );
}

namespace core
{
	namespace win
	{
		// statics
		static HINSTANCE  s_hInstance = 0;
		static HWND s_hWnd = 0;

		/// GetWinInstance - Returns the current windows instance handle
		/// \return HINSTANCE - instance handle of current window
		HINSTANCE WinApp::GetWinInstance( void )		
		{ 
			return s_hInstance; 
		}

		/// SetWinHandle - Sets the current windows handle
		/// \param hWnd - handle of current window
		void WinApp::SetWinHandle( HWND hWnd )			
		{ 
			s_hWnd = hWnd; 
		}
		/// GetWinHandle - Returns the current windows handle
		/// \return HWND - handle of current window
		HWND WinApp::GetWinHandle( void )			
		{ 
			return s_hWnd; 
		}

		// when pretending to be the iphone, win32 has no titlebar
#ifdef BASE_WINDOW_IOS
		const int WINDOW_TITLEBAR_SIZE	= 0;
#else // !BASE_WINDOW_IOS
		const int WINDOW_TITLEBAR_SIZE	= 26;
#endif // BASE_WINDOW_IOS

		const int WIN_BPP = 32;

		bool bIsFullscreen = false;
		bool bExitMessagePump = false;
	}
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
WinApp::WinApp()
{
	m_HiResTimerAvailable = false;

	m_SecsPerTick = 0.0f;
	m_Time = 0.0f;	
	m_ElapsedTime = 0.0f;		
	m_AppTime = 0.0f;		
	m_ElapsedAppTime = 0.0f;	

	m_Paused = 0;			
	m_FPS = 0.0f;				

	m_LockFramerate = false;
	m_FramerateLock = core::FPS60;
}

/////////////////////////////////////////////////////
/// Default destructor
/// 
///
/////////////////////////////////////////////////////
WinApp::~WinApp()
{

}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int WinApp::Create( void )
{
	core::app::SetPlatform( core::PLATFORM_WINDOWS );
	//SetStdOutToNewConsole();

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF);
	_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT,_CRTDBG_FILE_STDERR);
#endif

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
	renderer::OpenGL::GetInstance()->SetupDisplay( m_hDC, WIN_BPP, WIN_BPP );

	// some default GL values
	renderer::OpenGL::GetInstance()->Init();	

	renderer::OpenGL::GetInstance()->SetupPerspectiveView( m_Width, m_Height );
	renderer::OpenGL::GetInstance()->SetNearFarClip( 1.0f, 10000.0f );
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.282f, 0.415f, 1.0f );

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	dbg::DebugCreateFont();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

#ifdef BASE_SUPPORT_OPENAL
	// initialise OpenAL
	snd::OpenAL::Initialise();
#endif // BASE_SUPPORT_OPENAL

#ifdef BASE_SUPPORT_NETWORKING
	network::Initialise();
#endif // BASE_SUPPORT_NETWORKING

	// call the virtual to setup anything per app
	if( Initialise() )
	{
		DBGLOG( "WinApp: Call to Initialise() failed!" );
		return(1);
	}

	// get the frequency of the timer
	LARGE_INTEGER m_qwTicksPerSec;

	// this firstly checks to see if the hi-res timer is avaialable on this processor
	if( QueryPerformanceFrequency( &m_qwTicksPerSec ) != 0 )
	{
		m_HiResTimerAvailable = true;

		m_SecsPerTick = 1.0f / static_cast<float>(m_qwTicksPerSec.QuadPart);

		QueryPerformanceCounter( &m_qwTime );
		m_qwLastTime.QuadPart = m_qwTime.QuadPart;

		m_qwAppTime.QuadPart        = 0;
		m_qwElapsedTime.QuadPart    = 0;
		m_qwElapsedAppTime.QuadPart = 0;
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Execute
/// Params: None
///
/////////////////////////////////////////////////////
int WinApp::Execute( void )
{
	if( m_HiResTimerAvailable )
	{
		// get the current time (keep in LARGE_INTEGER format for precision)
		QueryPerformanceCounter( &m_qwTime );
		m_qwElapsedTime.QuadPart = m_qwTime.QuadPart - m_qwLastTime.QuadPart;
		m_qwLastTime.QuadPart    = m_qwTime.QuadPart;

		m_qwElapsedAppTime.QuadPart = m_qwElapsedTime.QuadPart;

		m_qwAppTime.QuadPart    += m_qwElapsedAppTime.QuadPart;

		// store the current time values as floating point
		m_Time           = m_SecsPerTick * (static_cast<float>(m_qwTime.QuadPart));
		m_ElapsedTime    = m_SecsPerTick * (static_cast<float>(m_qwElapsedTime.QuadPart));
		m_AppTime        = m_SecsPerTick * (static_cast<float>(m_qwAppTime.QuadPart));
		m_ElapsedAppTime = m_SecsPerTick * (static_cast<float>(m_qwElapsedAppTime.QuadPart));
	
		// compute the frames per second once per second
		static float fLastTime = 0.0f;
		static DWORD dwFrames  = 0L;
		dwFrames++;

		// if there's a really large time step then it's probably been from debugging
		if( m_ElapsedTime > 0.5f )
			m_ElapsedTime = 0.0166f;

		// lock to the requested framerate
		/*if( m_LockFramerate )
		{
			while( m_ElapsedTime < m_FramerateLock )
			{
				QueryPerformanceCounter( &m_qwTime );
				m_qwElapsedTime.QuadPart = m_qwTime.QuadPart - m_qwLastTime.QuadPart;

				m_ElapsedTime    = m_SecsPerTick * (static_cast<float>(m_qwElapsedTime.QuadPart));

				Sleep( static_cast<DWORD>(m_FramerateLock-m_ElapsedTime) );
			}
		}*/

		// calculate the framerate and put it as the windows title
		if( m_Time - fLastTime > 1.0f )
		{
			m_FPS    = dwFrames / ( m_Time - fLastTime );
			fLastTime = m_Time;
			dwFrames  = 0L;
			snprintf( m_szFrameRate, FRAMERATE_TEXT, "%0.02f fps", m_FPS );
		}
		
		// if not fullscreen, show fps
		//if( !m_cfConfigFile.bFullscreen )
#ifdef BASE_SUPPORT_64BIT

#else
			SetWindowText( s_hWnd, m_szFrameRate );
#endif
	}
	else
	{
		static float lastTime = 0.0f;				// hold the time from the last frame
		static float frameTime = 0.0f;				// stores the last frame's time

		// Get the current time in seconds
		float currentTime = timeGetTime() * 0.001f;				

		// store the elapsed time between the current and last frame,
		// then keep the current frame in our static variable for the next frame.
		m_ElapsedTime = currentTime - frameTime;
		frameTime = currentTime;

		/*if( m_LockFramerate )
		{
			while( m_ElapsedTime < m_FramerateLock )
			{
				currentTime = timeGetTime() * 0.001f;	
				m_ElapsedTime = currentTime - frameTime;

				Sleep( static_cast<DWORD>(m_FramerateLock-m_ElapsedTime) );
			}
		}*/

		// Increase the frame counter
		++m_FPS;

		// calculate the framerate and put it as the windows title
		if( currentTime - lastTime > 1.0f )
		{
			// Here we set the lastTime to the currentTime
			lastTime = currentTime;
			m_FPS = 0;
			snprintf(m_szFrameRate, FRAMERATE_TEXT, "%0.02f fps", m_FPS );
		}
		
		// if not fullscreen and in debug, show fps
#ifdef _DEBUG
//		if( !m_cfConfigFile.bFullscreen )
#ifdef BASE_SUPPORT_64BIT

#else
			SetWindowText(s_hWnd, m_szFrameRate);
#endif // 
#endif // _DEBUG
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
			*/
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
			case SDL_CONTROLLERBUTTONDOWN:
			{
				if (input::InputManager::IsInitialised())
					input::InputManager::GetInstance()->ButtonEvent(e.cbutton.which, e.cbutton.button, true);
			}break;

			case SDL_JOYBUTTONUP:
			{
				if (input::InputManager::IsInitialised())
					input::InputManager::GetInstance()->ButtonEvent(e.jbutton.which, e.jbutton.button, false);
			}break;
			case SDL_CONTROLLERBUTTONUP:
			{
				if (input::InputManager::IsInitialised())
					input::InputManager::GetInstance()->ButtonEvent(e.cbutton.which, e.cbutton.button, false);
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

	DrawFrame();

	if (!core::app::IstvOS())
	{
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
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void WinApp::Destroy( void )
{
	Cleanup();
}

/////////////////////////////////////////////////////
/// Method: Run
/// Params: [in]WinEngineProc, [in]context, [in]hWndParent
///
/////////////////////////////////////////////////////
int WinApp::Run( int nWidth, int nHeight, bool bFullscreen, WNDPROC WinEngineProc, void* /*context*/, HWND hWndParent )
{
	WNDCLASSEX WndClass;
	MSG msg;

	s_hInstance = GetModuleHandle(0);

	m_Width = nWidth;
	m_Height = nHeight;

	// setup the window class
	WndClass.cbSize			= sizeof( WNDCLASSEX );
	WndClass.style			= 0;
	
	if( WinEngineProc )
		WndClass.lpfnWndProc	= WinEngineProc;
	else
		WndClass.lpfnWndProc	= WinProc;

	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;
	WndClass.hInstance		= s_hInstance;
	
	WndClass.hIcon	= LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(core::app::GetIconID()));
    WndClass.hIconSm  = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(core::app::GetIconID()), IMAGE_ICON, 16, 16, 0);

	//WndClass.hIcon			= LoadIcon( 0, IDI_APPLICATION );
	//WndClass.hIconSm		= LoadIcon( 0, IDI_APPLICATION );
	WndClass.hCursor		= LoadCursor( 0, IDC_ARROW );
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
	WndClass.lpszMenuName	= 0;
	WndClass.lpszClassName	= CLASS_NAME;

	// register it
	if( !RegisterClassEx( &WndClass ) )
	{
		DBGLOG( "WIN32APP: *ERROR* failed to Register Class\n" );
		return(1);
	}

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	if( !dbg::Debug::IsInitialised() )
	{
		dbg::Debug::Initialise();
		dbg::Debug::GetInstance()->CreateLogFile();
	}
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

	DWORD		dwExStyle;		
	DWORD		dwStyle;				
	RECT		WindowRect;	

	WindowRect.left		= static_cast<long>(0);	
	WindowRect.right	= m_Width;	
	WindowRect.top		= static_cast<long>(0);			
	WindowRect.bottom	= m_Height;	

	dwExStyle			= 0;//WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		
	dwStyle				= 0;//WS_OVERLAPPEDWINDOW;							

	AdjustWindowRectEx(&WindowRect, dwStyle, false, dwExStyle);	
	
	// launched from an external, set the path so assets will work
	if( core::app::GetArgumentCount() > 1 )
	{
		char** argVars = core::app::GetArgumentVariables();
		file::TFile appPath;

		file::CreateFileStructure( argVars[0], &appPath );
		
		char newPath[core::MAX_PATH+core::MAX_PATH];
		std::memset( &newPath, 0 , sizeof(char)*core::MAX_PATH+core::MAX_PATH );
		snprintf( newPath, core::MAX_PATH+core::MAX_PATH, "%s%s", appPath.szDrive, appPath.szPath );
		
		if( argVars[0] != 0 )
		{
			SetCurrentDirectory(newPath);
			DBGLOG( "Setting Path to: %s\n", newPath );
		}
	}

	// store the path the app was run in
	GetCurrentDirectoryA( MAX_PATH+MAX_PATH, m_szRootPath );
	DBGLOG( "Current Path: %s\n", m_szRootPath );

	// set the save path
	std::sprintf( m_szSaveFilePath, "%s", m_szRootPath );
	DBGLOG( "Save Path: %s\n", m_szSaveFilePath );

	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0 , sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/save", m_szSaveFilePath );

	// copy back
	snprintf( m_szSaveFilePath, core::MAX_PATH+core::MAX_PATH, savePath);

	// language request
	LANGID language = GetSystemDefaultUILanguage();

	if( PRIMARYLANGID( language ) == LANG_ENGLISH )
	{
		core::app::SetLanguage( core::LANGUAGE_ENGLISH );
	}
	else if( PRIMARYLANGID( language ) == LANG_FRENCH )
	{
		core::app::SetLanguage( core::LANGUAGE_FRENCH );
	}	
	else if( PRIMARYLANGID( language ) == LANG_ITALIAN )
	{
		core::app::SetLanguage( core::LANGUAGE_ITALIAN );
	}
	else if( PRIMARYLANGID( language ) == LANG_GERMAN )
	{
		core::app::SetLanguage( core::LANGUAGE_GERMAN );
	}
	else if( PRIMARYLANGID( language ) == LANG_SPANISH )
	{
		core::app::SetLanguage( core::LANGUAGE_SPANISH );
	}
	else
	{
		core::app::SetLanguage( core::LANGUAGE_ENGLISH );
	}

	// copy from parameter
	bIsFullscreen = bFullscreen;

	// try and create the window
	if( !bIsFullscreen )
	{
		dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS /*| WS_POPUP | WS_SYSMENU*/;

#ifdef BASE_WINDOW_IOS
		dwStyle |= WS_POPUP;
#endif // BASE_WINDOW_IOS

		//if( hWndParent )
		//	dwStyle |= WS_CHILD;

		// windowed mode
		s_hWnd = CreateWindowEx( dwExStyle, CLASS_NAME, APP_NAME, dwStyle,  ( GetSystemMetrics(SM_CXSCREEN)/2) - (m_Width/2), ( GetSystemMetrics(SM_CYSCREEN)/2) - (m_Height/2),  
								m_Width, m_Height, hWndParent, 0, s_hInstance, 0 ); 
	}
	else
	{
		dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;

		//if( hWndParent )
		//	dwStyle |= WS_CHILD;

		// fullscreen
		s_hWnd = CreateWindowEx( dwExStyle, CLASS_NAME, APP_NAME, dwStyle,  0, 0,  
								m_Width, m_Height, hWndParent, 0, s_hInstance, 0 ); 

		DEVMODE	dmScreen;
		std::memset( &dmScreen, 0, sizeof(dmScreen) );	

		dmScreen.dmSize			=	sizeof( dmScreen );			
		dmScreen.dmPelsWidth		=	m_Width;					
		dmScreen.dmPelsHeight		=	m_Height;					
		dmScreen.dmBitsPerPel		=	WIN_BPP;						
		dmScreen.dmFields			=	DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if( ChangeDisplaySettings( &dmScreen, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL )
		{
			DBGLOG( "The Requested Fullscreen Mode Is Not Supported By\nYour Video Card." );
			return(1);
		}
	}

	// was the creation successful
	if( s_hWnd == 0 )
	{
		DBGLOG( "hWnd == 0" );
		return(1);
	}

	// need a device context to pass to OpenGL
	m_hDC = GetWindowDC( s_hWnd );

	// show the created window
	ShowWindow( s_hWnd, SW_SHOWNORMAL );
	UpdateWindow( s_hWnd );
	SetFocus( s_hWnd );	

	// call the create method, which sets up the rendering and sound
	if( Create() )
	{
		DBGLOG( "WIN32APP: *ERROR* App creation failed\n" );
		return(1);
	}

	// initialise
	bExitMessagePump = false;

	// enter the application loop
	while( 1 )
	{
		// see if a message is waiting and remove it once dispatched
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{			
			if( msg.message == WM_QUIT )
				break;

			TranslateMessage( &msg );
	
			DispatchMessage( &msg );
		}
		else
		{
			if (!core::app::IstvOS())
			{
				if (gInputState.LButtonPressed)
				{
					gInputState.TouchesData[input::FIRST_TOUCH].Ticks += m_ElapsedTime;

					if (gInputState.TouchesData[input::FIRST_TOUCH].Ticks >= input::HOLD_TIME)
						gInputState.TouchesData[input::FIRST_TOUCH].bHeld = true;
				}

				if (gInputState.RButtonPressed)
				{
					gInputState.TouchesData[input::SECOND_TOUCH].Ticks += m_ElapsedTime;

					if (gInputState.TouchesData[input::SECOND_TOUCH].Ticks >= input::HOLD_TIME)
						gInputState.TouchesData[input::SECOND_TOUCH].bHeld = true;
				}
			}
			// call the Execute method
			Execute();

			if (!core::app::IstvOS())
			{
				// clear release flags
				if (gInputState.LButtonReleased)
				{
					gInputState.LButtonReleased = false;
					gInputState.TouchesData[input::FIRST_TOUCH].bActive = false;
					gInputState.TouchesData[input::FIRST_TOUCH].bRelease = false;
					gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec = math::Vec2(0.0f, 0.0f);
				}

				if (gInputState.RButtonReleased)
				{
					gInputState.RButtonReleased = false;
					gInputState.TouchesData[input::SECOND_TOUCH].bActive = false;
					gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
					gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2(0.0f, 0.0f);
				}

				// mouse wheel
				gInputState.bMouseWheel = false;
				gInputState.nMouseWheelDelta = 0;
			}
		}
		
		Sleep(0);
	}

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	dbg::DebugDestroyFont();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

	// user shutdown
	Destroy();

#ifdef BASE_SUPPORT_NETWORKING
	network::Shutdown();
#endif // BASE_SUPPORT_NETWORKING

#ifdef BASE_SUPPORT_OPENAL
	// release OpenAL
	snd::OpenAL::Shutdown();	
#endif // BASE_SUPPORT_OPENAL	

	// release OpenGL
	renderer::OpenGL::Shutdown();

    input::InputManager::Shutdown();
    
#ifdef BASE_SUPPORT_SDL
	SDL_Quit();
#endif // BASE_SUPPORT_SDL

	// destory window
	if( s_hWnd )
	{
		int wasDCReleased = ReleaseDC( s_hWnd, GetDC( s_hWnd ) );

		if( !wasDCReleased )
			DBGLOG( "WIN32APP: *ERROR* ReleaseDC did not release device context\n" );

		BOOL wasWindowDestroyed = DestroyWindow( s_hWnd );
		if( !wasWindowDestroyed )
			DBGLOG( "WIN32APP: *ERROR* Window was not destroyed\n" );

		BOOL unregisteredClass = UnregisterClass( CLASS_NAME, GetModuleHandle(0) );
		if( !unregisteredClass )
			DBGLOG( "WIN32APP: *ERROR* Class was not unregistered\n" );

		s_hWnd = 0;
	}

#if defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)
	file::ReportFileCounts();
	
	if( dbg::Debug::IsInitialised() )
		dbg::Debug::Shutdown();
#endif // defined(_DEBUG) || defined(BASE_USE_DEBUG_FONT)

//#ifdef _DEBUG
//	_CrtDumpMemoryLeaks();
//#endif // _DEBUG

	return( static_cast<int>(msg.wParam) );
}

/////////////////////////////////////////////////////
/// Method: DrawFrame
/// Params: None
///
/////////////////////////////////////////////////////
void WinApp::DrawFrame()
{
#ifdef BASE_WINDOW_IOS
	math::Vec3 clearColour;
	renderer::OpenGL::GetInstance()->GetClearColour( &clearColour.R, &clearColour.G, &clearColour.B );

	glClearColor( clearColour.R, clearColour.G, clearColour.B, 1.0f );

	// if not using the RTT do this
	renderer::OpenGL::GetInstance()->ClearScreen();
#else 
	// clear the screen
	renderer::OpenGL::GetInstance()->ClearScreen();
#endif // BASE_WINDOW_IOS

	// render anything
	Render();

#ifdef BASE_SUPPORT_OPENAL
	// check for AL errors
	snd::OpenAL::GetInstance()->CheckALErrors();
#endif // BASE_SUPPORT_OPENAL

	// flip the buffer
	renderer::OpenGL::GetInstance()->Flip();

	input::UpdateTouches(m_ElapsedTime);
}

/////////////////////////////////////////////////////
/// Function: WinProc
/// Params: [in]hWnd, [in]msg, [in]wParam, [in]lParam
///
/////////////////////////////////////////////////////
LRESULT CALLBACK core::win::WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;

	switch( msg )
	{
		case WM_CREATE:
		{
			SetFocus(hWnd);

			return(0);
		}break;
		case WM_SYSCOMMAND:							
		{
			switch( wParam )							
			{
				case SC_SCREENSAVE:					
				case SC_MONITORPOWER:				
				return 0;						
			}				
		}break;
		case WM_SIZE:
		{
			// react to window size changes
			if( renderer::OpenGL::IsInitialised() )
				renderer::OpenGL::GetInstance()->SetupPerspectiveView( static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)) );
		}break;
		case WM_MOUSEWHEEL:
		{
			if (!core::app::IstvOS())
			{
				gInputState.bMouseWheel = true;
				gInputState.nMouseWheelDelta = static_cast<long>(GET_WHEEL_DELTA_WPARAM(wParam));
			}
		}break;
		case WM_MOUSEMOVE:
		{
			if (!core::app::IstvOS())
			{
				int nWidth;
				int nHeight;

				if (renderer::OpenGL::IsInitialised())
					renderer::OpenGL::GetInstance()->GetWidthHeight(&nWidth, &nHeight);

				//bool bWrapMouse = false;

				// last mouse position
				gInputState.nLastMouseX = gInputState.nMouseX;
				gInputState.nLastMouseY = gInputState.nMouseY;

				// current mouse position
				long nCursorMouseX = static_cast<long>(LOWORD(lParam));
				long nCursorMouseY = static_cast<long>(HIWORD(lParam));

				gInputState.nMouseX = nCursorMouseX;
				gInputState.nMouseY = nCursorMouseY;

				// swap Y value so 0,0 is bottom left
				if (bIsFullscreen)
					gInputState.nMouseY = nHeight - gInputState.nMouseY;
				else
					gInputState.nMouseY = (nHeight - gInputState.nMouseY) - WINDOW_TITLEBAR_SIZE;

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

				gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec += math::Vec2(static_cast<float>(gInputState.TouchesData[input::FIRST_TOUCH].nXDelta), static_cast<float>(gInputState.TouchesData[input::FIRST_TOUCH].nYDelta));
			}
		}break;

		case WM_LBUTTONDOWN:
		{
			if (!core::app::IstvOS())
			{
				gInputState.LButtonPressed = true;
				gInputState.LButtonReleased = false;

				gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;
				gInputState.TouchesData[input::FIRST_TOUCH].bPress = true;
				gInputState.TouchesData[input::FIRST_TOUCH].bRelease = false;
				gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec = math::Vec2(0.0f, 0.0f);

				int nWidth;
				int nHeight;

				if (renderer::OpenGL::IsInitialised())
					renderer::OpenGL::GetInstance()->GetWidthHeight(&nWidth, &nHeight);

				//bool bWrapMouse = false;

				// last mouse position
				gInputState.nLastMouseX = gInputState.nMouseX;
				gInputState.nLastMouseY = gInputState.nMouseY;

				// current mouse position
				long nCursorMouseX = static_cast<long>(LOWORD(lParam));
				long nCursorMouseY = static_cast<long>(HIWORD(lParam));

				gInputState.nMouseX = nCursorMouseX;
				gInputState.nMouseY = nCursorMouseY;

				// swap Y value so 0,0 is bottom left
				if (bIsFullscreen)
					gInputState.nMouseY = nHeight - gInputState.nMouseY;
				else
					gInputState.nMouseY = (nHeight - gInputState.nMouseY) - WINDOW_TITLEBAR_SIZE;

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

		}break;
		case WM_LBUTTONUP:
		{
			if (!core::app::IstvOS())
			{
				gInputState.LButtonPressed = false;
				gInputState.LButtonReleased = true;

				gInputState.TouchesData[input::FIRST_TOUCH].bActive = true;
				gInputState.TouchesData[input::FIRST_TOUCH].Ticks = 0.0f;
				gInputState.TouchesData[input::FIRST_TOUCH].bPress = false;
				gInputState.TouchesData[input::FIRST_TOUCH].bRelease = true;
				gInputState.TouchesData[input::FIRST_TOUCH].bHeld = false;
			}
		}break;

		case WM_RBUTTONDOWN:
		{
			if (!core::app::IstvOS())
			{
				gInputState.RButtonPressed = true;
				gInputState.RButtonReleased = false;

				gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
				gInputState.TouchesData[input::SECOND_TOUCH].bPress = true;
				gInputState.TouchesData[input::SECOND_TOUCH].bRelease = false;
				gInputState.TouchesData[input::SECOND_TOUCH].vAccumulatedVec = math::Vec2(0.0f, 0.0f);

				int nWidth;
				int nHeight;

				if (renderer::OpenGL::IsInitialised())
					renderer::OpenGL::GetInstance()->GetWidthHeight(&nWidth, &nHeight);

				// current mouse position
				long nCursorMouseX = static_cast<long>(LOWORD(lParam));
				long nCursorMouseY = static_cast<long>(HIWORD(lParam));

				gInputState.nMouseX = nCursorMouseX;
				gInputState.nMouseY = nCursorMouseY;

				// swap Y value so 0,0 is bottom left
				if (bIsFullscreen)
					gInputState.nMouseY = nHeight - gInputState.nMouseY;
				else
					gInputState.nMouseY = (nHeight - gInputState.nMouseY) - WINDOW_TITLEBAR_SIZE;

				// touch
				gInputState.TouchesData[input::SECOND_TOUCH].nTouchX = gInputState.nMouseX;
				gInputState.TouchesData[input::SECOND_TOUCH].nTouchY = gInputState.nMouseY;
				gInputState.TouchesData[input::SECOND_TOUCH].nLastTouchX = gInputState.nMouseX;
				gInputState.TouchesData[input::SECOND_TOUCH].nLastTouchY = gInputState.nMouseY;
				gInputState.TouchesData[input::SECOND_TOUCH].nXDelta = 0;
				gInputState.TouchesData[input::SECOND_TOUCH].nYDelta = 0;
			}
		}break;
		case WM_RBUTTONUP:
		{
			if (!core::app::IstvOS())
			{
				gInputState.RButtonPressed = false;
				gInputState.RButtonReleased = true;

				gInputState.TouchesData[input::SECOND_TOUCH].bActive = true;
				gInputState.TouchesData[input::SECOND_TOUCH].Ticks = 0.0f;
				gInputState.TouchesData[input::SECOND_TOUCH].bPress = false;
				gInputState.TouchesData[input::SECOND_TOUCH].bRelease = true;
				gInputState.TouchesData[input::SECOND_TOUCH].bHeld = false;
			}
		}break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			bool vkPressed = false;
		    switch(wParam)
            {
				case VK_ESCAPE:
				case VK_PRIOR:
				case VK_NEXT:
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:
				case VK_F6:
				case VK_F7:
				case VK_F8:
				case VK_F9:
				case VK_F10:

				case VK_BACK:
				case VK_UP:
				case VK_DOWN:
				case VK_RIGHT:
				case VK_LEFT:

				case VK_ADD:
				case VK_SUBTRACT:

				case VK_TAB:
				case VK_RETURN:
				case VK_SPACE:
				case VK_LCONTROL:
				case VK_RCONTROL:
				//case VK_SHIFT:
				case VK_MENU:
				{
					gInputState.KeyStates.vkKeys[wParam] = true;
					gInputState.bKeyPressed = true;

					vkPressed = true;
				} break;
				default: 
					break;
            } // end switch

			if( !vkPressed )
			{
				gInputState.nKeyPressCode = static_cast<int>(wParam);
		
				//gInputState.KeyStates.bKeyPressTime[wParam] = 0;
				gInputState.KeyStates.bKeys[wParam] = true;

				if( (wParam >= 65 && wParam <= 90) )
				{
					//gInputState.KeyStates.bKeyPressTime[wParam+32] = 0;
					gInputState.KeyStates.bKeys[wParam+32] = true;
				}
				else if( (wParam >= 97 && wParam <= 122) )
				{
					//gInputState.KeyStates.bKeyPressTime[wParam-32] = 0;
					gInputState.KeyStates.bKeys[wParam-32] = true;
				}
				gInputState.bKeyPressed = true;

				// do shift handling here
				if( wParam == VK_SHIFT )
				{
					gInputState.KeyStates.vkKeys[wParam] = true;
					//gInputState.KeyStates.vkKeyPressTime[wParam] = 0;
				}
			}

			return 0;
		}break;
		case WM_CHAR:
		{
			gInputState.nKeyPressCode = static_cast<int>(wParam);
			gInputState.bKeyPressed = true;

			gInputState.KeyStates.bKeys[wParam] = true;

			return 0;
		}break;
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			bool vkReleased = false;
			switch(wParam)
            {
				case VK_ESCAPE:
				case VK_PRIOR:
				case VK_NEXT:
				case VK_F1:
				case VK_F2:
				case VK_F3:
				case VK_F4:
				case VK_F5:
				case VK_F6:
				case VK_F7:
				case VK_F8:
				case VK_F9:
				case VK_F10:

				case VK_BACK:
				case VK_UP:
				case VK_DOWN:
				case VK_RIGHT:
				case VK_LEFT:

				case VK_ADD:
				case VK_SUBTRACT:

				case VK_TAB:
				case VK_RETURN:
				case VK_SPACE:
				case VK_LCONTROL:
				case VK_RCONTROL:
				//case VK_SHIFT:
				case VK_MENU:
				{
					gInputState.KeyStates.vkKeys[wParam] = false;
					gInputState.KeyStates.vkKeyPressTime[wParam] = 0;

					gInputState.nKeyPressCode = -1;
					gInputState.bKeyPressed = false;

					vkReleased = true;
				} break;
				default: 
					break;
            } // end switch

			if( !vkReleased )
			{
				if( wParam >= 0 )
				{
					// disable capital codes
					gInputState.KeyStates.bKeyPressTime[wParam] = 0;
					gInputState.KeyStates.bKeys[wParam] = false;
					if( (wParam >= 65 && wParam <= 90) )
					{
						gInputState.KeyStates.bKeyPressTime[wParam+32] = 0;
						gInputState.KeyStates.bKeys[wParam+32] = false;
					}
					else if( (wParam >= 97 && wParam <= 122) )
					{
						gInputState.KeyStates.bKeyPressTime[wParam-32] = 0;
						gInputState.KeyStates.bKeys[wParam-32] = false;
					}

					// do shift handling here
					if( wParam == VK_SHIFT )
					{
						gInputState.KeyStates.vkKeys[wParam] = false;
						gInputState.KeyStates.vkKeyPressTime[wParam] = 0;
					}
				}

				gInputState.nKeyPressCode = -1;
				gInputState.bKeyPressed = false;
			}

			return 0;
		}break;
		case WM_CLOSE:
		{
			bExitMessagePump = true;
			DestroyWindow(hWnd);
			return(0);
		}break;

		case WM_DESTROY:
		{
			//PostQuitMessage(0);
			return(0);
		}break;
		case WM_QUIT:
		{
			bExitMessagePump = true;
			return(0);
		}break;

		default:
//			DBGLOG( "window message %d", msg );
			return DefWindowProc( hWnd, msg, wParam, lParam ); 
			break;
	}

	return( 0 );
}

// useful functions remapped for cross-platform

/////////////////////////////////////////////////////
/// Function: GetScreenCenter
/// Params: [in/out]pScreenX, [in/out]pScreenY
///
/////////////////////////////////////////////////////
void core::GetScreenCenter( int *pScreenX, int *pScreenY )
{
	if( pScreenX )
		*pScreenX = GetSystemMetrics(SM_CXSCREEN)/2;

	if( pScreenY )
		*pScreenY = GetSystemMetrics(SM_CYSCREEN)/2;
}

/////////////////////////////////////////////////////
/// Function: SetCursorPosition
/// Params: [in]nPosX, [in]nPosY
///
/////////////////////////////////////////////////////
void core::SetCursorPosition( int nPosX, int nPosY )
{
	SetCursorPos( nPosX, nPosY );
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
		
	}
	else
	{
	
	}
}

/////////////////////////////////////////////////////
/// Function: GetAccelerometerState
/// Params: None
///
/////////////////////////////////////////////////////
bool core::app::GetAccelerometerState()
{
	return false;
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
#ifdef BASE_SUPPORT_64BIT

#else
	//ShellExecute(0, "open", url, 0, 0, SW_SHOWDEFAULT);
#endif // 
}

#endif // BASE_PLATFORM_WINDOWS
