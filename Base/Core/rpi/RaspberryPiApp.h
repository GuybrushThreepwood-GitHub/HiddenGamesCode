
#ifndef __RASPBERRYPIAPP_H__
#define __RASPBERRYPIAPP_H__

#ifdef BASE_PLATFORM_RASPBERRYPI

#include "Core/CoreConsts.h"

#ifndef __RASPBERRYPIINCLUDES_H__
	#include "RaspberryPiIncludes.h"
#endif // __RASPBERRYPIINCLUDES_H__

#include <cstdio>
#include <cstdarg>

namespace core
{
	static const char APP_NAME[]	= "RASPBERRYPIAPP";
	const int WINDOW_WIDTH		= 800;
	const int WINDOW_HEIGHT		= 600;
	const int FRAMERATE_TEXT	= 32;
	
	namespace rpi
	{
		#define MAINFUNC int main( int argc, char *argv[] )

		/// QuitEventHandler - Handles the app quit message
		bool QuitEventHandler();
		
		class RaspberryPiApp
		{
			protected:
				/// stored path the application was run from
				char m_szRootPath[core::MAX_PATH+core::MAX_PATH];
				/// stored path for the save data
				char m_szSaveFilePath[core::MAX_PATH+core::MAX_PATH];
				
				/// seconds since last tick
				float m_SecsPerTick;
				/// current absolute time in seconds
				float			m_Time;	
				/// elapsed absolute time since last frame
				float			m_ElapsedTime;		
				/// current app time in seconds
				float			m_AppTime;		
				/// elapsed app time since last frame
				float			m_ElapsedAppTime;	
				/// whether app time is paused by user
				int				m_Paused;			
				/// instantaneous frame rate
				float			m_FPS;				
				/// frame rate written to a string
				char			m_szFrameRate[FRAMERATE_TEXT];	

				float			m_Interval;

				/// lock framerate flag
				bool m_LockFramerate;
				/// locks to this framerate
				float m_FramerateLock;
			
				/// Initialise - virtual Initialise which can be called by a users custom app creation
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				virtual int Initialise()            { return 0; }
				/// FrameMove - virtual FrameMove which can be called by a users custom app update
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				virtual int FrameMove()             { return 0; }
				/// Render - virtual Render which can be called by a users custom app render
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				virtual int Render()                { return 0; }
				/// Cleanup - virtual Cleanup which can be called by a users custom app cleanup
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				virtual int Cleanup()               { return 0; }

			public:
				/// default constructor
				RaspberryPiApp();
				/// default destructor
				virtual ~RaspberryPiApp();

				/// Create - Used to Initialise any platform specific data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Create();
				/// Execute - Used to Execute any platform specific data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Execute();
				/// Destroy - Used to Destroy any platform specific data
				void Destroy();
				/// Run - Main platform loop
				/// \param nWidth - screen width
				/// \param nHeight - screen height
				/// \param bFullscreen - go fullscreen
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Run(  int nWidth=WINDOW_WIDTH, int nHeight=WINDOW_HEIGHT, bool bFullscreen=false );
				
				/// SetFrameLockState - enables/disables frame locked updates
				/// \param state - true or false
				void SetFrameLockState( bool state )	{ m_LockFramerate = state; }
				/// LockFramerate - virtual LockFramerate allows locking of the frame update
				/// \param rate - rate as which to lock at
				void LockFramerate( float rate )		{ m_FramerateLock = rate; }			
			
				/// GetFPS - Gets the app framerate
				/// \return float - current frames per second
				float GetFPS()							{ return m_FPS; }

				/// SetSaveDirectory - Sets the directory for saving
				/// \param dir - directory for saving
				void SetSaveDirectory( const char* dir )		
				{
					if( dir != 0 )
					{
						snprintf( m_szSaveFilePath, MAX_PATH+MAX_PATH, "%s", dir );
					}
				}
				/// GetSaveDirectory - Gets the directory for saving
				/// \return const char* - save directory
				const char* GetSaveDirectory()		{ return m_szSaveFilePath; }
			
				void CallFrameMove()				{ FrameMove(); }
				void CallRender()					{ Render(); }
				void SetElapsedTime( float delta )	{ m_ElapsedTime = delta; }
				float GetElapsedTime()				{ return m_ElapsedTime; }

				void SetSmoothUpdates( bool state )	{ m_SmoothUpdates = state; }
				bool GetSmoothUpdates()				{ return m_SmoothUpdates; }

			private:
				// DrawFrame - draws a frame
				void DrawFrame();
				
				bool ReadKeyboard();
				void ProcessKeyboard();

				bool ReadMouse();
				void ProcessMouse();

				void FilterLanguage( const char* languageString );

			private:
				bool m_SmoothUpdates;

			protected:
				int m_ScreenWidth;
				int m_ScreenHeight;
				/// width of the window
				int m_Width;
				/// height of the window
				int m_Height;		

				/// mouse file descriptor
				int m_KeyboardStream;
				/// mouse file descriptor
				int m_MouseStream;
		};

	} // namespace rpi
	
} // namespace core

#endif // BASE_PLATFORM_RASPBERRYPI

#endif // __RASPBERRYPIAPP_H__

