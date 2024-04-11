
#ifndef __IOSAPP_H__
#define __IOSAPP_H__

#ifdef BASE_PLATFORM_iOS

#include "GameViewController.h"
#include "Core/CoreConsts.h"

namespace core
{
	static const char APP_NAME[]	= "IPHONEAPP";
	const int WINDOW_WIDTH		= 320;
	const int WINDOW_HEIGHT		= 480;
	const int FRAMERATE_TEXT	= 32;
	
	namespace ios
	{
		#define MAINFUNC int main( int argc, char *argv[] )

		/// QuitEventHandler - Handles the app quit message
		bool QuitEventHandler();
		
		class iOSApp
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
				iOSApp();
				/// default destructor
				virtual ~iOSApp();

				/// Create - Used to Initialise any platform specific data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Create();
				/// Execute - Used to Execute any platform specific data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Execute();
				/// Destroy - Used to Destroy any platform specific data
				void Destroy();
				/// Run - Main platform loop
				/// \param EngineProc - custom main loop callback
				/// \param context - custom callback data
				/// \return integer - ( SUCCESS: OK or FAIL: FAIL )
				int Run( void (*EngineProc)(void *context)=0, void *context=0 );
				
				/// SetFrameLockState - enables/disables frame locked updates
				/// \param state - true or false
				void SetFrameLockState( bool state )	{ m_LockFramerate = state; }
				/// LockFramerate - virtual LockFramerate allows locking of the frame update
				/// \param rate - rate as which to lock at
				void LockFramerate( float rate )		{ m_FramerateLock = rate; }			
			
				/// GetFPS - Gets the app framerate
				/// \return float - current frames per second
				float GetFPS()							{ return m_FPS; }

				/// GetSaveDirectory - Gets the directory for saving
				const char* GetSaveDirectory() { return m_szSaveFilePath; }
			
				///
				bool GetPListCheck()			{ return m_PListCheck; }
			
				/// iphone specific 
				void CallFrameMove()				{ FrameMove(); }
				void CallRender()					{ Render(); }
				void SetElapsedTime( float delta )	{ m_ElapsedTime = delta; }
				float GetElapsedTime()				{ return m_ElapsedTime; }
			
				void SetSmoothUpdates( bool state )	{ m_SmoothUpdates = state; }
				bool GetSmoothUpdates()				{ return m_SmoothUpdates; }
            
                void SetScreenWidth( int width )	{ m_ScreenWidth = width; }
                int GetScreenWidth( )				{ return m_ScreenWidth; }
	
                void SetScreenHeight( int height )	{ m_ScreenHeight = height; }
                int GetScreenHeight( )				{ return m_ScreenHeight; }
            
				bool IsSimulator()					{ return m_IsSimulator; }
				bool IsiPod()						{ return m_IsiPod; }
				bool IsiPhone()						{ return m_IsiPhone; }
				bool IsiPad()						{ return m_IsiPad; }
				bool IsAppleTV()					{ return m_IsAppleTV; }
				bool IsUnknown()					{ return m_UnknownDevice; }

				int GetMajorNum()					{ return m_HardwareMajorNum; }
				int GetMinorNum()					{ return m_HardwareMinorNum; }

            private:
                void FilterHardware( const char* deviceVersion );
                void FilterLanguage( const char* languageString );
            
			private:
				bool m_PListCheck;
				bool m_SmoothUpdates;
            
				char m_DeviceName[core::MAX_PATH];
            
            protected:
                int m_ScreenWidth;
                int m_ScreenHeight;            
			
				bool m_IsSimulator;
				bool m_IsiPod;
				bool m_IsiPhone;
				bool m_IsiPad;
				bool m_IsAppleTV;
			
				bool m_UnknownDevice;
			
				int m_HardwareMajorNum;
				int m_HardwareMinorNum;
			
		};

	} // namespace ios
	
} // namespace core

#endif // BASE_PLATFORM_iOS

#endif // __IOSAPP_H__

