
/*===================================================================
	File: Cabby.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SupportBase.h"

// app includes
#include "CabbyConsts.h"

#include "Audio/AudioSystem.h"
#include "Audio/CustomerAudio.h"
#include "Cabby.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "Profiles/ProfileManager.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "ScriptAccess/ScriptAccess.h"

#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/TitleScreenUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/MainGameState.h"

#include "Resources/ModelResources.h"
#include "Resources/EmitterResources.h"

#include "Resources/ModelResources.h"
#include "Resources/AnimatedResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/EmitterResources.h"
#include "Resources/SoundResources.h"
#include "Resources/FontResources.h"
#include "Resources/StringResources.h"
#include "Resources/TextureResources.h"
#include "Resources/IAPList.h"
#include "Resources/AchievementList.h"

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
Cabby::Cabby()
{
	m_pGameSystems = 0;
	m_pScriptHolder = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
Cabby::~Cabby()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
int Cabby::Initialise( void )
{
	input::gInputState.nTouchWidth = static_cast<int>(TOUCH_SIZE_MENU);
	input::gInputState.nTouchHeight = static_cast<int>(TOUCH_SIZE_MENU);
	input::gInputState.nTouchRadius = static_cast<int>(TOUCH_SIZE_MENU);
	
	core::app::SetSavePath( m_szSaveFilePath );
	file::CreateDirectory( core::app::GetSavePath() );
	
	AudioSystem::Initialise(snd::MusicMode_PlaylistPlayer);
	snd::SetMusicPauseCall( AudioSystem::GetInstance() );

	script::LuaScripting::Initialise();

	// get the save file
	std::memset( m_ProfileName, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );

#ifdef CABBY_LITE
	sprintf( m_ProfileName, "%s/ProfileLite_2_0.sav", m_szSaveFilePath );
#elif  CABBY_FREE
	sprintf( m_ProfileName, "%s/ProfileFree_2_0.sav", m_szSaveFilePath );
#else
	sprintf( m_ProfileName, "%s/Profile_2_0.sav", m_szSaveFilePath );
#endif // CABBY_LITE

	DBGLOG( "Save File '%s'\n", m_ProfileName );
	
	m_pScriptHolder = new ScriptDataHolder();
	DBG_ASSERT_MSG( (m_pScriptHolder != 0), "Could not allocate script data holder" );
	
	// load the profiles
	ProfileManager::Initialise();

	RegisterScriptFunctions( *m_pScriptHolder );
	script::LuaScripting::GetInstance()->LoadScript( "lua/boot.lua" );
	script::LuaCallFunction( "BootInitialise", 0, 0 );

	ScriptDataHolder::DevScriptData devData = m_pScriptHolder->GetDevData();

	ProfileManager::GetInstance()->Setup();
	ProfileManager::GetInstance()->LoadProfile( m_ProfileName );
	//ProfileManager::GetInstance()->SaveProfileToScript();

	glHint(	GL_FOG_HINT, GL_NICEST );
	glHint(	GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glHint(	GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint(	GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glHint(	GL_POINT_SMOOTH_HINT, GL_FASTEST );

#ifdef BASE_PLATFORM_WINDOWS 
	core::app::SetIsRetinaDisplay( devData.isRetina );
	core::app::SetIsTablet( devData.isTablet );
	core::app::SetIsPCOnly( devData.isPCOnly );
	core::app::SetIstvOS(devData.istvOS);
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_ANDROID  
	core::app::SetIsSupportingAchievements(false);

    // use VBO on android
	core::app::SetUseVertexArrays( true );
	
    devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
#endif // BASE_PLATFORM_ANDROID
   
#ifdef BASE_PLATFORM_RASPBERRYPI 
	core::app::SetIsRetinaDisplay( devData.isRetina );
	core::app::SetIsTablet( devData.isTablet );
	core::app::SetIsPCOnly( devData.isPCOnly );
	core::app::SetUseVertexArrays( true );
	
	DBGLOG( "HTEMPLATE: res %d x %d\n", m_ScreenWidth, m_ScreenHeight );
	devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
#endif // BASE_PLATFORM_RASPBERRYPI

#ifdef BASE_PLATFORM_iOS
	core::app::SetLowPowerMode(true);
	core::app::SetIstvOS(false);
	devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
	#ifdef BASE_PLATFORM_tvOS
		core::app::SetIstvOS(true);
	#endif
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_MAC
	core::app::SetIsRetinaDisplay( true );
	core::app::SetIsTablet( true );
	core::app::SetIsPCOnly( devData.isPCOnly );
	core::app::SetIstvOS(devData.istvOS);
	core::app::SetUseVertexArrays( false );
	
	devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
#endif // BASE_PLATFORM_MAC
	
	core::app::SetAppWidth( devData.screenWidth );
	core::app::SetAppHeight( devData.screenHeight );
	core::app::SetBaseAssetsWidth( devData.assetBaseWidth );
	core::app::SetBaseAssetsHeight( devData.assetBaseHeight );

    core::app::SupportLandscape(true);
    core::app::SupportPortrait(false);
    
	core::app::SetLandscape(true);

    // set and load the correct language file
	int language = devData.language;
    devData.language = core::app::GetLanguage();
    
	// in debug use the script
#ifdef _DEBUG
	#ifdef BASE_PLATFORM_WINDOWS 
	if( language != devData.language )
	{
		devData.language = language;
	}
	#endif // BASE_PLATFORM_WINDOWS
#endif // _DEBUG

    lua_getglobal( script::LuaScripting::GetState(), "LoadLanguageFile" );
    lua_pushnumber( script::LuaScripting::GetState(), devData.language );
    lua_pcall( script::LuaScripting::GetState(), 1, 0, 0 );

	// register the resource list(s)
	res::CreateResourceMap();
	res::CreateAnimatedResourceMap();
	res::CreateSpriteResourceMap();
	res::CreateEmitterResourceMap();
	res::CreateSoundResourceMap();
	res::CreateFontResourceMap();
	res::CreateStringResourceMap();
	res::CreateTextureResourceMap();

	res::CreateIAPList();
	res::CreateAchievementList();

    lua_getglobal( script::LuaScripting::GetState(), "CabbyInitialise" );
    lua_pcall( script::LuaScripting::GetState(), 0, 0, 0 );

	LoadCustomerAudio();

	// used by title sequence
	//AudioSystem::GetInstance()->AddAudioFile( 130/*"flyby.wav"*/ );

	// used by all money deductions
	AudioSystem::GetInstance()->AddAudioFile( 105/*"money_big.wav"*/ );

	// level end
	AudioSystem::GetInstance()->AddAudioFile( 101/*"complete.wav"*/ );
	AudioSystem::GetInstance()->AddAudioFile( 103/*"fail.wav"*/ );

	ALuint bufferId = AudioSystem::GetInstance()->AddAudioFile( 131/*"select.wav"*/ );
	AudioSystem::GetInstance()->SetUIBufferId( bufferId );

	AudioSystem::GetInstance()->SetSFXState( ProfileManager::GetInstance()->GetProfile()->sfxState );

	// setup GL
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );
	renderer::OpenGL::GetInstance()->SetPerspective( GAME_FOV, static_cast<float>(core::app::GetOrientationWidth())/static_cast<float>(core::app::GetOrientationHeight()), 1.0f, 50.0f );
	renderer::OpenGL::GetInstance()->SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	renderer::OpenGL::GetInstance()->DisableSeparateSpecular();

	math::Vec3 pos( 0.0f, 0.0f, 0.0f );
	math::Vec3 target( 0.0f, 0.0f, -100.0f );
	gDebugCamera.SetPosition( pos ); 
	gDebugCamera.SetTarget( target );
	gDebugCamera.Enable();

	// create gamesystems
	m_pGameSystems = new GameSystems( m_Input );
	DBG_ASSERT_MSG( (m_pGameSystems != 0), "Could not allocate GameSystems" );

	m_pGameSystems->CreateGameCameraNoPlayer();

	// initialise
	m_pGameSystems->CreateScore();
	m_pGameSystems->CreatePurchase();
	
	// register in app purchase
	support::Purchase::GetInstance()->Initialise(res::GetIAPAsList());
	
	// turn off accelerometer
	core::app::SetAccelerometerState( false );
	
#ifdef CABBY_FREE
	GameData gameData = m_pScriptHolder->GetGameData();
			
	// adbar
	#ifdef BASE_SUPPORT_ADBAR
		core::app::SetAdvertBarInfo( devData.appAdFilterId, devData.localAdvertXML, devData.externalAdvertXML, core::ADBAR_PHONE_320x50, core::ADBAR_TABLET_728x90, core::ADBAR_PHONE_320x50, core::ADBAR_TABLET_728x90, devData.allowAdvertBarScaling );

		if( core::app::IsTablet() )
			core::app::SetAdBarPosition( gameData.ADBAR_TABLET_POSX, gameData.ADBAR_TABLET_POSY );
		else
			core::app::SetAdBarPosition( gameData.ADBAR_PHONE_POSX, gameData.ADBAR_PHONE_POSY );

		CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
		if( ret != CURLE_OK )
			DBG_ASSERT( 0 );

		m_AdBar = new renderer::AdvertBar;
		DBG_ASSERT( m_AdBar != 0 );

		m_AdBar->Init();
	#endif // BASE_SUPPORT_ADBAR
#endif // CABBY_FREE

	if( devData.levelTest )
	{
		// go through all the packs
		for( unsigned int i=0; i < m_pScriptHolder->GetLevelPackList().size(); ++i )
		{
			// through all the level
			for( int j=0; j < m_pScriptHolder->GetLevelPackList()[i]->packLevelCount; ++j )
			{
				if( std::strcmp( m_pScriptHolder->GetLevelPackList()[i]->pPackLevelInfo[j].levelName, m_pScriptHolder->GetDevData().levelTestName ) == 0 )
				{
					ProfileManager::GetInstance()->SetGameMode( ProfileManager::ARCADE_MODE );
					m_pScriptHolder->SetDefaultVehicle( devData.levelTestVehicle );
					ProfileManager::GetInstance()->SetArcadeModeVehicle( devData.levelTestVehicle );
					ProfileManager::GetInstance()->SetArcadeModeLevel( i, j );

					script::LuaCallFunction( m_pScriptHolder->GetLevelPackList()[i]->pPackLevelInfo[j].levelLoadFuncName, 0, 0 );

					DBG_MEMTRY
						m_TaxiStateManager.ChangeState( new TitleScreenState(m_TaxiStateManager, m_Input) );
					DBG_MEMCATCH

					m_UIStateManager.SetSecondaryManager( &m_TaxiStateManager );

					DBG_MEMTRY
						m_UIStateManager.ChangeState( new LevelStartUI(m_UIStateManager, m_Input ) );	
					DBG_MEMCATCH

					m_TaxiStateManager.SetSecondaryManager( &m_UIStateManager );
				}
			}
		}
	}
	else
	{
/*		DBG_MEMTRY
			m_TaxiStateManager.ChangeState( new ProfileSelectState(m_TaxiStateManager, m_Input) );
		DBG_MEMCATCH

		m_UIStateManager.SetSecondaryManager( &m_TaxiStateManager );

		DBG_MEMTRY
			m_UIStateManager.ChangeState( new UIStateCredits(m_UIStateManager, m_Input ) );	
		DBG_MEMCATCH

		m_TaxiStateManager.SetSecondaryManager( &m_UIStateManager );
*/
		DBG_MEMTRY
			m_TaxiStateManager.ChangeState( new TitleScreenState(m_TaxiStateManager, m_Input) );
		DBG_MEMCATCH

		m_UIStateManager.SetSecondaryManager( &m_TaxiStateManager );

		DBG_MEMTRY
			m_UIStateManager.ChangeState( new TitleScreenUI(m_UIStateManager, m_Input ) );	
		DBG_MEMCATCH

		m_TaxiStateManager.SetSecondaryManager( &m_UIStateManager );
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: FrameMove
/// Params: None
///
/////////////////////////////////////////////////////
int Cabby::FrameMove( void )
{
	AudioSystem::GetInstance()->Update( m_ElapsedTime );
	
	if( GameSystems::IsInitialised() )
	{
		GameSystems::GetInstance()->Update( m_ElapsedTime );
	}

	m_TaxiStateManager.Update( m_ElapsedTime );
	m_UIStateManager.Update( m_ElapsedTime );

#ifdef BASE_SUPPORT_ADBAR
	if( core::app::GetAdBarState() )
	{
		if( m_AdBar != 0 )
			m_AdBar->Update( m_ElapsedTime );
	}
#endif // BASE_SUPPORT_ADBAR

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Render
/// Params: None
///
/////////////////////////////////////////////////////
int Cabby::Render( void )
{
	Input( );

	m_TaxiStateManager.Draw();
	m_UIStateManager.Draw();

#ifdef BASE_SUPPORT_ADBAR
	if( core::app::GetAdBarState() )
	{
		// draw the adbar
		if( m_AdBar != 0 )
			m_AdBar->Render();
	}
#endif // BASE_SUPPORT_ADBAR

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Input
/// Params: None
///
/////////////////////////////////////////////////////
void Cabby::Input( void )
{
	if( m_Input.GetEscape() )
		core::app::QuitApplication();
}

/////////////////////////////////////////////////////
/// Method: Cleanup
/// Params: None
///
/////////////////////////////////////////////////////
int Cabby::Cleanup( void )
{
	// set null state to properly exit
	//AudioSystem::GetInstance()->StopBGMTrack();
	input::InitialiseInput();

#ifdef BASE_SUPPORT_ADBAR
	if( m_AdBar != 0 )
	{
		delete m_AdBar;
		m_AdBar = 0;
	}

	curl_global_cleanup();
#endif // BASE_SUPPORT_ADBAR

	m_TaxiStateManager.ChangeState( 0 );
	m_UIStateManager.ChangeState( 0 );

	ProfileManager::Shutdown();

	res::ClearSpriteResources();
	res::ClearEmitterResources();
	res::ClearSoundResources();
	res::ClearFontResources();
	res::ClearStringResources();
	res::ClearTextureResources();
	res::ClearIAPList();
	res::ClearAchievementList();

	if( m_pGameSystems != 0 )
	{	
		m_pGameSystems->DestroyScore();
		m_pGameSystems->DestroyPurchase();

		m_pGameSystems->DestroyGameCamera();

		delete m_pGameSystems;
		m_pGameSystems = 0;
	}

	if( m_pScriptHolder != 0 )
	{
		delete m_pScriptHolder;
		m_pScriptHolder = 0;
	}

	AudioSystem::Shutdown();

	script::LuaScripting::Shutdown();

	return(0);
}

