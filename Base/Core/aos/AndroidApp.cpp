
/*===================================================================
	File: AndroidApp.cpp
	Library: CoreLib

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "CoreBase.h"

#include "Math/RandomTables.h"

#include "Input/Input.h"
#include "Math/Vectors.h"
#include "Render/OpenGLCommon.h"
#include "Render/FF/OpenGLES/OpenGLES.h"
#include "Render/GLSL/OpenGLSL.h"
#include "Sound/OpenAL.h"
#include "Network/NetworkFunctions.h"

using core::aos::AndroidApp;
using input::gInputState;

static int nUserScreenWidth = core::WINDOW_WIDTH;
static int nUserScreenHeight = core::WINDOW_HEIGHT;
	
namespace
{
	bool bIsAppRunning = false;
	bool accelerometerState = false;

	struct timeval frameStartTime, frameEndTime;
}

/////////////////////////////////////////////////////
/// Function: QuitEventHandler
/// Params: None
///
/////////////////////////////////////////////////////
bool core::aos::QuitEventHandler()
{
	bIsAppRunning = false;
	return false;
}

/////////////////////////////////////////////////////
/// Default constructor
/// 
///
/////////////////////////////////////////////////////
AndroidApp::AndroidApp()
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
AndroidApp::~AndroidApp()
{

}

/////////////////////////////////////////////////////
/// Method: Create
/// Params: None
///
/////////////////////////////////////////////////////
int AndroidApp::Create( int screenWidth, int screenHeight )
{
	core::app::SetPlatform( core::PLATFORM_ANDROID );
	core::app::SetLoadFilesFromZip( true );

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
	JNIEnv *env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);

	// get default locale object
	jclass javaLocale = env->FindClass("java/util/Locale");
	jmethodID jmethodGetDefault = env->GetStaticMethodID(javaLocale, "getDefault", "()Ljava/util/Locale;");
	jobject defaultLocale = env->CallStaticObjectMethod(javaLocale, jmethodGetDefault);

	// ask for language code
	jmethodID jmethodGetLanguage = env->GetMethodID(javaLocale, "getLanguage", "()Ljava/lang/String;");
	jstring jLanguageCode = (jstring) env->CallObjectMethod(defaultLocale, jmethodGetLanguage);
		
	// get C string
	const char* languageCode = env->GetStringUTFChars(jLanguageCode, JNI_FALSE);

	FilterLanguage( languageCode ); 
	
	env->ReleaseStringUTFChars(jLanguageCode, languageCode);

	input::InitialiseInput();
			
	// initialise OpenGL
	renderer::OpenGL::Initialise();

	// context
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;

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
		DBGLOG( "ANDROIDAPP: Call to Initialise() failed!\n" );
		return(1);
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Execute
/// Params: None
///
/////////////////////////////////////////////////////
int AndroidApp::Execute( void )
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

	for( i=0; i < input::MAX_TOUCHES; ++i )
	{
		if( input::gInputState.TouchesData[i].bActive )
		{
			if( input::gInputState.TouchesData[i].bPress )
			{
				input::gInputState.TouchesData[i].Ticks += m_ElapsedTime;
					
				if( input::gInputState.TouchesData[i].Ticks > input::HOLD_TIME )
					input::gInputState.TouchesData[i].bHeld = true;
			}
		}
	}

	// call the update method
	FrameMove();

	// clear the screen
	renderer::OpenGL::GetInstance()->ClearScreen();

	// render anything
	Render();

	// check for AL errors
	snd::OpenAL::GetInstance()->CheckALErrors();

	// flip the buffer
	renderer::OpenGL::GetInstance()->Flip();

	input::UpdateTouches( m_ElapsedTime );
				
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Destroy
/// Params: None
///
/////////////////////////////////////////////////////
void AndroidApp::Destroy( void )
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

	exit(0);
}

/////////////////////////////////////////////////////
/// Method: Run
/// Params: [in]EngineProc, [in]context
///
/////////////////////////////////////////////////////
int AndroidApp::Run( void (*EngineProc)(void *context), void *context )
{						
	return(0);
}

/////////////////////////////////////////////////////
/// Method: FilterLanguage
/// Params: [in]languageString
///
/////////////////////////////////////////////////////
void AndroidApp::FilterLanguage( const char* languageString )
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

}

/////////////////////////////////////////////////////
/// Function: SetAccelerometerState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void core::app::SetAccelerometerState( bool state, float frequency )
{
	// Get jmethod SetAccelerometerState from SensorInput class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaSensorInput = env->FindClass("base/Input/InputSensors");
	if( javaSensorInput != 0 )
	{
		jmethodID jmethodSetAccelerometerState = env->GetStaticMethodID(javaSensorInput, "SetAccelerometerState", "(ZF)V");

		jboolean jState = state;
		jfloat jFreq = frequency;

		// call it
		env->CallStaticVoidMethod( javaSensorInput, jmethodSetAccelerometerState, jState, jFreq );
	}
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
	// Get jmethod OpenWebLink from CoreUtility class
	JNIEnv* env = 0;
	core::aos::GetVM()->GetEnv( (void **) &env, JNI_VERSION_1_6);
	jclass javaCoreUtility = env->FindClass("base/Core/CoreUtility");
	if( javaCoreUtility != 0 )
	{
		jmethodID jmethodOpenWebLink = env->GetStaticMethodID(javaCoreUtility, "OpenWebLink", "(Ljava/lang/String;)V");

		// Create Java strings for parameters
		jstring jUrl = env->NewStringUTF(url);

		// open link
		env->CallStaticVoidMethod( javaCoreUtility, jmethodOpenWebLink, jUrl );
	}
}

#endif // BASE_PLATFORM_ANDROID

