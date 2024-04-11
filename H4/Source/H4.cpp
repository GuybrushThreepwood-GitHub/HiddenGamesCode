
/*===================================================================
	File: H4.cpp
	Game: H4

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

// app includes
#include "AppConsts.h"
#include "GameConsts.h"

#include "Audio/AudioSystem.h"
#include "ScriptAccess/ScriptAccess.h"

#include "H4.h"
#include "GameSystems.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

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

#include "GameStates/FrontendState.h"
#include "GameStates/MainState.h"
#include "GameStates/CreditRollState.h"
#include "GameStates/EndingState.h"

bool H4::m_HiResMode = true;

namespace
{
	//input::Input debugInput;
}

/////////////////////////////////////////////////////
/// Function: SetHiResMode
/// Params: [in]flag
///
/////////////////////////////////////////////////////
void H4::SetHiResMode( bool flag )
{
	m_HiResMode = flag;
}

/////////////////////////////////////////////////////
/// Function: GetHiResMode
/// Params: None
///
/////////////////////////////////////////////////////
bool H4::GetHiResMode()
{
	return m_HiResMode;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
H4::H4()
{
	m_pScriptHolder = 0;
	m_pGameSystems = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
H4::~H4()
{
	if( m_pScriptHolder != 0 )
	{
		delete m_pScriptHolder;
		m_pScriptHolder = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
int H4::Initialise( void )
{	
	input::gInputState.nTouchWidth = static_cast<int>(TOUCH_SIZE);
	input::gInputState.nTouchHeight = static_cast<int>(TOUCH_SIZE);
	input::gInputState.nTouchRadius = static_cast<int>(TOUCH_SIZE);
	
	core::app::SetSavePath( m_szSaveFilePath );
	file::CreateDirectory( core::app::GetSavePath() );

	AudioSystem::Initialise();
	script::LuaScripting::Initialise();

	m_pScriptHolder = new ScriptDataHolder();
	DBG_ASSERT_MSG( (m_pScriptHolder != 0), "Could not allocate script data holder" );

	RegisterScriptFunctions( *m_pScriptHolder );
	script::LuaScripting::GetInstance()->LoadScript( "lua/boot.lua" );
	script::LuaCallFunction( "BootInitialise", 0, 0 );

	ScriptDataHolder::DevScriptData devData = m_pScriptHolder->GetDevData();
	H4::SetHiResMode( devData.hiResMode );
	
	glHint(	GL_FOG_HINT, GL_NICEST );
	glHint(	GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glHint(	GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
	glHint(	GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glHint(	GL_POINT_SMOOTH_HINT, GL_FASTEST );

#ifdef BASE_PLATFORM_WINDOWS 
	core::app::SetIsRetinaDisplay( devData.isRetina );
	core::app::SetIsTablet( devData.isTablet );
#endif // BASE_PLATFORM_WINDOWS

#ifdef BASE_PLATFORM_ANDROID  
	core::app::SetIsSupportingAchievements(false);
    // use VBO on android
	core::app::SetUseVertexArrays( true );

    devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;

	H4::SetHiResMode( true );
#endif // BASE_PLATFORM_ANDROID
	
#ifdef BASE_PLATFORM_RASPBERRYPI
	core::app::SetIsRetinaDisplay( true );
	core::app::SetIsTablet( true );
	//core::app::SetIsPCOnly( true );
	core::app::SetUseVertexArrays( true );
	H4::SetHiResMode( true );
	
	DBGLOG( "HTEMPLATE: res %d x %d\n", m_ScreenWidth, m_ScreenHeight );
	devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
#endif // BASE_PLATFORM_RASPBERRYPI
    
#ifdef BASE_PLATFORM_iOS
	H4::SetHiResMode( false );

	if( m_IsSimulator )
	{
		// use whatever is in script
		H4::SetHiResMode( devData.hiResMode );
	}
	else
	{
		// work out the min hardware for the hi res mode
		// iPod
        if( m_IsiPod )
		{
			// iPod 4,1 - iPod 4th Gen
			if( m_HardwareMajorNum > 4 )
			{
				// higher major num
				H4::SetHiResMode( true );
			}
			else
			if( m_HardwareMajorNum == 4 )
			{
				// check minor is greater or equal
				if( m_HardwareMinorNum >= 1 )
					H4::SetHiResMode( true );
			}
		}
		else if( m_IsiPhone )
		{
			// iPhone 3,1 - iPhone 4th Gen
			if( m_HardwareMajorNum > 3 )
			{
				// higher major num
				H4::SetHiResMode( true );
			}
			else
			if( m_HardwareMajorNum == 3 )
			{
				// check minor is greater or equal
				if( m_HardwareMinorNum >= 1 )
					H4::SetHiResMode( true );
			}
		}
		else if( m_IsiPad )
		{
			// iPad 1,1 - iPad 1st gen
			if( m_HardwareMajorNum > 1 )
			{
				// higher major num
				H4::SetHiResMode( true );
			}
			else
			if( m_HardwareMajorNum == 1 )
			{
				// check minor is greater or equal
				if( m_HardwareMinorNum >= 1 )
					H4::SetHiResMode( true );
			}
		}
		else if( m_IsAppleTV )
		{
			// AppleTV 2,1 - AppleTV 2nd gen
			if( m_HardwareMajorNum > 2 )
			{
				// higher major num
				H4::SetHiResMode( true );
			}
			else
			if( m_HardwareMajorNum == 2 )
			{
				// check minor is greater or equal
				if( m_HardwareMinorNum >= 1 )
					H4::SetHiResMode( true );
			}
		}
		else if( m_UnknownDevice )
		{
			H4::SetHiResMode( true );
		}
	}

	devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
#endif // BASE_PLATFORM_iOS

#ifdef BASE_PLATFORM_MAC
	core::app::SetIsRetinaDisplay( true );
	core::app::SetIsTablet( true );
	core::app::SetIsSupportingAchievements(false);
	core::app::SetUseVertexArrays( false );
	
    devData.screenWidth = m_ScreenWidth;
	devData.screenHeight = m_ScreenHeight;
	
	H4::SetHiResMode( true );
#endif // BASE_PLATFORM_MAC
	
	core::app::SetAppWidth( devData.screenWidth );
	core::app::SetAppHeight( devData.screenHeight );
	core::app::SetBaseAssetsWidth( devData.assetBaseWidth );
	core::app::SetBaseAssetsHeight( devData.assetBaseHeight );
	
    core::app::SupportLandscape(true);
    core::app::SupportPortrait(false);
    
	core::app::SetLandscape(true);

	// register the resource list(s)
	res::CreateResourceMap();
	res::CreateAnimatedResourceMap();
	res::CreateSpriteResourceMap();
	res::CreateEmitterResourceMap();
	res::CreateSoundResourceMap();
	res::CreateFontResourceMap();
	res::CreateStringResourceMap();
	
	if( H4::GetHiResMode() )
		res::CreateTextureResourceMap();

	res::CreateIAPList();
	res::CreateAchievementList();

	// setup GL
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );
	renderer::OpenGL::GetInstance()->SetPerspective( GAME_FOV, static_cast<float>(core::app::GetOrientationWidth())/static_cast<float>(core::app::GetOrientationHeight()), NEAR_CLIP, FAR_CLIP );
	renderer::OpenGL::GetInstance()->SetClearBits( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	renderer::OpenGL::GetInstance()->DisableSeparateSpecular();

	// create gamesystems
	m_pGameSystems = new GameSystems( m_Input );
	DBG_ASSERT_MSG( (m_pGameSystems != 0), "Could not allocate GameSystems" );

	// initialise
	m_pGameSystems->CreateScore();
	m_pGameSystems->CreatePurchase();
	
	// register in app purchase
	support::Purchase::GetInstance()->Initialise(res::GetIAPAsList());

	// turn off accelerometer
	core::app::SetAccelerometerState( false );

	math::Vec3 camPos( 0.0f, 0.0f, 0.0f );
	math::Vec3 camTarget( 0.0f, 0.0f, -100.0f );

	gDebugCamera.SetPosition( camPos );
	gDebugCamera.SetTarget( camTarget );
	gDebugCamera.Disable();

	// call the level jump when saving is disabled
	if( !devData.enableSave )
		script::LuaCallFunction( "PostInitialise", 0, 0 );

	// load the options
	GameSystems::GetInstance()->LoadOptions();

	GameSystems::GetInstance()->RegisterSaveVariables();

	GameSystems::GetInstance()->SetCorrectStage();
	
	if( devData.bootState == 0 )
	{
		// front end
		m_MainStateManager.SetSecondaryManager( 0 );

		DBG_MEMTRY
			m_MainStateManager.ChangeState( new FrontendState(m_MainStateManager, m_Input) );
		DBG_MEMCATCH
	}
	else if( devData.bootState == 1 )
	{
		// main game
		m_MainStateManager.SetSecondaryManager( 0 );

		DBG_MEMTRY
			m_MainStateManager.ChangeState( new MainState(m_MainStateManager, m_Input) );
		DBG_MEMCATCH
	}
	else if( devData.bootState == 2 )
	{
		// intro
		m_MainStateManager.SetSecondaryManager( 0 );

		DBG_MEMTRY
			m_MainStateManager.ChangeState( new EndingState( 0, m_MainStateManager, m_Input) );
		DBG_MEMCATCH
	}
	else if( devData.bootState == 3 )
	{
		// ending
		m_MainStateManager.SetSecondaryManager( 0 );

		DBG_MEMTRY
			m_MainStateManager.ChangeState( new EndingState( 1, m_MainStateManager, m_Input) );
		DBG_MEMCATCH
	}
	else if( devData.bootState == 4 )
	{
#ifdef USE_CREDITROLLSTATE
		// credits
		m_MainStateManager.SetSecondaryManager( 0 );

		DBG_MEMTRY
			m_MainStateManager.ChangeState( new CreditRollState( m_MainStateManager, m_Input) );
		DBG_MEMCATCH
#endif // USE_CREDITROLLSTATE
	}
	
	return(0);
}

/////////////////////////////////////////////////////
/// Method: FrameMove
/// Params: None
///
/////////////////////////////////////////////////////
int H4::FrameMove( void )
{
	/*if( debugInput.IsKeyPressed( input::KEY_F1, true, true ) )
	{
		static int achievement = 1;
		GameSystems::GetInstance()->AwardAchievement(achievement++);
	}*/

	AudioSystem::GetInstance()->Update( m_ElapsedTime );
	
	if( GameSystems::IsInitialised() )
	{
		GameSystems::GetInstance()->Update( m_ElapsedTime );
	}

	m_MainStateManager.Update( m_ElapsedTime );

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Render
/// Params: None
///
/////////////////////////////////////////////////////
int H4::Render( void )
{
	Input( );

	m_MainStateManager.Draw();

	// always draw
	GameSystems::GetInstance()->DrawAchievementUI();

	return(0);
}

/////////////////////////////////////////////////////
/// Method: Input
/// Params: None
///
/////////////////////////////////////////////////////
void H4::Input( void )
{	
	if( m_Input.GetEscape() )
		core::app::QuitApplication();
}

/////////////////////////////////////////////////////
/// Method: Cleanup
/// Params: None
///
/////////////////////////////////////////////////////
int H4::Cleanup( void )
{
	// set null state to properly exit
	input::InitialiseInput();

	m_MainStateManager.ChangeState( 0 );

	if( AudioSystem::IsInitialised() )
	{
		AudioSystem::GetInstance()->ClearBGMTrack();
		AudioSystem::Shutdown();
	}

	res::ClearSpriteResources();
	res::ClearEmitterResources();
	res::ClearSoundResources();
	res::ClearFontResources();
	res::ClearStringResources();

	//if( H4::GetHiResMode() )
		res::ClearTextureResources();
	
	if( m_pGameSystems != 0 )
	{
		m_pGameSystems->DestroyScore();
		m_pGameSystems->DestroyPurchase();

		m_pGameSystems->DestroyGameCamera();

		delete m_pGameSystems;
		m_pGameSystems = 0;
	}

	// stop the script system
	script::LuaScripting::Shutdown();

	return(0);
}

