
/*===================================================================
	File: App.cpp
	Library: Core

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "CollisionBase.h"
#include "RenderBase.h"

extern "C" 
{
	#include "Render/FreetypeZZipAccess.h"
}
#include "App.h"

static core::EPlatform s_Platform = core::PLATFORM_WINDOWS;

static bool s_LoadFilesFromZip = false;
static file::TFile s_RootZipFile;

static float s_FPS = 0.0f;
static float s_FrameLock = core::FPS60;

static int s_Language = core::LANGUAGE_ENGLISH;
static bool s_SupportHardwareKeyboard = false;

static int s_ArgumentCount = 0;
static char** s_ArgumentVars = 0;

static bool s_SupportLandscape = false;
static bool s_SupportPortrait = true;

static bool s_IsLandscape = false;
static bool s_IsPortrait = false;

static bool s_IsRetina = false;
static bool s_IsTablet = false;
static bool s_IsPCOnly = false;
static bool s_IstvOS = false;

static bool s_SupportAchievements = true;
static bool s_IsGameCenterAvailable = false;
static bool s_IsStoreAvailable = false;
static bool s_IsInBackground = false;

static int s_AppWidth = 320;
static int s_AppHeight = 480;

static int s_BaseAssetWidth = 320;
static int s_BaseAssetHeight = 480;

static float s_WidthScale = 1.0f;
static float s_HeightScale = 1.0f;

static int s_LandscapeWidth = 480;
static int s_LandscapeHeight = 320;

static int s_PortraitWidth = 320;
static int s_PortraitHeight = 480;

static bool s_NaughtyFlag = false;
static bool s_SmoothUpdates = true;
static const char* s_SavePath = 0;
static bool s_LowPowerMode = false;
static bool s_UseVertexArrays = false;

static bool s_AllowiPodMusic = false;
static const char* s_StoreLinkToRate = 0;

static bool s_AdvertBarAllowScaling = false;
static const char* s_AdvertBarLocalAppFilter = 0;
static const char* s_AdvertBarLocalXMLPath = 0;
static const char* s_AdvertBarExternalXMLPath = 0;
static int s_AdvertBarPhoneBarSize = 0;
static int s_AdvertBarTabletBarSize = 0;
static int s_AdvertBarPhoneRetinaBarSize = 0;
static int s_AdvertBarTabletRetinaBarSize = 0;
static float s_AdvertBarAdPosX = 0.0f;
static float s_AdvertBarAdPosY = 0.0f;
static float s_AdvertBarAdCenterX = 0.0f;
static float s_AdvertBarAdCenterY = 0.0f;

static bool s_AdBarState = false;

static const char* s_KiipAppKey = 0;
static const char* s_KiipSecretKey = 0;

static bool s_HandleMenuButton = false;

static int s_AppIconID = 32512; // IDI_APPLICATION

#ifdef BASE_PLATFORM_tvOS
    static GameViewController* s_CurrentViewController = nil;
#endif

/////////////////////////////////////////////////////
/// Function: SetPlatform
/// Params: [in]platform
///
/////////////////////////////////////////////////////
void core::app::SetPlatform( core::EPlatform platform )
{
	s_Platform = platform;

	/*if( s_Platform == PLATFORM_WINDOWS ||
		s_Platform == PLATFORM_RASPBERRYPI ||
       s_Platform == PLATFORM_tvOS ||
	   s_Platform == PLATFORM_MAC )*/
		s_SupportHardwareKeyboard = true;
}

/////////////////////////////////////////////////////
/// Function: GetPlatform
/// Params: None
///
/////////////////////////////////////////////////////
core::EPlatform core::app::GetPlatform()
{
	return s_Platform;
}

/////////////////////////////////////////////////////
/// Function: SupportsHardwareKeyboard
/// Params: None
///
/////////////////////////////////////////////////////
bool core::app::SupportsHardwareKeyboard()
{
	return s_SupportHardwareKeyboard;
}

/////////////////////////////////////////////////////
/// Function: SetArgumentCount
/// Params: None
///
/////////////////////////////////////////////////////
void core::app::SetArgumentCount( int count )
{
	s_ArgumentCount = count;
}

/////////////////////////////////////////////////////
/// Function: GetArgumentCount
/// Params: None
///
/////////////////////////////////////////////////////
int core::app::GetArgumentCount()
{
	return s_ArgumentCount;
}

/////////////////////////////////////////////////////
/// Function: SetArgumentVariables
/// Params: None
///
/////////////////////////////////////////////////////
void core::app::SetArgumentVariables( char** argv )
{
	s_ArgumentVars = argv;
}

/////////////////////////////////////////////////////
/// Function: GetArgumentVariables
/// Params: None
///
/////////////////////////////////////////////////////
char** core::app::GetArgumentVariables()
{
	return s_ArgumentVars;
}

/////////////////////////////////////////////////////
/// Function: SetLoadFilesFromZip
/// Params: [in]flag 
///
/////////////////////////////////////////////////////		
void core::app::SetLoadFilesFromZip( bool flag )
{
	s_LoadFilesFromZip = flag;
}

/////////////////////////////////////////////////////
/// Function: GetLoadFilesFromZip
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetLoadFilesFromZip()
{
	return s_LoadFilesFromZip;
}

/////////////////////////////////////////////////////
/// Function: SetRootZipFile
/// Params: [in]rootFile
///
/////////////////////////////////////////////////////		
void core::app::SetRootZipFile( const char* rootFile )
{
	file::CreateFileStructure( rootFile, &s_RootZipFile );

#if defined(BASE_SUPPORT_FREETYPE) || defined(BASE_SUPPORT_FREETYPE_UTF8)
	SetFreeTypeZipFile( s_RootZipFile.szFilename );
#endif // BASE_SUPPORT_FREETYPE
}

/////////////////////////////////////////////////////
/// Function: GetRootZipFile
/// 
///
/////////////////////////////////////////////////////
const char* core::app::GetRootZipFile()
{
	return s_RootZipFile.szFilename;
}

/////////////////////////////////////////////////////
/// Function: SetFPS
/// Params: [in]fps
///
/////////////////////////////////////////////////////
void core::app::SetFrameLock( float fps )
{
	s_FrameLock = fps;
}

/////////////////////////////////////////////////////
/// Function: GetFrameLock
/// 
///
/////////////////////////////////////////////////////
float core::app::GetFrameLock()
{
	return s_FrameLock;
}

/////////////////////////////////////////////////////
/// Function: SetFPS
/// Params: [in]fps
///
/////////////////////////////////////////////////////
void core::app::SetFPS( float fps )
{
	s_FPS = fps;
}

/////////////////////////////////////////////////////
/// Function: GetFPS
/// 
///
/////////////////////////////////////////////////////
float core::app::GetFPS()
{
	return s_FPS;
}

/////////////////////////////////////////////////////
/// Function: SupportLandscape
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SupportLandscape( bool state )
{
    s_SupportLandscape = state;    
}

/////////////////////////////////////////////////////
/// Function: SupportPortrait
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SupportPortrait( bool state )
{
    s_SupportPortrait = state;    
}

/////////////////////////////////////////////////////
/// Function: SupportsLandscape
/// 
///
/////////////////////////////////////////////////////
bool core::app::SupportsLandscape()
{
    return s_SupportLandscape;
}

/////////////////////////////////////////////////////
/// Function: SupportsPortrait
/// 
///
/////////////////////////////////////////////////////
bool core::app::SupportsPortrait()
{
    return s_SupportPortrait;
}

/////////////////////////////////////////////////////
/// Function: SetLandscape
/// Params: [in]forceUpdate
///
/////////////////////////////////////////////////////
void core::app::SetLandscape(bool forceUpdate)
{
	if( s_IsLandscape && 
		!forceUpdate)
		return;

	s_IsLandscape = true;
	s_IsPortrait = false;

#ifdef BASE_PLATFORM_WINDOWS
	s_LandscapeWidth = s_AppHeight;
	s_LandscapeHeight = s_AppWidth;
    
	int x = ( GetSystemMetrics(SM_CXSCREEN)/2) - (s_LandscapeWidth/2);
	int y = ( GetSystemMetrics(SM_CYSCREEN)/2) - (s_LandscapeHeight/2);
 
	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotationStyle( renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT ); // pc has to specify a rotation
		renderer::OpenGL::GetInstance()->SetRotated( false );
	}
#ifndef BASE_DUMMY_APP	
		if( core::win::WinApp::GetWinHandle() )
			MoveWindow( core::win::WinApp::GetWinHandle(), x, y, s_LandscapeWidth, s_LandscapeHeight, TRUE );
#endif //!BASE_DUMMY_APP
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_iOS
    s_LandscapeWidth = s_AppWidth;
    s_LandscapeHeight = s_AppHeight;        

	if( renderer::OpenGL::IsInitialised() )
		renderer::OpenGL::GetInstance()->SetRotated( false );
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
	s_LandscapeWidth = s_AppWidth;
	s_LandscapeHeight = s_AppHeight;
 
	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotated( false );
		// JAVA call: setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
	}
#endif // BASE_PLATFORM_ANDROID

#ifdef BASE_PLATFORM_RASPBERRYPI
	s_LandscapeWidth = s_AppWidth;
	s_LandscapeHeight = s_AppHeight;
    
	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotationStyle( renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT ); // rpi has to specify a rotation
		renderer::OpenGL::GetInstance()->SetRotated( false );
	}
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_MAC

#ifdef BASE_DUMMY_APP
	s_LandscapeWidth = s_AppHeight;
	s_LandscapeHeight = s_AppWidth;
#else
	s_LandscapeWidth = s_AppWidth;
	s_LandscapeHeight = s_AppHeight;
#endif
	
	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotationStyle( renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT ); // mac has to specify a rotation
		renderer::OpenGL::GetInstance()->SetRotated( false );
	}
#endif // BASE_PLATFORM_MAC
	
	if( renderer::OpenGL::IsInitialised() &&
		renderer::OpenGL::GetInstance()->GetIsRotated() )
	{
		s_HeightScale = static_cast<float>(s_LandscapeWidth) / static_cast<float>(s_BaseAssetWidth);
		s_WidthScale = static_cast<float>(s_LandscapeHeight) / static_cast<float>(s_BaseAssetHeight);  
	}
	else
	{
		s_WidthScale = static_cast<float>(s_LandscapeWidth) / static_cast<float>(s_BaseAssetHeight);
		s_HeightScale = static_cast<float>(s_LandscapeHeight) / static_cast<float>(s_BaseAssetWidth);  
	}
}

/////////////////////////////////////////////////////
/// Function: SetPortrait
/// Params: [in]forceUpdate
///
/////////////////////////////////////////////////////
void core::app::SetPortrait(bool forceUpdate)
{
	if( s_IsPortrait && 
		!forceUpdate)
		return;

	s_IsPortrait = true;
	s_IsLandscape = false;

#ifdef BASE_PLATFORM_WINDOWS
	s_PortraitWidth = s_AppWidth;
	s_PortraitHeight = s_AppHeight;
    
	int x = ( GetSystemMetrics(SM_CXSCREEN)/2) - (s_PortraitWidth/2);
	int y = ( GetSystemMetrics(SM_CYSCREEN)/2) - (s_PortraitHeight/2);

#ifndef BASE_DUMMY_APP	
	if( core::win::WinApp::GetWinHandle() )
		MoveWindow( core::win::WinApp::GetWinHandle(), x, y, s_PortraitWidth, s_PortraitHeight, TRUE );
#endif // !BASE_DUMMY_APP

	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotationStyle( renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM );
		renderer::OpenGL::GetInstance()->SetRotated( false );
	}
	
	// windows does this as is
	s_WidthScale = static_cast<float>(s_PortraitWidth) / static_cast<float>(s_BaseAssetWidth);
	s_HeightScale = static_cast<float>(s_PortraitHeight) / static_cast<float>(s_BaseAssetHeight);
#endif // BASE_PLATFORM_WINDOWS
    
#ifdef BASE_PLATFORM_iOS
    s_PortraitWidth = s_AppWidth;
    s_PortraitHeight = s_AppHeight;
    
	if( renderer::OpenGL::IsInitialised() )
		renderer::OpenGL::GetInstance()->SetRotated( true );

	s_WidthScale = static_cast<float>(s_PortraitWidth) / static_cast<float>(s_BaseAssetWidth);
	s_HeightScale = static_cast<float>(s_PortraitHeight) / static_cast<float>(s_BaseAssetHeight);
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_ANDROID
    s_PortraitWidth = s_AppWidth;
    s_PortraitHeight = s_AppHeight;
    
	if( renderer::OpenGL::IsInitialised() )
		renderer::OpenGL::GetInstance()->SetRotated( false );  
	//JAVA CALL: setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

	s_WidthScale = static_cast<float>(s_PortraitWidth) / static_cast<float>(s_BaseAssetWidth);
	s_HeightScale = static_cast<float>(s_PortraitHeight) / static_cast<float>(s_BaseAssetHeight);
#endif // BASE_PLATFORM_ANDROID
    

#ifdef BASE_PLATFORM_RASPBERRYPI
	s_PortraitWidth = s_AppWidth;
	s_PortraitHeight = s_AppHeight;

	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotationStyle( renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM );
		renderer::OpenGL::GetInstance()->SetRotated( false );
	}
	
	// windows does this as is
	s_WidthScale = static_cast<float>(s_PortraitWidth) / static_cast<float>(s_BaseAssetWidth);
	s_HeightScale = static_cast<float>(s_PortraitHeight) / static_cast<float>(s_BaseAssetHeight);
#endif // BASE_PLATFORM_RASPBERRYPI
	
#ifdef BASE_PLATFORM_MAC
	s_PortraitWidth = s_AppWidth;
	s_PortraitHeight = s_AppHeight;
	
	if( renderer::OpenGL::IsInitialised() )
	{
		renderer::OpenGL::GetInstance()->SetRotationStyle( renderer::VIEWROTATION_PORTRAIT_BUTTON_BOTTOM );
		renderer::OpenGL::GetInstance()->SetRotated( false );
	}
	
	// windows does this as is
	s_WidthScale = static_cast<float>(s_PortraitWidth) / static_cast<float>(s_BaseAssetWidth);
	s_HeightScale = static_cast<float>(s_PortraitHeight) / static_cast<float>(s_BaseAssetHeight);
#endif // BASE_PLATFORM_MAC
}

/////////////////////////////////////////////////////
/// Function: IsLandscape
/// 
///
/////////////////////////////////////////////////////
bool core::app::IsLandscape()
{
	return s_IsLandscape;
}

/////////////////////////////////////////////////////
/// Function: IsPortrait
/// 
///
/////////////////////////////////////////////////////
bool core::app::IsPortrait()
{
	return s_IsPortrait;
}

/////////////////////////////////////////////////////
/// Function: SetIsTablet
/// Params: [in] tablet
///
/////////////////////////////////////////////////////   
void core::app::SetIsTablet(bool tablet)
{
	s_IsTablet = tablet;
}

/////////////////////////////////////////////////////
/// Function: IsTablet
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IsTablet()
{
	return s_IsTablet;
}

/////////////////////////////////////////////////////
/// Function: SetIsRetinaDisplay
/// Params: [in] retina
///
/////////////////////////////////////////////////////   
void core::app::SetIsRetinaDisplay(bool retina)
{
	s_IsRetina = retina;
}

/////////////////////////////////////////////////////
/// Function: IsRetinaDisplay
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IsRetinaDisplay()
{
	return s_IsRetina;
}
	
/////////////////////////////////////////////////////
/// Function: SetIsPCOnly
/// Params: [in] state
///
/////////////////////////////////////////////////////   
void core::app::SetIsPCOnly(bool state)
{
	s_IsPCOnly = state;
}

/////////////////////////////////////////////////////
/// Function: IsPCOnly
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IsPCOnly()
{
	return s_IsPCOnly;
}

/////////////////////////////////////////////////////
/// Function: SetIstvOS
/// Params: [in] state
///
/////////////////////////////////////////////////////   
void core::app::SetIstvOS(bool state)
{
	s_IstvOS = state;
}

/////////////////////////////////////////////////////
/// Function: IstvOS
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IstvOS()
{
	return s_IstvOS;
}

/////////////////////////////////////////////////////
/// Function: SetIsSupportingAchievements
/// Params: [in] achievementSupport
///
/////////////////////////////////////////////////////   
void core::app::SetIsSupportingAchievements(bool achievementSupport)
{
	s_SupportAchievements = achievementSupport;
}

/////////////////////////////////////////////////////
/// Function: IsSupportingAchievements
/// Params: None
///
/////////////////////////////////////////////////////   
bool core::app::IsSupportingAchievements()
{
	return s_SupportAchievements;
}

/////////////////////////////////////////////////////
/// Function: SetIsGameCenterAvailable
/// Params: [in] gcSupport
///
/////////////////////////////////////////////////////   
void core::app::SetIsGameCenterAvailable(bool gcSupport)
{
	s_IsGameCenterAvailable = gcSupport;
}

/////////////////////////////////////////////////////
/// Function: IsGameCenterAvailable
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IsGameCenterAvailable()
{
	return s_IsGameCenterAvailable;
}

/////////////////////////////////////////////////////
/// Function: SetIsStoreAvailable
/// Params: [in] storeSupport
///
/////////////////////////////////////////////////////   
void core::app::SetIsStoreAvailable(bool storeSupport)
{
	s_IsStoreAvailable = storeSupport;
}

/////////////////////////////////////////////////////
/// Function: IsStoreAvailable
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IsStoreAvailable()
{
	return s_IsStoreAvailable;
}

/////////////////////////////////////////////////////
/// Function: IsStoreAvailable
/// 
///
/////////////////////////////////////////////////////   
void core::app::SetInBackground(bool inBackground)
{
	s_IsInBackground = inBackground;
}

/////////////////////////////////////////////////////
/// Function: IsStoreAvailable
/// 
///
/////////////////////////////////////////////////////   
bool core::app::IsInBackground()
{
	return s_IsInBackground;
}

/////////////////////////////////////////////////////
/// Function: SetLanguage
/// Params: [in] language
///
/////////////////////////////////////////////////////
void core::app::SetLanguage( int language )
{
    switch( language )
    {           
        case LANGUAGE_ENGLISH:
        {
            s_Language = core::LANGUAGE_ENGLISH;
        }break;
        case LANGUAGE_FRENCH:
        {
            s_Language = core::LANGUAGE_FRENCH;
        }break;
        case LANGUAGE_ITALIAN:
        {
            s_Language = core::LANGUAGE_ITALIAN;
        }break;
        case LANGUAGE_GERMAN:
        {
            s_Language = core::LANGUAGE_GERMAN;
        }break;
        case LANGUAGE_SPANISH:
        {
            s_Language = core::LANGUAGE_SPANISH;
        }break;    
        default:
            // default to english
            s_Language = core::LANGUAGE_ENGLISH;
            break;
    }
}

/////////////////////////////////////////////////////
/// Function: GetLanguage
/// 
///
/////////////////////////////////////////////////////
int core::app::GetLanguage()
{
    return s_Language;
}

/////////////////////////////////////////////////////
/// Function: SetAppWidth
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAppWidth( int width )
{
	s_AppWidth = width;
}

/////////////////////////////////////////////////////
/// Function: GetAppWidth
/// 
///
/////////////////////////////////////////////////////
int core::app::GetAppWidth()
{
	return s_AppWidth;
}

/////////////////////////////////////////////////////
/// Function: SetAppHeight
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAppHeight( int height )
{
	s_AppHeight = height;
}

/////////////////////////////////////////////////////
/// Function: GetAppHeight
/// 
///
/////////////////////////////////////////////////////
int core::app::GetAppHeight()
{
	return s_AppHeight;
}

/////////////////////////////////////////////////////
/// Function: SetBaseAssetsWidth
/// 
///
/////////////////////////////////////////////////////
void core::app::SetBaseAssetsWidth( int width )
{
	s_BaseAssetWidth = width;
}

/////////////////////////////////////////////////////
/// Function: GetBaseAssetsWidth
/// 
///
/////////////////////////////////////////////////////
int core::app::GetBaseAssetsWidth()
{
	return s_BaseAssetWidth;
}

/////////////////////////////////////////////////////
/// Function: SetBaseAssetsHeight
/// 
///
/////////////////////////////////////////////////////
void core::app::SetBaseAssetsHeight( int height )
{
	s_BaseAssetHeight = height;
}

/////////////////////////////////////////////////////
/// Function: GetBaseAssetsHeight
/// 
///
/////////////////////////////////////////////////////
int core::app::GetBaseAssetsHeight()
{
	return s_BaseAssetHeight;
}

/////////////////////////////////////////////////////
/// Function: GetOrientationWidth
/// 
///
/////////////////////////////////////////////////////
int core::app::GetOrientationWidth()
{
	if( s_IsLandscape )
		return(s_LandscapeWidth);

	return(s_PortraitWidth);
}

/////////////////////////////////////////////////////
/// Function: GetOrientationHeight
/// 
///
/////////////////////////////////////////////////////
int core::app::GetOrientationHeight()
{
	if( s_IsLandscape )
		return(s_LandscapeHeight);

	return(s_PortraitHeight);
}

/////////////////////////////////////////////////////
/// Function: GetWidthScale
/// 
///
/////////////////////////////////////////////////////
float core::app::GetWidthScale()
{
	return s_WidthScale;
}

/////////////////////////////////////////////////////
/// Function: GetHeightScale
/// 
///
/////////////////////////////////////////////////////
float core::app::GetHeightScale()
{
	return s_HeightScale;
}

/////////////////////////////////////////////////////
/// Function: GetNaughtyFlag
/// 
///
/////////////////////////////////////////////////////
void core::app::SetNaughtyFlag( bool flag )
{
	s_NaughtyFlag = flag;
}

/////////////////////////////////////////////////////
/// Function: GetNaughtyFlag
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetNaughtyFlag()
{
	return( s_NaughtyFlag );
}

/////////////////////////////////////////////////////
/// Function: SetSmoothUpdate
/// 
///
/////////////////////////////////////////////////////
void core::app::SetSmoothUpdate( bool flag )
{
	s_SmoothUpdates = flag;
}

/////////////////////////////////////////////////////
/// Function: GetNaughtyFlag
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetSmoothUpdate()
{
	return( s_SmoothUpdates );
}

/////////////////////////////////////////////////////
/// Function: SetSavePath
/// 
///
/////////////////////////////////////////////////////
void core::app::SetSavePath( const char* path )
{
	s_SavePath = path;
}

/////////////////////////////////////////////////////
/// Function: GetSavePath
/// 
///
/////////////////////////////////////////////////////
const char* core::app::GetSavePath()
{
	return s_SavePath;
}

/////////////////////////////////////////////////////
/// Function: SetLowPowerMode
/// 
///
/////////////////////////////////////////////////////
void core::app::SetLowPowerMode( bool flag )
{
	s_LowPowerMode = flag;
}

/////////////////////////////////////////////////////
/// Function: GetLowPowerMode
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetLowPowerMode()
{
	return( s_LowPowerMode );
}

/////////////////////////////////////////////////////
/// Function: SetUseVertexArrays
/// 
///
/////////////////////////////////////////////////////
void core::app::SetUseVertexArrays( bool flag )
{
	s_UseVertexArrays = flag;
}

/////////////////////////////////////////////////////
/// Function: GetUseVertexArrays
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetUseVertexArrays()
{
	return s_UseVertexArrays;
}

/////////////////////////////////////////////////////
/// Function: SupportiPodMusic
/// 
///
/////////////////////////////////////////////////////
void core::app::SupportiPodMusic( bool state )
{
	s_AllowiPodMusic = state;
}

/////////////////////////////////////////////////////
/// Function: GetSupportiPodMusic
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetSupportiPodMusic()
{
	return s_AllowiPodMusic;
}

/////////////////////////////////////////////////////
/// Function: SetAppRateURL
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAppRateURL( const char* url )
{
	s_StoreLinkToRate = url;
}

/////////////////////////////////////////////////////
/// Function: GetAppRateURL
/// 
///
/////////////////////////////////////////////////////
const char* core::app::GetAppRateURL()
{
	return s_StoreLinkToRate;
}

/////////////////////////////////////////////////////
/// Function: SetAdvertBarInfo
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAdvertBarInfo( const char* appAdFilterId, const char* localXMLPath, const char* externalXMLPath, int adBarSizePhone, int adBarSizeTablet, int adBarSizePhoneRetina, int adBarSizeTabletRetina, bool allowAdvertBarScaling )
{
	s_AdvertBarLocalAppFilter = appAdFilterId;
	s_AdvertBarLocalXMLPath = localXMLPath;
	s_AdvertBarExternalXMLPath = externalXMLPath;
	s_AdvertBarPhoneBarSize = adBarSizePhone;
	s_AdvertBarTabletBarSize = adBarSizeTablet;
	s_AdvertBarPhoneRetinaBarSize = adBarSizePhoneRetina;
	s_AdvertBarTabletRetinaBarSize = adBarSizeTabletRetina;
	s_AdvertBarAllowScaling = allowAdvertBarScaling;

/*
#ifdef BASE_PLATFORM_ANDROID
	// Get jmethod SetMobclixAppData from MobclixSupport class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaMobclixSupport = env->FindClass("base/Support/MobclixSupport");
	if( javaMobclixSupport != 0 )
	{
		jmethodID jmethodSetMobclixAppData = env->GetStaticMethodID(javaMobclixSupport, "SetMobclixAppData", "(IZ)V");

		jint barSize = s_MobclixPhoneBarSize;
		jboolean supportFullscreen = supportFullscreenAds;

		// call it
		env->CallStaticVoidMethod( javaMobclixSupport, jmethodSetMobclixAppData, barSize, supportFullscreenAds );
	}
#endif // BASE_PLATFORM_ANDROID
*/
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarAppFilter
/// 
///
/////////////////////////////////////////////////////
const char* core::app::GetAdvertBarAppFilter()
{
	return s_AdvertBarLocalAppFilter;
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarLocalXMLPath
/// 
///
/////////////////////////////////////////////////////
const char* core::app::GetAdvertBarLocalXMLPath()
{
	return s_AdvertBarLocalXMLPath;
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarExternalXMLPath
/// 
///
/////////////////////////////////////////////////////
const char* core::app::GetAdvertBarExternalXMLPath()
{
	return s_AdvertBarExternalXMLPath;
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarBarSizePhone
/// 
///
/////////////////////////////////////////////////////
int core::app::GetAdvertBarSizePhone()
{
	return s_AdvertBarPhoneBarSize;
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarBarSizeTablet
/// 
///
/////////////////////////////////////////////////////
int core::app::GetAdvertBarSizeTablet()
{
	return s_AdvertBarTabletBarSize;
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarSizePhoneRetina
/// 
///
/////////////////////////////////////////////////////
int core::app::GetAdvertBarSizePhoneRetina()
{
	return s_AdvertBarPhoneRetinaBarSize;
}

/////////////////////////////////////////////////////
/// Function: GetAdvertBarSizeTabletRetina
/// 
///
/////////////////////////////////////////////////////
int core::app::GetAdvertBarSizeTabletRetina()
{
	return s_AdvertBarTabletRetinaBarSize;
}

/////////////////////////////////////////////////////
/// Function: SetAdBarPosition
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAdBarPosition( float x, float y )
{
	s_AdvertBarAdPosX = x;
	s_AdvertBarAdPosY = y;
	
	s_AdvertBarAdPosX *= s_WidthScale;
	s_AdvertBarAdPosY *= s_HeightScale;
}

/////////////////////////////////////////////////////
/// Function: SetAdBarCenter
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAdBarCenter( float x, float y )
{
	s_AdvertBarAdCenterX = x;
	s_AdvertBarAdCenterY = y;
	
	s_AdvertBarAdCenterX *= s_WidthScale;
	s_AdvertBarAdCenterY *= s_HeightScale;
}

/////////////////////////////////////////////////////
/// Function: GetAdBarPosition
/// 
///
/////////////////////////////////////////////////////
void core::app::GetAdBarPosition( float *x, float *y )
{
	if( x != 0 )
		*x = s_AdvertBarAdPosX;

	if( y != 0 )
		*y = s_AdvertBarAdPosY;
}

/////////////////////////////////////////////////////
/// Function: GetAdBarCenter
/// 
///
/////////////////////////////////////////////////////
void core::app::GetAdBarCenter( float *x, float *y )
{
	if( x != 0 )
		*x = s_AdvertBarAdCenterX;
	
	if( y != 0 )
		*y = s_AdvertBarAdCenterY;
}

/////////////////////////////////////////////////////
/// Function: SetAdBarState
/// 
///
/////////////////////////////////////////////////////
void core::app::SetAdBarState( bool state )
{
	if( s_AdBarState == state )
		return;

	s_AdBarState = state;
}

/////////////////////////////////////////////////////
/// Function: CanAdBarScale
/// 
///
/////////////////////////////////////////////////////
bool core::app::CanAdBarScale()
{
	return s_AdvertBarAllowScaling;
}

/////////////////////////////////////////////////////
/// Function: GetAdBarState
/// 
///
/////////////////////////////////////////////////////
bool core::app::GetAdBarState()
{
	return s_AdBarState;
}

/////////////////////////////////////////////////////
/// Function: SetKiipKeys
///
///
/////////////////////////////////////////////////////
void core::app::SetKiipKeys( const char* appKey, const char* secretKey)
{
#ifdef BASE_SUPPORT_KIIP
	s_KiipAppKey = appKey;
	s_KiipSecretKey = secretKey;

	#ifdef BASE_PLATFORM_ANDROID
		// Get jmethod SetKiipKeys from KiipSupport class
		JNIEnv* env = 0;
		core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
		jclass javaKiipSupport = env->FindClass("base/Support/KiipSupport");
		if( javaKiipSupport != 0 )
		{
			jmethodID jmethodSetKiipKeys = env->GetStaticMethodID(javaKiipSupport, "SetKiipKeys", "(Ljava/lang/String;Ljava/lang/String;)V");

			jstring theAppKey = (jstring)env->NewStringUTF(appKey);
			jstring theSecretKey = (jstring)env->NewStringUTF(secretKey);

			// call it
			env->CallStaticVoidMethod( javaKiipSupport, jmethodSetKiipKeys, theAppKey, theSecretKey );
		}
	#endif // BASE_PLATFORM_ANDROID
#endif // BASE_SUPPORT_KIIP
}

/////////////////////////////////////////////////////
/// Function: GetKiipAppKey
///
///
/////////////////////////////////////////////////////
const char* core::app::GetKiipAppKey()
{
	return s_KiipAppKey;
}

/////////////////////////////////////////////////////
/// Function: GetKiipSecretKey
///
///
/////////////////////////////////////////////////////
const char* core::app::GetKiipSecretKey()
{
	return s_KiipSecretKey;
}

/////////////////////////////////////////////////////
/// Function: SetHandleMenuButton
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SetHandleMenuButton( bool state )
{
    s_HandleMenuButton = state;
}

/////////////////////////////////////////////////////
/// Function: GetHandleMenuButton
/// Params: None
///
/////////////////////////////////////////////////////
bool core::app::GetHandleMenuButton()
{
    return s_HandleMenuButton;
}

/////////////////////////////////////////////////////
/// Function: SetIconID
/// 
///
/////////////////////////////////////////////////////
void core::app::SetIconID( int resourceId )
{
	s_AppIconID = resourceId;
}
		
/////////////////////////////////////////////////////
/// Function: GetIconID
/// 
///
/////////////////////////////////////////////////////
int core::app::GetIconID()
{
	return s_AppIconID;
}

/////////////////////////////////////////////////////
/// Function: IsNetworkAvailable
/// Params: None
///
/////////////////////////////////////////////////////
bool core::app::IsNetworkAvailable()
{
#ifndef BASE_DUMMY_APP
	#if defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_MAC)	
		CFNetDiagnosticRef diag;        
		diag = CFNetDiagnosticCreateWithURL(kCFAllocatorDefault, (__bridge CFURLRef)[NSURL URLWithString:@"www.apple.com"]);
		
		CFNetDiagnosticStatus status;
		status = CFNetDiagnosticCopyNetworkStatusPassively(diag, 0);        
		
		CFRelease (diag);
		
		if ( status == kCFNetDiagnosticConnectionUp )
		{
			//DBGLOG("Connection is up");
			return true;
		} 
		else 
		{
			//DBGLOG("Connection is down");
			return false;
		}
	#endif // defined(BASE_PLATFORM_iOS) || defined(BASE_PLATFORM_MAC)			
#endif // !BASE_DUMMY_APP
	return false;
}


/////////////////////////////////////////////////////
/// Function: QuitApplication
/// Params: None
///
/////////////////////////////////////////////////////
void core::app::QuitApplication()
{
#ifndef BASE_DUMMY_APP
	#ifdef BASE_PLATFORM_WINDOWS
		PostQuitMessage(0);
	#endif // BASE_PLATFORM_WINDOWS

	#ifdef BASE_PLATFORM_RASPBERRYPI
		core::rpi::QuitEventHandler();
	#endif // BASE_PLATFORM_RASPBERRYPI
		
	#ifdef BASE_PLATFORM_MAC
		core::mac::QuitEventHandler();
	#endif // BASE_PLATFORM_MAC
#endif // !BASE_DUMMY_APP
}

#ifdef BASE_PLATFORM_tvOS
// SetViewController - needs to be set to show the GC pop ups
void core::app::SetViewController( GameViewController* controller )
{
    s_CurrentViewController = controller;
}

GameViewController* core::app::GetViewController()
{
    return s_CurrentViewController;
}
#endif


