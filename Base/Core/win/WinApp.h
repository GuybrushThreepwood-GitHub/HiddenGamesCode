
#ifndef __WINAPP_H__
#define __WINAPP_H__

#ifdef BASE_PLATFORM_WINDOWS

#ifndef __WININCLUDES_H__
	#include "WinIncludes.h"
#endif // __WININCLUDES_H__

#include "Core/CoreConsts.h"

namespace core
{
#ifdef BASE_SUPPORT_64BIT
	static const wchar_t CLASS_NAME[]	= L"DEFAULT_CLASS";
	static const wchar_t APP_NAME[]		= L"WIN32APP";
#else
	static const char CLASS_NAME[]	= "DEFAULT_CLASS";
	static const char APP_NAME[]	= "WIN32APP";
#endif //

	const int WINDOW_WIDTH		= 800;
	const int WINDOW_HEIGHT		= 600;
	const int FRAMERATE_TEXT	= 32;

	namespace win
	{
		#define MAINFUNC int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )

		/// WinProc - Main Win32 window procedure 
		/// \param hWnd - handle to the window
		/// \param wParam - 
		/// \param lParam - 
		LRESULT CALLBACK WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

		class WinApp
		{
			protected:
				/// stored path the application was run from
				char m_szRootPath[core::MAX_PATH+core::MAX_PATH];
				/// stored path for the save data
				char m_szSaveFilePath[core::MAX_PATH+core::MAX_PATH];

				// variables for timing

				/// hires timer flag
				int	m_HiResTimerAvailable;

				LARGE_INTEGER m_qwTime, m_qwLastTime, m_qwElapsedTime;
				LARGE_INTEGER m_qwAppTime, m_qwElapsedAppTime;
				
				/// seconds since last tick
				float m_SecsPerTick;
				/// current absolute time in seconds
				float m_Time;	
				/// elapsed absolute time since last frame
				float m_ElapsedTime;		
				/// current app time in seconds
				float m_AppTime;		
				/// elapsed app time since last frame
				float m_ElapsedAppTime;	
				/// whether app time is paused by user
				int	m_Paused;			
				/// instantaneous frame rate
				float m_FPS;				
				/// frame rate written to a string
				char m_szFrameRate[FRAMERATE_TEXT];	

				/// lock framerate flag
				bool m_LockFramerate;
				/// locks to this framerate
				float m_FramerateLock;

				/// Initialise - virtual Initialise which can be called by a users custom app creation
				/// \return integer - ( SUCCESS: 1 or FAIL: 0 )
				virtual int Initialise()            { return 1; }
				/// FrameMove - virtual FrameMove which can be called by a users custom app update
				/// \return integer - ( SUCCESS: 1 or FAIL: 0 )
				virtual int FrameMove()             { return 1; }
				/// Render - virtual Render which can be called by a users custom app render
				/// \return integer - ( SUCCESS: 1 or FAIL: 0 )
				virtual int Render()                { return 1; }
				/// Cleanup - virtual Cleanup which can be called by a users custom app cleanup
				/// \return integer - ( SUCCESS: 1 or FAIL: 0 )
				virtual int Cleanup()               { return 1; }

			public:
				/// default constructor
				WinApp();
				/// default destructor
				virtual ~WinApp();

				/// Create - Used to Initialise any platform specific data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Create();
				/// Execute - Used to Execute any platform specific data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Execute();
				/// Destroy - Used to Destroy any platform specific data
				void Destroy();
				/// Run - Main platform loop
				/// \param nWidth - width of the app
				/// \param nHeight - height of the app
				/// \param bFullscreen - fullscreen flag
				/// \param WinEngineProc - custom main loop callback
				/// \param context - custom callback data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Run( int nWidth=WINDOW_WIDTH, int nHeight=WINDOW_HEIGHT, bool bFullscreen=false, WNDPROC WinEngineProc = 0, void *context = 0, HWND hWndParent = 0 );

				/// SetFrameLockState - enables/disables frame locked updates
				/// \param state - true or false
				void SetFrameLockState( bool state )	{ m_LockFramerate = state; }
				/// LockFramerate - virtual LockFramerate allows locking of the frame update
				/// \param rate - rate as which to lock at
				void LockFramerate( float rate )		{ m_FramerateLock = rate; }

				/// GetFPS - Gets the app framerate
				/// \return float - current frames per second
				float GetFPS()							{ return m_FPS; }

				/// GetWinDC - Returns the current windows device context
				/// \return HDC - device context of current window
				HDC GetWinDC( void ) { return m_hDC; }
				/// GetWinInstance - Returns the current windows instance handle
				/// \return HINSTANCE - instance handle of current window
				static HINSTANCE GetWinInstance( void );

				/// SetWinHandle - Sets the current window handle
				/// \param hWnd - handle of current window
				static void SetWinHandle( HWND hWnd );
				/// GetWinHandle - Returns the current windows handle
				/// \return HWND - handle of current window
				static HWND GetWinHandle( void );

				/// GetSaveDirectory - Gets the directory for saving
				const char* GetSaveDirectory() { return m_szSaveFilePath; }

			private:
				// DrawFrame - draws a frame
				void DrawFrame();

			private:
				/// window handle
				static HWND ms_hWnd;
				/// instance handle
				static HINSTANCE ms_hInstance;
				/// device context handle
				HDC m_hDC;
				/// width of the window
				int m_Width;
				/// height of the window
				int m_Height;

		};

	} // namespace win

} // namespace core

#endif // BASE_PLATFORM_WINDOWS

#endif // __WINAPP_H__

