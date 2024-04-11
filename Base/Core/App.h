
#ifndef __APP_H__
#define __APP_H__

#ifndef BASE_DUMMY_APP
	#ifdef BASE_PLATFORM_WINDOWS
		#ifndef __WINAPP_H__
			#include "win/WinApp.h"
		#endif // __WINAPP_H__
	#endif // BASE_PLATFORM_WINDOWS

	#ifdef BASE_PLATFORM_iOS
		#ifndef __IOSAPP_H__
			#include "ios/iOSApp.h"
		#endif // __IOSAPP_H__
	#endif // BASE_PLATFORM_iOS

	#ifdef BASE_PLATFORM_ANDROID
		#ifndef __ANDROIDAPP_H__
			#include "aos/AndroidApp.h"
		#endif // __ANDROIDAPP_H__
	#endif // BASE_PLATFORM_ANDROID

	#ifdef BASE_PLATFORM_RASPBERRYPI
		#ifndef __RASPBERRYPIAPP_H__
			#include "rpi/RaspberryPiApp.h"
		#endif // __RASPBERRYPIAPP_H__
	#endif // BASE_PLATFORM_RASPBERRYPI

	#ifdef BASE_PLATFORM_MAC
		#ifndef __MACAPP_H__
			#include "mac/MacApp.h"
		#endif // __MACAPP_H__
	#endif // BASE_PLATFORM_MAC
#endif // BASE_DUMMY_APP

namespace core
{
	enum EPlatform
	{
		PLATFORM_WINDOWS=0,
		PLATFORM_iOS,
        PLATFORM_tvOS,
		PLATFORM_ANDROID,
		PLATFORM_RASPBERRYPI,
		PLATFORM_MAC
	};
	
	namespace app
	{
		#ifndef BASE_DUMMY_APP
			#ifdef BASE_PLATFORM_WINDOWS
				#define PLATFORM_APP_CLASS win::WinApp
			#endif // BASE_PLATFORM_WINDOWS

			#ifdef BASE_PLATFORM_iOS
				#define PLATFORM_APP_CLASS ios::iOSApp
			#endif // BASE_PLATFORM_iOS
		
			#ifdef BASE_PLATFORM_ANDROID
				#define PLATFORM_APP_CLASS aos::AndroidApp
			#endif // BASE_PLATFORM_ANDROID
		
			#ifdef BASE_PLATFORM_RASPBERRYPI
				#define PLATFORM_APP_CLASS rpi::RaspberryPiApp
			#endif // BASE_PLATFORM_RASPBERRYPI

			#ifdef BASE_PLATFORM_MAC
				#define PLATFORM_APP_CLASS mac::MacApp
			#endif // BASE_PLATFORM_MAC
		#else
			class DummyApp
			{
				public:
					DummyApp(){}
					~DummyApp(){}
			};

			#define PLATFORM_APP_CLASS DummyApp
		#endif

		class App : public PLATFORM_APP_CLASS
		{	

		};

		/// SetPlatform - runtime platform details
		/// \param platform - which platform are we running
		void SetPlatform( core::EPlatform platform );
		/// GetPlatform - get the current known platform
		/// \return EPlatform - current platform
		core::EPlatform GetPlatform();
		
		/// SupportsHardwareKeyboard - does this platform have a hardware keyboard
		/// \return boolean - flag whether the current platform supports a hardware keyboard
		bool SupportsHardwareKeyboard();
		
		/// SetArgumentCount - set the argument count from main call
		/// \param count - number of arguments
		void SetArgumentCount( int count );
		/// GetArgumentCount - get the argument count from main call
		/// \return integer - number of arguments
		int GetArgumentCount();

		/// SetArgumentVariables - set the argument strings from main call
		/// \param argv - number of arguments
		void SetArgumentVariables( char** argv );
		/// GetArgumentVariables - get the argument strings from main call
		/// \return char pointer to pointer - all arguments
		char** GetArgumentVariables();

		/// SetUseZipFiles - set the flag to load everything from a zip file
		/// \param flag - flag state
		void SetLoadFilesFromZip( bool flag );
		/// GetLoadFilesFromZip - get the flag to check if everything is loaded from a zip file
		/// \return boolean - current flag state
		bool GetLoadFilesFromZip();

		/// SetRootZipFile - set the name of the root zip file if all files are stored in a zip
		/// \param rootFile - root zip file all files will load from
		void SetRootZipFile( const char* rootFile );
		/// GetRootZipFile - get the name of the root zip file if all files are stored in a zip
		/// \return const char* - get the root file name
		const char* GetRootZipFile();

		/// SetFPS - set the current fps
		/// \param fps - frame rate to set
		void SetFPS( float fps );
		/// GetFPS - get the current fps
		/// \return float - current app frame rate
		float GetFPS();

		/// SetFrameLock - set the current frame rate lock
		/// \param fps - frame rate lock to set
		void SetFrameLock( float fps );
		/// GetFrameLock - get the current fps
		/// \return float - current app locked frame rate
		float GetFrameLock();
      
		/// SupportLandscape - set the application to support/not support landscape mode
        /// \param state - true or false support
		void SupportLandscape( bool state );
		/// SupportPortrait - set the application to support/not support portrait mode
        /// \param state - true or false support       
		void SupportPortrait( bool state );
		/// SupportsLandscape - get the application support/no support of landscape mode
        /// \return state - true or false support
		bool SupportsLandscape();
		/// SupportsPortrait - get the application to support/no support of portrait mode
        /// \return state - true or false support       
		bool SupportsPortrait();        
        
		/// SetLandscape - set the application to landscape mode
        /// \param forceUpdate - update the values
		void SetLandscape(bool forceUpdate=false);
		/// SetPortrait - set the application to portrait mode
        /// \param forceUpdate - update the values        
		void SetPortrait(bool forceUpdate=false);

		/// IsLandscape - is the current mode landscape
		/// \return boolean - true if landscape
		bool IsLandscape();
		/// IsPortrait - is the current mode portrait       
		/// \return boolean - true if portrait
		bool IsPortrait();

		/// SetIsTablet - set the boolean if this is a tablet or not
        /// \param tablet - tablet state       
		void SetIsTablet(bool tablet);
		/// IsTablet - is this device a tablet      
		/// \return boolean - true if a tablet
		bool IsTablet();

		/// SetIsPCOnly - set the boolean if this is a pc only build
        /// \param state - pc only state       
		void SetIsPCOnly(bool state);
		/// IsPCOnly - is this build is pc only   
		/// \return boolean - true if a pc only build
		bool IsPCOnly();

		/// SetIstvOS - set the boolean if this should be tvOS
		/// \param state - tvOS state       
		void SetIstvOS(bool state);
		/// IstvOS - is this tvOS  
		/// \return boolean - true if a tvOS run
		bool IstvOS();

		/// SetIsRetinaDisplay - set the boolean if this is using retina display
        /// \param retina - retina state       
		void SetIsRetinaDisplay(bool retina);
		/// IsRetinaDisplay - is this device retina   
		/// \return boolean - true if a retina display
		bool IsRetinaDisplay();

		/// SetIsSupportingAchievements - set the boolean if this app support achievements
        /// \param achievementSupport - support state       
		void SetIsSupportingAchievements(bool achievementSupport);
		/// IsSupportingAchievements - is this has GC support  
		/// \return boolean - true if support is requested
		bool IsSupportingAchievements();

		/// SetIsGameCenterAvailable - set the boolean if this device has GameCenter
        /// \param gcSupport - GameCenter support state       
		void SetIsGameCenterAvailable(bool gcSupport);
		/// IsGameCenterAvailable - is this has GC support  
		/// \return boolean - true if available
		bool IsGameCenterAvailable();
		
		/// SetIsStoreAvailable - set the boolean if this device has a Store
        /// \param storeSupport - store support state       
		void SetIsStoreAvailable(bool storeSupport);
		/// IsStoreAvailable - is this has store support  
		/// \return boolean - true if available
		bool IsStoreAvailable();
	
		/// SetInBackground - set the boolean if this app is in the background
        /// \param inBackground - in background state       
		void SetInBackground(bool inBackground);
		/// IsInBackground - is this app is in the background
		/// \return boolean - true if in background
		bool IsInBackground();
		
        /// SetLanguage - current language
        /// \param language - set the current language id
        void SetLanguage( int language );
        /// GetLanguage - get the current language
        /// \return integer - current language id
        int GetLanguage();
        
		/// SetAppWidth - set the default width of the application
		/// \param width - width to set
		void SetAppWidth( int width );
		/// GetAppWidth - get the default width of the application
		/// \return integer - current app width 
		int GetAppWidth();

		/// SetAppHeight - set the default height of the application
		/// \param height - height to set
		void SetAppHeight( int height );
		/// GetAppHeight - get the default height of the application
		/// \return integer - current app height 
		int GetAppHeight();

		/// SetBaseAssetsWidth - set the default width of the assets (they can be scaled up/down)
		/// \param width - width to set
		void SetBaseAssetsWidth( int width );
		/// GetBaseAssetsWidth - get the default width of the assets
		/// \return integer - base assets width
		int GetBaseAssetsWidth();

		/// SetBaseAssetsHeight - set the default height of the assets (they can be scaled up/down)
		/// \param height - height to set
		void SetBaseAssetsHeight( int height );
		/// GetBaseAssetsHeight - get the default height of the assets
		/// \return integer - base assets height
		int GetBaseAssetsHeight();

		/// GetWidthScale - get the width scaling factor for assets
		/// \return float - assets width scale
		float GetWidthScale();
		/// GetHeightScale - get the height scaling factor for assets
		/// \return float - assets height scale
		float GetHeightScale();

		/// GetOrientationWidth - get the current orientation width
		/// \return integer - orientation width
		int GetOrientationWidth();
		/// GetOrientationHeight - get the current orientation height
		/// \return integer - orientation height
		int GetOrientationHeight();

		/// SetNaughtyFlag - set if the plist has been modified
		/// \param flag - true or false
		void SetNaughtyFlag( bool flag );
		/// GetNaughtyFlag - get if the plist has been modified
		/// \return boolean - naughty flag state
		bool GetNaughtyFlag();

		/// SetSmoothUpdate - change the update mode on iOS
		/// \param flag - true=on / false=off
		void SetSmoothUpdate( bool flag );
		/// GetSmoothUpdate - gets the current smooth update flag
		/// \param boolean - current flag state
		bool GetSmoothUpdate();

		/// SetSavePath - set the app save path
		/// \param path - app save path
		void SetSavePath( const char* path );
		/// GetSavePath - get the app save path
		/// \return const char - app save path
		const char* GetSavePath();

		/// SetUseVertexArrays - set the vertex array flag
		/// \param flag - flag state
		void SetUseVertexArrays( bool flag );
		/// GetUseVertexArrays - get the vertex array flag
		/// \return boolean - current flag state 
		bool GetUseVertexArrays();

		/// SetLowPowerMode - set the low power flag (ios uses pvr textures if available)
		/// \param flag - flag state
		void SetLowPowerMode( bool flag );
		/// GetLowPowerMode - get the low power flag
		/// \return boolean - current flag state 
		bool GetLowPowerMode();

		////////////////////////////////////////////////////////////////////////////////////////////////////
		
		/// SetIconID - Sets the application icon (should be called in main.cpp)
		/// \param resourceId = which resource in the rc/header file
		void SetIconID( int resourceId );
		/// GetIconID - Gets the set application icon
		/// \return integer - app icon id
		int GetIconID();

		/// SetAccelerometerState - Sets the state of the accelerometer if available
		/// \param state - true or false for on/off
		/// \param frequency - set the frequency of the updates
		void SetAccelerometerState( bool state, float frequency=0.0f );
		/// GetAccelerometerState - Gets the state of the accelerometer if available
		/// \return boolean - true or false for on/off
		bool GetAccelerometerState();

		/// SetDimScreenTimer - Sets the state of the idle time screen dimmer
		/// \param state - true or false for on/off
		void SetDimScreenTimer( bool state );
		
		/// SupportiPodMusic - Sets the state of the allowing ipod music to play (should be called in main.m)
		/// \param state - true or false for support/do not support
		void SupportiPodMusic( bool state );
		/// GetSupportiPodMusic - Gets the state of the allowing ipod music to play
		/// \return boolean - true or false for support/do not support		
		bool GetSupportiPodMusic();
		
		/// SetAppRateURL - Sets the url for app rating (should be called in main.m)
		/// \param url - full url 
		void SetAppRateURL( const char* url );
		/// GetAppRateURL - Gets the url for app rating
		/// \return const char* - the url 	
		const char* GetAppRateURL();		
		
		/// SetAdvertBarInfo - Sets the advert bar data
		/// \param appAdFilterId - string id to ignore certain ads
		/// \param localXMLPath - local asset path for the advert data
		/// \param externalXMLPath - url link for downloading external ads
		/// \param adBarSizeiPod - which size to use for ipod/iphone
		/// \param adBarSizeiPad - which size to use for ipad
		/// \param allowAdvertBarScaling - should the bar scale 
		void SetAdvertBarInfo( const char* appAdFilterId, const char* localXMLPath, const char* externalXMLPath, int adBarSizePhone, int adBarSizeTablet, int adBarSizePhoneRetina, int adBarSizeTabletRetina, bool allowAdvertBarScaling );	
		/// GetAdvertBarAppFilter - Gets the filter string for the ads
		/// \return const char* - filter string for the ads		
		const char* GetAdvertBarAppFilter();		
		/// GetAdvertBarLocalXMLPath - Gets the asset path for the local xml
		/// \return const char* - local asset path for the advert data		
		const char* GetAdvertBarLocalXMLPath();
		/// GetAdvertBarExternalXMLPath - Gets the asset path for the external xml
		/// \return const char* - external path/url for the advert data		
		const char* GetAdvertBarExternalXMLPath();
		/// GetAdvertBarSizePhone - Gets the app bar size for a phone
		/// \return integer - the bar size index	
		int GetAdvertBarSizePhone();	
		/// GetGetAdvertBarSizeTablet - Gets the app bar size for a tablet
		/// \return integer - the bar size index	
		int GetAdvertBarSizeTablet();	
		/// GetGetAdvertBarSizePhoneRetina - Gets the app bar size for a HD phone
		/// \return integer - the bar size index	
		int GetAdvertBarSizePhoneRetina();	
		/// GetGetAdvertBarSizeTabletRetina - Gets the app bar size for a HD tablet
		/// \return integer - the bar size index	
		int GetAdvertBarSizeTabletRetina();	
		/// SetAdBarPosition - Set the position of the ad bar
		/// \params x - x position
		/// \params y - y position
		void SetAdBarPosition( float x, float y );
		/// SetAdBarCenter - Set the center pos of the ad bar
		/// \params x - x position
		/// \params y - y position
		void SetAdBarCenter( float x, float y );		
		/// GetAdBarPosition - Get the position of the ad bar
		/// \params x - x position
		/// \params y - y position
		void GetAdBarPosition( float *x, float *y );
		/// GetAdBarCenter - Get the center of the ad bar
		/// \params x - x position
		/// \params y - y position
		void GetAdBarCenter( float *x, float *y );		
		/// SetAdBarState - Set the adbar draw state
		/// \params state - true for on, false for off
		void SetAdBarState( bool state );
		/// CanAdBarScale - get the flag whether the ad bar can scale
		/// \return boolean
		bool CanAdBarScale();
		/// GetAdBarState - Get the adbar draw state
		/// \return boolean - true for on, false for off
		bool GetAdBarState();


		/// SetKiipKeys - Sets the app and secret key for kiip (should be called in main.m)
		/// \param appKey - kiip api key
		/// \param secretKey - kiip secret key
		void SetKiipKeys( const char* appKey, const char* secretKey );
		/// GetKiipAppKey - Gets the app app key
		/// \return const char* - the key
		const char* GetKiipAppKey();
		/// GetKiipSecretKey - Gets the kiip secret key
		/// \return const char* - the key
		const char* GetKiipSecretKey();
        
        /// SetHandleMenuButton - Determines if the app is using the tvOS Menu button messages
        /// \params state - true for on, false for off
        void SetHandleMenuButton( bool state );
        /// GetHandleMenuButton - Get the state of the menu button handle flag
        /// \return boolean - true for on, false for off
        bool GetHandleMenuButton();
		
		/// OpenWebLink - opens a web link
		/// \param url - URL of the page to open
		void OpenWebLink( const char* url );

		/// IsNetworkAvailable - Check to see if we have access to the net
		/// \return boolean - true if network is available, false if it isn't
		bool IsNetworkAvailable();
		
		/// QuitApplication - Gracefully quits the application
		void QuitApplication();
    
#ifdef BASE_PLATFORM_tvOS
        // SetViewController - needs to be set to show the GC pop ups
        void SetViewController( GameViewController* controller );
        
        GameViewController* GetViewController();
#endif
        
	} // namespace app

} // namespace core

#endif // __APP_H__

