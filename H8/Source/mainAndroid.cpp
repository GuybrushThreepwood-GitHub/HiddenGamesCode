
/*===================================================================
	File: mainAndroid.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifdef BASE_PLATFORM_ANDROID

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"

#include "Input/InputInclude.h"
#include "Input/Input.h"

#include "ScriptAccess/ScriptAccess.h"

#include "H8.h"
#include "H8Consts.h"

#include "zzip/zzip.h"

namespace
{
	H8* theApp = 0;

	static bool createCalled = false;
	static bool resInitialised = false;
	static bool resUpdated = false;

	static int screenWidth;
	static int screenHeight;
	static int screenRotation;

	char fullSavePath[core::MAX_PATH+core::MAX_PATH];
	ScriptDataHolder* pScriptHolder = 0;

	bool audioSystemPaused = false;
	snd::MusicPauseCall* musicPauseCall = 0;
}

// define the activity name for the build
#define MAKE_FN_NAME(x)			JNIEXPORT void JNICALL Java_uk_co_hiddengames_firewall_FirewallActivity_ ## x
#define FUNCTION_NAME(caller)	MAKE_FN_NAME(caller)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	/////////////////////////////////////////////////////
	/// Function: setAPK
	/// Params: [in]env, [in]thiz, [in]apkPath
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(setAPK)( JNIEnv* env, jobject thiz, jstring apkPath )
	{
		DBGLOG( "MAINANDROID: setAPK" );

		const char *buffer = env->GetStringUTFChars( apkPath, JNI_FALSE );
		
		core::app::SetLoadFilesFromZip(true);

		char fullAPKAndAssetsFolder[core::MAX_PATH+core::MAX_PATH];
		snprintf( fullAPKAndAssetsFolder, core::MAX_PATH+core::MAX_PATH, "%s/assets", buffer );

		core::app::SetRootZipFile(fullAPKAndAssetsFolder);

		DBGLOG( "MAINANDROID: APK path set %s", core::app::GetRootZipFile() );

		env->ReleaseStringUTFChars( apkPath, buffer );
	}

	/////////////////////////////////////////////////////
	/// Function: setSaveDirectory
	/// Params: [in]env, [in]thiz, [in]apkPath
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(setSaveDirectory)( JNIEnv* env, jobject thiz, jstring savePath )
	{
		DBGLOG( "MAINANDROID: setSaveDirectory" );

		const char *buffer = env->GetStringUTFChars( savePath, JNI_FALSE );
		
		snprintf( fullSavePath, core::MAX_PATH+core::MAX_PATH, "%s", buffer );

		env->ReleaseStringUTFChars( savePath, buffer );
	}

	/////////////////////////////////////////////////////
	/// Function: setTabletFlag
	/// Params: [in]env, [in]thiz, [in]tabletFlag
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(setTabletFlag)( JNIEnv* env, jobject thiz, jboolean tabletFlag )
	{
		DBGLOG( "MAINANDROID: setTabletFlag" );

		bool tablet = static_cast<bool>(tabletFlag);
		core::app::SetIsTablet( tablet );
	}

	/////////////////////////////////////////////////////
	/// Function: setRetinaFlag
	/// Params: [in]env, [in]thiz, [in]tabletFlag
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(setRetinaFlag)( JNIEnv* env, jobject thiz, jboolean retinaFlag )
	{
		DBGLOG( "MAINANDROID: setRetinaFlag" );

		bool retina = static_cast<bool>(retinaFlag);
		core::app::SetIsRetinaDisplay( retina );
	}

	/////////////////////////////////////////////////////
	/// Function: create
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(create)( JNIEnv* env, jobject thiz )
	{
		DBGLOG( "MAINANDROID: create" );

		theApp = new H8;
		theApp->SetSaveDirectory(fullSavePath);

		DBGLOG( "MAINANDROID: Save path set %s", theApp->GetSaveDirectory() );

		core::app::SetFlurryAPIKey( "" );

		core::app::SetAppRateURL( "" );

		musicPauseCall = snd::GetMusicPauseCall();
	}

	/////////////////////////////////////////////////////
	/// Function: execute
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(execute)( JNIEnv* env, jobject thiz )
	{
		//DBGLOG( "MAINANDROID: execute" );

		// update
		if( createCalled && 
			resInitialised )
		{
			if( !core::app::IsInBackground() )
				theApp->Execute();
		}
	}

	/////////////////////////////////////////////////////
	/// Function: resize
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(resize)( JNIEnv* env, jobject thiz, jint width, jint height, jint rotation )
	{
		DBGLOG( "MAINANDROID: resize" );

		screenRotation = static_cast<int>(rotation);
		
		if( !resInitialised )
		{
			screenWidth = static_cast<int>(width);
			screenHeight = static_cast<int>(height);
			
			DBGLOG( "MAINANDROID: resize %d x %d", screenWidth, screenHeight );
			
			if( !createCalled )
			{
				// create the application
				theApp->Create( screenWidth, screenHeight );
				createCalled = true;
			}

			resInitialised = true;
		}

		if( renderer::OpenGL::IsInitialised() )
		{
			switch( screenRotation )
			{
				case 0: // Surface.ROTATION_0
				{
					renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT);
				}break;
				case 1: // Surface.ROTATION_90
				{
					renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT);
				}break;
				case 2: // Surface.ROTATION_180
				{
					renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT);
				}break;
				case 3: // Surface.ROTATION_270
				{
					renderer::OpenGL::GetInstance()->SetRotationStyle(renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT);
				}break;
				default:
					DBGLOG( "MAINANDROID: unrecognised rotation\n" );
					break;
			}
		}
	}

	/////////////////////////////////////////////////////
	/// Function: destroy
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(destroy)( JNIEnv* env, jobject thiz )
	{
		DBGLOG( "MAINANDROID: destroy" );

		if( theApp != 0 )
		{
			theApp->Destroy();

			createCalled = false;
			resInitialised = false;

			// cleanup
			if( pScriptHolder != 0 )
			{
				delete pScriptHolder;
				pScriptHolder = 0;
			}

			// stop the script system
			script::LuaScripting::Shutdown();

			delete theApp;
			theApp = 0;

			musicPauseCall = 0;
		}
	}

	/////////////////////////////////////////////////////
	/// Function: appInBackground
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(appInBackground)( JNIEnv* env, jobject thiz )
	{
		DBGLOG( "MAINANDROID: appInBackground" );

		if( createCalled )
		{
			musicPauseCall = snd::GetMusicPauseCall();

			if( musicPauseCall != 0 )
				musicPauseCall->PauseMusic();

			if( AudioSystem::IsInitialised() )
			{
				audioSystemPaused = AudioSystem::GetInstance()->IsSystemPaused();

				if( !audioSystemPaused )
					AudioSystem::GetInstance()->Pause();
			}

			core::app::SetInBackground(true);
		}
	
	}

	/////////////////////////////////////////////////////
	/// Function: appInForeground
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(appInForeground)( JNIEnv* env, jobject thiz )
	{
		DBGLOG( "MAINANDROID: appInForeground" );

		if( createCalled )
		{
			musicPauseCall = snd::GetMusicPauseCall();

			if( musicPauseCall != 0 )
				musicPauseCall->UnPauseMusic();
	
			if( AudioSystem::IsInitialised() )
			{
				if( !audioSystemPaused )
					AudioSystem::GetInstance()->UnPause();
			}

			core::app::SetInBackground(false);
		}
	}

	/////////////////////////////////////////////////////
	/// Function: AddTouch
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(AddTouch)( JNIEnv* env, jobject thiz, jint index, jfloat x, jfloat y )
	{
		//DBGLOG( "MAINANDROID: AddTouch" );

		int touchIndex = static_cast<int>( index );

		input::AddTouch( false, touchIndex, static_cast<short>(x), static_cast<short>(y) );
	}

	/////////////////////////////////////////////////////
	/// Function: TouchMoved
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(TouchMoved)( JNIEnv* env, jobject thiz, jint index, jfloat x, jfloat y )
	{
		//DBGLOG( "MAINANDROID: TouchMoved" );

		int touchIndex = static_cast<int>( index );

		input::TouchMoved( false, touchIndex, static_cast<short>(x), static_cast<short>(y) );
	}

	/////////////////////////////////////////////////////
	/// Function: RemoveTouch
	/// Params: [in]env, [in]thiz
	///
	/////////////////////////////////////////////////////
	FUNCTION_NAME(RemoveTouch)( JNIEnv* env, jobject thiz, jint index )
	{
		//DBGLOG( "MAINANDROID: RemoveTouch" );

		int touchIndex = static_cast<int>( index );

		input::RemoveTouch( touchIndex );
	}
#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // BASE_PLATFORM_ANDROID