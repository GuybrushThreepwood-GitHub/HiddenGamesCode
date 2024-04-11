
/*===================================================================
	File: MainGameState.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Model/ModelCommon.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "H1Consts.h"
#include "H1.h"

#include "Effects/Emitter.h"
#include "Physics/PhysicsContact.h"
#include "Physics/RayObject.h"
#include "Resources/ModelResources.h"
#include "Resources/EmitterResources.h"
#include "Resources/StringResources.h"
#include "GameStates/IBaseGameState.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/H1/LightAccess.h"
#include "ScriptAccess/H1/FogAccess.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/LevelCompleteUI.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/MainGameState.h"

#include "Profiles/ProfileManager.h"

namespace
{
	const float RESPAWN_TIMER = 2.5f;
	const float ROTATION_PER_FRAME = 30.0f;
	const float COUNTDOWN_TIME = 4.0f;

	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	input::Input debugInput;
	bool drawGameUI = true;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MainGameState::MainGameState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
, m_pScriptData(0)
{
	m_LastDelta = 0.0f;

	m_LevelStartWait = true;
	m_LevelCompleteMode = false;

	m_WasPausedLastFrame = false;

	m_CountdownTime = 0.0f; 

	m_CalledRespawnFade = false;

	m_pLevelData = 0;

	m_DeadFirstFrame = true;
	m_PlayerDeadTimer = 0.0f;

	m_RainSrcId = snd::INVALID_SOUNDSOURCE;
	m_RainBufferId = snd::INVALID_SOUNDBUFFER;

	GameSystems::GetInstance()->SetLevel( &m_Level );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainGameState::~MainGameState()
{
	GameSystems::GetInstance()->SetPlayer( 0 );

	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}

	//PhysicsWorld::SetPlayer( 0 );

	m_Level.Release();

	// clear the vehicle
	m_pScriptData->ReleaseVehicleList();

	GameSystems::GetInstance()->SetLevel( 0 );
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Enter()
{
	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();
	m_LevelData = m_pScriptData->GetLevelData();
	m_GameData = m_pScriptData->GetGameData();
	m_PhysicsData = m_pScriptData->GetPhysicsData();

	m_GameCamera = GameSystems::GetInstance()->GetGameCamera();

	m_GameMode = GameSystems::GetInstance()->GetGameMode();
	int gameModeIndex = GameSystems::GetInstance()->GetGameModeIndex();
	int targetResId = -1;
	if( m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		if( gameModeIndex == 0 )
			targetResId = m_LevelData.objectTargetId1;
		else if( gameModeIndex == 1 )
			targetResId = m_LevelData.objectTargetId2;
		else if( gameModeIndex == 2 )
			targetResId = m_LevelData.objectTargetId3;
		else
			DBG_ASSERT(0);
	}

	m_LevelIndex = (ProfileManager::GetInstance()->GetModePackId() * m_GameData.MAX_ITEMS_PER_PACK) + ProfileManager::GetInstance()->GetModeLevelId();
	m_PlaneIndex = m_pScriptData->GetDefaultVehicle();

	renderer::OpenGL::GetInstance()->SetMSAAState(true);
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	GameSystems::GetInstance()->CreatePhysics();
	GameSystems::GetInstance()->ResetTargetsDestroyed();
	GameSystems::GetInstance()->ResetObjectsDestroyed();
	GameSystems::GetInstance()->ResetLaps();
	GameSystems::GetInstance()->ResetGatesComplete();

	m_LastCountObjectsDestroyed = GameSystems::GetInstance()->GetObjectsDestroyed();

	// load from script
	script::LuaCallFunction( "PostPhysicsSetup", 0, 0 );

	m_Level.Initialise();
	m_Level.LoadMeshData( m_LevelData.levelModel, m_DevData.useAABBCull );
	m_Level.LoadComplexPhysicsData( m_LevelData.levelPhysicsComplex );
	m_Level.CreatePhysics();
	m_Level.LoadData( m_LevelData.levelData, (m_GameMode == GameSystems::GAMEMODE_TARGET), targetResId, m_LevelData.objectTargetId1, m_LevelData.objectTargetId2, m_LevelData.objectTargetId3 );
	m_Level.LoadMappingData( m_LevelData.coverage, &m_LevelData.spriteBatches[0], m_LevelData.numUsedSpriteBatches );
	
	if( m_GameData.ALLOW_DETAIL_MAP )
		m_Level.DetailMapSetup( m_LevelData.detailMap, m_LevelData.detailMapScaleX, m_LevelData.detailMapScaleY );

	math::Vec3 spawnPos, spawnRot;
	if( m_GameMode == GameSystems::GAMEMODE_RINGRUN )
	{
		m_Level.SetGateGroup( gameModeIndex );
		spawnPos = m_Level.GetPlayerStart();
		spawnRot = m_Level.GetPlayerStartRot();
	}
	else if( m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		m_Level.GetRandomSpawn( &spawnPos, &spawnRot );
		
		if( gameModeIndex == 0 )
		{
			m_Level.SetTargetObjectId( m_LevelData.objectTargetId1 );
			GameSystems::GetInstance()->SetGameModeExtraData(m_LevelData.objectTargetId1);
		}
		else if( gameModeIndex == 1 )
		{
			m_Level.SetTargetObjectId( m_LevelData.objectTargetId2 );
			GameSystems::GetInstance()->SetGameModeExtraData(m_LevelData.objectTargetId2);
		}
		else if( gameModeIndex == 2 )
		{
			m_Level.SetTargetObjectId( m_LevelData.objectTargetId3 );
			GameSystems::GetInstance()->SetGameModeExtraData(m_LevelData.objectTargetId3);
		}
		else
			DBG_ASSERT(0);
	}
	else if( m_GameMode == GameSystems::GAMEMODE_FREEFLIGHT )
	{
		m_Level.GetRandomSpawn( &spawnPos, &spawnRot );
	}

	// call the vehicle setup
	int vehicleId = m_pScriptData->GetDefaultVehicle();

	// get pack and vehicle offset
	int packId = vehicleId / m_GameData.MAX_ITEMS_PER_PACK;
	int vehicleOffsetId = vehicleId % m_GameData.MAX_ITEMS_PER_PACK;

	m_Player = new Player( m_InputSystem, m_pScriptData->GetVehiclePackList()[packId]->pVehicleScriptData[vehicleOffsetId], m_Level );
	DBG_ASSERT( m_Player != 0 );
	m_Player->LoadPhysicsData( m_pScriptData->GetVehiclePackList()[packId]->pVehicleScriptData[vehicleOffsetId].physicsData );
	m_Player->Setup();

	m_Player->SetRespawnPosition(spawnPos, spawnRot);
	m_Player->Respawn();

	GameSystems::GetInstance()->SetPlayer( m_Player );
	m_Level.SetPlayer( m_Player );

	m_GameTime = 0.0f;
	m_TokenTimerStarted = false;
	m_TokenCollectCountdownTimer = m_GameData.TOKEN_COLLECT_TIME;

	if( m_DevData.allowDebugCam )
	{
		math::Vec3 pos( 0.0f, 0.0f, 50.0f );
		math::Vec3 target( 0.0f, 0.0f, -500.0f );
		gDebugCamera.SetPosition( pos );
		gDebugCamera.SetTarget( target ); 
		gDebugCamera.Disable();
	}

	int randNum = math::RandomNumber(0, 10);

	if( randNum >= 0 && randNum < 4 )
		m_TimeOfDay = Level::LEVEL_DAY;
	else if( randNum >= 4 && randNum < 9)
		m_TimeOfDay = Level::LEVEL_AFTERNOON;
	else
		m_TimeOfDay = Level::LEVEL_NIGHT;

	switch( m_TimeOfDay )
	{
		// day
		case Level::LEVEL_DAY:
		{
			m_Level.SetTimeOfDay( Level::LEVEL_DAY );
			m_LevelFog = GetFogData( m_LevelData.levelFogDay );
			m_LevelLight = GetLightData( m_LevelData.levelDayLight );
			m_Level.SetupSkybox( m_LevelData.levelBGTextureDay, m_LevelData.levelBGDayUseFog, m_LevelData.levelSkyboxOffset );

		}break;
		// afternoon
		case Level::LEVEL_AFTERNOON:
		{
			m_Level.SetTimeOfDay( Level::LEVEL_AFTERNOON );
			m_LevelFog = GetFogData( m_LevelData.levelFogAfternoon );
			m_LevelLight = GetLightData( m_LevelData.levelAfternoonLight );
			m_Level.SetupSkybox( m_LevelData.levelBGTextureAfternoon, m_LevelData.levelBGAfternoonUseFog, m_LevelData.levelSkyboxOffset ); 
		}break;
		// night
		case Level::LEVEL_NIGHT:
		{
			m_Level.SetTimeOfDay( Level::LEVEL_NIGHT );
			m_LevelFog = GetFogData( m_LevelData.levelFogNight );
			m_LevelLight = GetLightData( m_LevelData.levelNightLight );
			m_Level.SetupSkybox( m_LevelData.levelBGTextureNight, m_LevelData.levelBGNightUseFog, m_LevelData.levelSkyboxOffset ); 
		}break;
		default:
		{
			m_LevelFog.bFogState = false;
			m_LevelLight = GetLightData( "defaultLightSettings" );	
		}break;
		
	}

	m_LevelFog.fFogFarClip *= m_GameData.FARCLIP_MODIFY;
	
	m_LevelLightBackup = m_LevelLight;
	m_LevelFogBackup = m_LevelFog;

	m_PlayerNightLight = GetLightData( "nightPlayerLightSettings" );

	renderer::OpenGL::GetInstance()->EnableLight( 0 );

	renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
	renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
	renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

	math::Vec4 lightPos( m_Player->GetPosition() );
	lightPos.X += m_PlayerNightLight.lightPos.X;
	lightPos.Y += m_PlayerNightLight.lightPos.Y;
	lightPos.Z += m_PlayerNightLight.lightPos.Z;
	lightPos.W = m_PlayerNightLight.lightPos.W;
	renderer::OpenGL::GetInstance()->SetLightPosition( 1, lightPos );	
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 1, m_PlayerNightLight.lightDiffuse );
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 1, m_PlayerNightLight.lightDiffuse );
	renderer::OpenGL::GetInstance()->SetLightAmbient( 1, m_PlayerNightLight.lightAmbient );
	renderer::OpenGL::GetInstance()->SetLightSpecular( 1, m_PlayerNightLight.lightSpecular );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 1, GL_CONSTANT_ATTENUATION, m_PlayerNightLight.lightAttenuation.X );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 1, GL_LINEAR_ATTENUATION, m_PlayerNightLight.lightAttenuation.Y );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 1, GL_QUADRATIC_ATTENUATION, m_PlayerNightLight.lightAttenuation.Z );

	m_pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( m_pLevelData != 0 );

	m_pLevelData->targetsDestroyed = 0;
	if( m_GameMode == GameSystems::GAMEMODE_TARGET )
		m_pLevelData->targetsTotal = m_Level.GetTotalTargets();
	else
		m_pLevelData->targetsTotal = 0;
	m_pLevelData->time = 6039.0f;

	// turn accelerometer on with 30 updates per sec
	//if( true )
	//	core::app::SetAccelerometerState(true, ACCELEROMETER_FREQ);
	//else
		core::app::SetAccelerometerState(false);

	// ODE
	physics::ShowPhysicsBodies(true);
	dWorldSetGravity( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.gravityX, m_PhysicsData.gravityY, m_PhysicsData.gravityZ );
	dWorldSetAutoDisableLinearThreshold( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.autoDisableLinearThreshold );
	dWorldSetAutoDisableAngularThreshold( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.autoDisableAngularThreshold );
	dWorldSetAutoDisableSteps( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.autoDisableSteps );
	dWorldSetAutoDisableTime( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.autoDisableTime );

	dWorldSetCFM( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.CFM );
	dWorldSetERP( physics::PhysicsWorldODE::GetWorld(), 0.8f );

	dWorldSetAutoDisableFlag( physics::PhysicsWorldODE::GetWorld(), 1 );
	dWorldSetLinearDamping( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.linearDamping );
	dWorldSetAngularDamping( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.angularDamping );
	//dWorldSetMaxAngularSpeed( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.m );
	dWorldSetContactMaxCorrectingVel( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.contactMaxCorrectingVel );
	dWorldSetContactSurfaceLayer( physics::PhysicsWorldODE::GetWorld(), m_PhysicsData.contactSurfaceLayer );
	dWorldSetQuickStepNumIterations( physics::PhysicsWorldODE::GetWorld(), 10 );

	m_GameCamera->SetLerpSpeed( 5.0f );
	m_GameCamera->SetLerp( false );

	math::Vec3 playerPos = m_Player->GetPosition();
	math::Vec3 playerRot = m_Player->GetAngles();
	math::Vec3 playerDir = m_Player->GetDirection();
	//math::Vec3 playerUp = m_Player->GetUpVector();

	math::Vec3 origPos = playerPos;

	origPos = math::Vec3( playerPos.X + m_GameData.CAMERA_X_OFFSET, playerPos.Y + m_GameData.CAMERA_Y_OFFSET, playerPos.Z - m_GameData.CAMERA_Z_OFFSET );

	math::Vec3 camPos = math::RotateAroundPoint( 0, origPos, playerPos, math::DegToRad(playerRot.X) );
	camPos = math::RotateAroundPoint( 1, camPos, playerPos, math::DegToRad(playerRot.Y) );
	
	m_GameCamera->SetPosition( camPos );

	math::Vec3 camTarget = playerPos + (playerDir*m_GameData.CAMERA_TARGET_MULTIPLY);

	m_GameCamera->SetTarget( camTarget );

	math::Vec3 upVec( 0.0f, 1.0f, 0.0f );
	m_GameCamera->SetUpVector( upVec );
	m_GameCamera->SetRotation( playerRot );
	m_GameCamera->SetLerp( false );

	GameSystems::GetInstance()->CreateEffectPool( 10, 10 );

	m_InitCountdown = false;
	m_LevelStartWait = false;

	// effects
	m_IsSnowing = false;

	if( m_LevelData.allowSnow &&
		m_GameData.ALLOW_SNOW_EFFECT )
	{
		if( math::RandomNumber( 0, 10 ) <= m_LevelData.snowProbability )
			m_IsSnowing = true;
	}

	m_IsRaining = false;
	if( m_LevelData.allowRain &&
		m_GameData.ALLOW_RAIN_EFFECT )
	{
		if( math::RandomNumber( 0, 10 ) <= m_LevelData.rainProbability )
			m_IsRaining = true;
	}

	m_FlyingEffect = new efx::Emitter;
	DBG_ASSERT( m_FlyingEffect != 0 );

	const res::EmitterResourceStore* er = 0;

	er = res::GetEmitterResource( 100 );
	m_FlyingEffect->Create( *er->block );
	res::SetupTexturesOnEmitter( m_FlyingEffect );
	m_FlyingEffect->SetBoundingCheckState( false );

	m_SnowEffect = new efx::Emitter;
	DBG_ASSERT( m_SnowEffect != 0 );

	er = res::GetEmitterResource( 101 );
	m_SnowEffect->Create( *er->block );
	res::SetupTexturesOnEmitter( m_SnowEffect );
	m_SnowEffect->SetBoundingCheckState( false );

	if( m_IsSnowing )
		m_SnowEffect->Enable();

	m_RainEffect = new efx::Emitter;
	DBG_ASSERT( m_RainEffect != 0 );

	er = res::GetEmitterResource( 102 );
	m_RainEffect->Create( *er->block );
	res::SetupTexturesOnEmitter( m_RainEffect );
	m_RainEffect->SetBoundingCheckState( false );

	if( m_IsRaining )
		m_RainEffect->Enable();

	// 
	m_BoundingAreaEffect = res::LoadModel( 141 );
	DBG_ASSERT( m_BoundingAreaEffect != 0 );
	m_BoundingAreaEnabled = false;
	m_BoundingAlpha = 0;

	// achievements
	m_AchAircraftTimer		= m_GameData.ACH_PLANE_USED_TIMER;
	m_AchLevelPlayedTimer	= m_GameData.ACH_LEVEL_PLAYED_TIMER;
	m_AchWeatherTimer		= m_GameData.ACH_WEATHER_RAINSNOW_TIMER;
	m_AchTimeOfDayTimer		= m_GameData.ACH_TIMEOFDAY_TIMER;
	m_AchFreeFlightTimer	= m_GameData.ACH_FREEFLIGHT_MODE_TIMER;

	m_FlightTime = 0.0f;

	m_RainSrcId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	m_RainBufferId = AudioSystem::GetInstance()->AddAudioFile( 1003/*light_rain.wav*/ );
	if( m_IsRaining )
	{
		AudioSystem::GetInstance()->PlayAudio( m_RainSrcId, m_RainBufferId, zeroVec, AL_TRUE, AL_TRUE, 1.0f, 1.0f );
		alSourcePause(m_RainSrcId);
	}

	m_TimerTick = AudioSystem::GetInstance()->AddAudioFile( 106/*"timer_tick.wav"*/ );
	m_TimerEnd = AudioSystem::GetInstance()->AddAudioFile( 107/*"timer_end.wav"*/ );
	m_TickTime = 0.0f; 

	GameSystems::GetInstance()->InitAudio();

	// do just one frame set up
	Draw();
	Update( 0.01f );
	Draw();

	m_InitCountdown = true;
	m_LevelStartWait = true;

	// everything is loaded, inform the Start Level UI
	m_pState = 0;
	m_pState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( m_pState->GetId() == UI_LEVELSTART )
	{
		LevelStartUI* pUIState = static_cast<LevelStartUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
		pUIState->SetFinishedLoading();

		m_LevelStartWait = true;
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Exit()
{
	m_GameCamera = 0;

	if( m_FlyingEffect != 0 )
	{
		delete m_FlyingEffect;
		m_FlyingEffect = 0;
	}
	if( m_SnowEffect != 0 )
	{
		delete m_SnowEffect;
		m_SnowEffect = 0;
	}
	if( m_RainEffect != 0 )
	{
		delete m_RainEffect;
		m_RainEffect = 0;
	}
	if( m_BoundingAreaEffect != 0 )
	{
		res::RemoveModel( m_BoundingAreaEffect );
		m_BoundingAreaEffect = 0;
	}

	ProfileManager::LevelBest bestScore;
	m_GameMode = GameSystems::GetInstance()->GetGameMode();
	int packId = ProfileManager::GetInstance()->GetModePackId();
	int levelId = ProfileManager::GetInstance()->GetModeLevelId();
	int gameModeIndex = GameSystems::GetInstance()->GetGameModeIndex();
	
	// try and save the target mode highest just to show you played it
	if( m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		DBG_ASSERT( m_pLevelData != 0 );
		m_pLevelData->targetsDestroyed = GameSystems::GetInstance()->GetTargetsDestroyed();
		m_pLevelData->targetsTotal = m_Level.GetTotalTargets();
		m_pLevelData->time = 6039.0f;

		if( ProfileManager::GetInstance()->GetScore( packId, levelId, m_GameMode, gameModeIndex, &bestScore ) )
		{
			if( m_pLevelData->targetsDestroyed >= bestScore.targetsDestroyed )
			{
				bestScore.targetsDestroyed = m_pLevelData->targetsDestroyed;

				// don't allow the time save if it's not 100% complete
				if( m_pLevelData->targetsDestroyed != m_pLevelData->targetsTotal )
					m_pLevelData->time = 6039.0f;

				if( m_pLevelData->time <= bestScore.bestTime )
					bestScore.bestTime = m_pLevelData->time;
				
				ProfileManager::GetInstance()->AddScore( true, packId, levelId, m_GameMode, gameModeIndex, bestScore.targetsDestroyed, bestScore.targetsTotal, bestScore.bestTime );
			}
		}
		else
		{
			ProfileManager::GetInstance()->AddScore( true, packId, levelId, m_GameMode, gameModeIndex, m_pLevelData->targetsDestroyed, m_pLevelData->targetsTotal, 6039.0f );
		}
	}

	ProfileManager::GetInstance()->GetProfile()->flightTime += m_FlightTime;
	ProfileManager::GetInstance()->SaveProfile();

	GameSystems::GetInstance()->DestroyEffectPool();

	// turn off lights for possible complete state
	renderer::OpenGL::GetInstance()->DisableLight( 0 );
	renderer::OpenGL::GetInstance()->DisableLight( 1 );

	core::app::SetAccelerometerState(false);
	
	renderer::OpenGL::GetInstance()->SetMSAAState(false);

	GameSystems::GetInstance()->DestroyPhysics();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int MainGameState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int MainGameState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Update( float deltaTime )
{
	if( m_LevelStartWait ||
		m_LevelCompleteMode )
		return;

	m_pState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	MainGameUI* pUIState = 0;

	if( m_pState->GetId() == UI_MAINGAME )
	{
		pUIState = static_cast<MainGameUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		if( m_InitCountdown )
		{
			pUIState->SetCountdownState( true );
			m_CountdownTime = COUNTDOWN_TIME;
			m_TickTime = 1.0f;
			pUIState->SetCountdown( static_cast<int>(m_CountdownTime) );

			m_InitCountdown = false;
		}
	}

	m_LastDelta = deltaTime;

//#ifdef _DEBUG
	if( m_DevData.allowDebugCam )
	{
		if( debugInput.IsKeyPressed( input::KEY_9, true ) )
		{
			if( gDebugCamera.IsEnabled() )
				gDebugCamera.Disable();	
			else
				gDebugCamera.Enable();
		}
	}	

	if( debugInput.IsKeyPressed( input::KEY_1, true ) )
	{
		m_LevelLight = GetLightData( "defaultLightSettings" );
		m_LevelFog.bFogState = false;
		
		m_Level.SetTimeOfDay( Level::LEVEL_DAY );
		m_TimeOfDay = Level::LEVEL_DAY;

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );
	}
	else if( debugInput.IsKeyPressed( input::KEY_2, true ) )
	{
		m_LevelLight = GetLightData( m_LevelData.levelDayLight );
		m_LevelFog = GetFogData( m_LevelData.levelFogDay );
		m_LevelFog.fFogFarClip *= m_GameData.FARCLIP_MODIFY;

		m_Level.SetupSkybox( m_LevelData.levelBGTextureDay, m_LevelData.levelBGDayUseFog, m_LevelData.levelSkyboxOffset ); 
		m_Level.SetTimeOfDay( Level::LEVEL_DAY );
		m_TimeOfDay = Level::LEVEL_DAY;

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );
	}
	else if( debugInput.IsKeyPressed( input::KEY_3, true ) )
	{
		m_LevelLight = GetLightData( m_LevelData.levelAfternoonLight );
		m_LevelFog = GetFogData( m_LevelData.levelFogAfternoon );
		m_LevelFog.fFogFarClip *= m_GameData.FARCLIP_MODIFY;

		m_Level.SetupSkybox( m_LevelData.levelBGTextureAfternoon, m_LevelData.levelBGAfternoonUseFog, m_LevelData.levelSkyboxOffset ); 
		m_Level.SetTimeOfDay( Level::LEVEL_AFTERNOON );
		m_TimeOfDay = Level::LEVEL_AFTERNOON;

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );
	}
	else if( debugInput.IsKeyPressed( input::KEY_4, true ) )
	{
		m_LevelLight = GetLightData( m_LevelData.levelNightLight );
		m_LevelFog = GetFogData( m_LevelData.levelFogNight );
		m_LevelFog.fFogFarClip *= m_GameData.FARCLIP_MODIFY;

		m_Level.SetupSkybox( m_LevelData.levelBGTextureNight, m_LevelData.levelBGNightUseFog, m_LevelData.levelSkyboxOffset ); 
		m_Level.SetTimeOfDay( Level::LEVEL_NIGHT );
		m_TimeOfDay = Level::LEVEL_NIGHT;

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );
	}

	// update the active camera
	if( m_DevData.allowDebugCam )
	{
		if( gDebugCamera.IsEnabled() )
			gDebugCamera.Update( deltaTime );
	}
//#endif // _DEBUG

	if( GetStateManager()->GetSecondaryStateManager() )
	{
		if( m_pState->GetId() == UI_MAINGAME )
		{
			if( !pUIState->IsPaused() )
			{
				// 3-2-1 timer
				if( m_CountdownTime > 0.0f )
				{
					m_CountdownTime -= deltaTime;

					if( m_CountdownTime < 0.0f )
					{
						m_CountdownTime = -99.0f;

						//disable countdown text
						pUIState->SetCountdownState( false );
					}
					else if( m_CountdownTime < 1.0f )
					{
						// show GO!
						pUIState->SetCountdown( -1 );

						if( m_TickTime != -1.0f )
						{
							AudioSystem::GetInstance()->PlayAudio( m_TimerEnd, zeroVec, true );
							m_TickTime = -1.0f;
						}

						// start audio
						m_Player->StartAudio();
					}
					else
					{
						m_TickTime += deltaTime;
						
						if( m_TickTime >= 1.0f )
						{
							AudioSystem::GetInstance()->PlayAudio( m_TimerTick, zeroVec, true );
							m_TickTime = 0.0f;
						}

						pUIState->SetCountdown( static_cast<int>(m_CountdownTime) );

						m_Player->StopAllAudio();

						return;
					}
				}
			}

			if( pUIState->IsPaused() )
			{
				if( !m_WasPausedLastFrame )
				{	
					// update token timer
					pUIState->SetShowTokenTime( m_TokenTimerStarted );
					pUIState->SetTokenCountdown( m_TokenCollectCountdownTimer );
					pUIState->SetTotalFlightTime( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) );

					int taku=0;
					int mei=0;
					int earl=0; 
					int fio=0;
					int mito=0; 
					int uki=0;

					m_Level.GetCollectedTokenCounts( &taku, &mei, &earl, &fio, &mito, &uki );

					pUIState->SetTokenCounts( taku, mei, earl, fio, mito, uki );

					// pause is on
					AudioSystem::GetInstance()->Pause();

					m_WasPausedLastFrame = true;
				}
				return;
			}
			else
			{
				// pause off
				if( m_WasPausedLastFrame )
				{
					AudioSystem::GetInstance()->UnPause();

					m_WasPausedLastFrame = false;
				}
			}

			// update game time
			if( !pUIState->IsLevelComplete() )
			{
				if( GameSystems::GetInstance()->GetGameMode() == GameSystems::GAMEMODE_RINGRUN )
				{
					m_GameTime += deltaTime;
					
					char tmpString[32];
					snprintf( tmpString, 32, "%d / %d", GameSystems::GetInstance()->GetGatesComplete(), GameSystems::GetInstance()->GetGatesTotal()-1 );

					if( GameSystems::GetInstance()->GetLapsCompleted() == m_GameData.RING_MODE_LAPS )
					{
						//snprintf( tmpString, 32, "%d / %d", m_GameData.RING_MODE_LAPS, m_GameData.RING_MODE_LAPS );
						snprintf( tmpString, 32, "%d / %d", GameSystems::GetInstance()->GetGatesTotal()-1, GameSystems::GetInstance()->GetGatesTotal()-1 );

						DBG_ASSERT( m_pLevelData != 0 );
						m_pLevelData->targetsDestroyed = 0;
						m_pLevelData->targetsTotal = 0;
						m_pLevelData->time = m_GameTime;

						//  mode is over
						m_LevelCompleteMode = true;
						pUIState->DoLevelComplete();
						
						AudioSystem::GetInstance()->Pause();
						if( m_IsRaining )
							alSourcePause(m_RainSrcId);

						GameSystems::GetInstance()->PlayLevelCompleteAudio();
						GameSystems::GetInstance()->AwardAchievement( 69, -1 );
					}					
					
					pUIState->SetGamePoints( tmpString );
					pUIState->SetGameTime( m_GameTime );
				}
				else if( GameSystems::GetInstance()->GetGameMode() == GameSystems::GAMEMODE_TARGET )
				{
					m_GameTime += deltaTime;

					char tmpString[32];
					snprintf( tmpString, 32, "%d / %d", GameSystems::GetInstance()->GetTargetsDestroyed(), m_Level.GetTotalTargets() );

					pUIState->SetGamePoints( tmpString );
					pUIState->SetGameTime( m_GameTime );

					if( GameSystems::GetInstance()->GetTargetsDestroyed() == m_Level.GetTotalTargets() )
					{						
						
						DBG_ASSERT( m_pLevelData != 0 );
						m_pLevelData->targetsDestroyed = GameSystems::GetInstance()->GetTargetsDestroyed();
						m_pLevelData->targetsTotal = m_Level.GetTotalTargets();
						m_pLevelData->time = m_GameTime;

						//  mode is over
						m_LevelCompleteMode = true;
						pUIState->DoLevelComplete();

						AudioSystem::GetInstance()->Pause();
						if( m_IsRaining )
							alSourcePause(m_RainSrcId);

						GameSystems::GetInstance()->PlayLevelCompleteAudio();
						GameSystems::GetInstance()->AwardAchievement( 70, -1 );
					
					}
				}
			}
		}
	}

	m_Player->CheckAgainstLevel( m_Level );

	if( m_Player->IsDead() )
	{
		if( m_DeadFirstFrame )
		{
			m_PlayerDeadTimer = RESPAWN_TIMER;
			
			m_DeadFirstFrame = false;
		}

		m_PlayerDeadTimer -= deltaTime;

		if( m_PlayerDeadTimer <= 0.0f )
		{
			if( m_pState->GetId() == UI_MAINGAME &&
				!pUIState->IsPaused() )
			{
				if( !m_CalledRespawnFade )
				{
					pUIState->DoRespawnFadeToBlack();
					m_CalledRespawnFade = true;
				}
			}	

			// do the repsawn
			if( m_Player->IsDead() )
			{
				if( m_pState->GetId() == UI_MAINGAME &&
					!pUIState->IsPaused() &&
					pUIState->IsRespawnFadeToBlackComplete() )
				{
					m_PlayerDeadTimer = 0.0f;

					m_Player->Respawn();
					m_Player->StartAudio();

					// update ui
					if( GetStateManager()->GetSecondaryStateManager() )
					{
						if( m_pState->GetId() == UI_MAINGAME )
						{
							float playerSpeed = m_Player->GetTargetSpeed();
							float range = m_Player->GetSpeedRange().maxVal - m_Player->GetSpeedRange().minVal;
							float val = playerSpeed - m_Player->GetSpeedRange().minVal;

							pUIState->SetSpeedLever( val / range );
						}
					}

					m_FlyingEffect->Reset();
					m_FlyingEffect->Disable();

					m_CalledRespawnFade = false;
				}
			}
		}
	} 
	else
		m_DeadFirstFrame = true;

#if defined(BASE_PLATFORM_WINDOWS) || defined(BASE_PLATFORM_tvOS)

		bool accelState = m_InputSystem.GetAccel();
		bool decelState = m_InputSystem.GetDecel();

		if( accelState || decelState )
		{
			if( GetStateManager()->GetSecondaryStateManager() )
			{
				if( m_pState->GetId() == UI_MAINGAME )
				{
					float playerSpeed = m_Player->GetTargetSpeed();
					float range = m_Player->GetSpeedRange().maxVal - m_Player->GetSpeedRange().minVal;
					float val = playerSpeed - m_Player->GetSpeedRange().minVal;

					pUIState->SetSpeedLever( val / range );
				}
			}
		}
#endif // BASE_PLATFORM_WINDOWS


	m_Player->Update( deltaTime );
	m_Level.Update( deltaTime );

	// check token start
	if( m_Level.TokenCollectedLastFrame() &&
		!m_TokenTimerStarted )
	{
		m_TokenTimerStarted = true;
	}
	else if( m_Level.AllCadetTokensCollected() &&
		m_TokenTimerStarted )
	{
		m_TokenTimerStarted = false;

		// within the time
		if( m_TokenCollectCountdownTimer > 0.0f )
		{
			GameSystems::GetInstance()->AwardAchievement( 20+m_LevelIndex, -1 );
		}
	}

	if( m_TokenTimerStarted )
	{
		m_TokenCollectCountdownTimer -= deltaTime;
		if( m_TokenCollectCountdownTimer < 0.0f )
			m_TokenCollectCountdownTimer = 0.0f;
	}

	math::Vec3 playerPos = m_Player->GetPosition();
	math::Vec3 playerRot = m_Player->GetAngles();
	math::Vec3 playerDir = m_Player->GetDirection();
	math::Vec3 playerUp = m_Player->GetUpVector();
	float distanceToGround = m_Player->GetDistanceToGround();

	math::Vec3 origPos = playerPos;

	origPos = math::Vec3( playerPos.X + m_GameData.CAMERA_X_OFFSET, playerPos.Y + m_GameData.CAMERA_Y_OFFSET, playerPos.Z - m_GameData.CAMERA_Z_OFFSET );

	math::Vec3 camPos = math::RotateAroundPoint( 0, origPos, playerPos, math::DegToRad(playerRot.X) );
	camPos = math::RotateAroundPoint( 1, camPos, playerPos, math::DegToRad(playerRot.Y) );
	
	m_GameCamera->SetPosition( camPos );

	math::Vec3 camTarget = playerPos + (playerDir*m_GameData.CAMERA_TARGET_MULTIPLY);

	m_GameCamera->SetTarget( camTarget );

	m_GameCamera->SetUpVector( playerUp );
	m_GameCamera->SetRotation( playerRot );
	m_GameCamera->Update( deltaTime );

	GameSystems::GetInstance()->UpdateEffectPool( deltaTime );

	bool nearEdges = false;
	if( m_BoundingAreaEffect != 0 )
	{
		if( playerPos.X <= m_GameData.MIN_BOUNDING_X+m_GameData.WALL_FADE_START_DISTANCE )
			nearEdges = true;
		else
		if( playerPos.Z <= m_GameData.MIN_BOUNDING_Z+m_GameData.WALL_FADE_START_DISTANCE )
			nearEdges = true;
		else
		if( playerPos.X >= m_GameData.MAX_BOUNDING_X-m_GameData.WALL_FADE_START_DISTANCE )
			nearEdges = true;
		else
		if( playerPos.Y >= m_GameData.MAX_BOUNDING_Y-m_GameData.WALL_FADE_START_DISTANCE )
			nearEdges = true;
		else
		if( playerPos.Z >= m_GameData.MAX_BOUNDING_Z-m_GameData.WALL_FADE_START_DISTANCE )
			nearEdges = true;

		if( nearEdges )
		{
			m_BoundingAlpha+=m_GameData.WALL_FADE_IN_INC;
			math::Clamp( &m_BoundingAlpha, 0, 255 );
		}
		else
		{
			m_BoundingAlpha-=m_GameData.WALL_FADE_OUT_DEC;
			math::Clamp( &m_BoundingAlpha, 0, 255 );
		}

		if( m_BoundingAlpha <= 0 )
			m_BoundingAreaEnabled = false;
		else
			m_BoundingAreaEnabled = true;
			
		m_BoundingAreaEffect->Update( deltaTime );
	}

	if( m_FlyingEffect != 0 )
	{
		if( playerPos.X <= m_GameData.MIN_BOUNDING_X ||
			playerPos.Z <= m_GameData.MIN_BOUNDING_Z ||
			playerPos.X >= m_GameData.MAX_BOUNDING_X ||
			playerPos.Y >= m_GameData.MAX_BOUNDING_Y ||
			playerPos.Z >= m_GameData.MAX_BOUNDING_Z )
		{
			m_FlyingEffect->Disable();
			m_FlyingEffect->Stop();
		}
		else
		{
			if( m_GameData.ALLOW_WIND_EFFECT &&
				!m_Player->IsDead() &&
				distanceToGround >= m_GameData.FLYING_WIND_ENABLE_HEIGHT )
			{
				if( !m_FlyingEffect->IsEnabled() )
					m_FlyingEffect->Enable();

				m_FlyingEffect->SetPos( playerPos+(playerDir*m_GameData.FLYING_WIND_DISTANCE_IN_FRONT) );
			
			}
			else
			{
				if( m_FlyingEffect->IsEnabled() )
					m_FlyingEffect->Disable();
			}
		}
		
		m_FlyingEffect->Update( deltaTime );
	}	
	
	if( m_SnowEffect != 0 )
	{
		math::Vec3 spawnPos = m_GameCamera->GetPosition();
		spawnPos.Y += m_GameData.SNOW_HEIGHT_ABOVE_PLAYER;

		m_SnowEffect->SetPos( spawnPos+(playerDir*m_GameData.SNOW_DISTANCE_IN_FRONT) );
		m_SnowEffect->Update( deltaTime );
	}

	if( m_RainEffect != 0 )
	{
		math::Vec3 spawnPos = m_GameCamera->GetPosition();
		spawnPos.Y += m_GameData.RAIN_HEIGHT_ABOVE_PLAYER;

		m_RainEffect->SetPos( spawnPos+(playerDir*m_GameData.RAIN_DISTANCE_IN_FRONT) );
		m_RainEffect->Update( deltaTime );
	}

	if( m_pState->GetId() == UI_MAINGAME &&
		m_IsRaining &&
		!m_LevelCompleteMode)
	{
		if( snd::SoundManager::GetInstance()->GetSoundState(m_RainSrcId) != AL_PLAYING )
			AudioSystem::GetInstance()->PlayAudio( m_RainSrcId, m_RainBufferId, zeroVec, AL_TRUE, AL_TRUE, 1.0f, 1.0f );
	}


	m_FlightTime += deltaTime;

	CheckAchievements( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Draw()
{
	m_pState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	MainGameUI* pUIState = 0;

	if( m_pState->GetId() == UI_MAINGAME )
		pUIState = static_cast<MainGameUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	
	// normal game
	if( m_LevelStartWait )
		return;

	// default colour
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	bool fogState = m_LevelFog.bFogState;

	if( m_DevData.allowDebugCam && gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.5f, 100000.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z, 
												gDebugCamera.GetTarget().X, gDebugCamera.GetTarget().Y, gDebugCamera.GetTarget().Z );
	
		snd::SoundManager::GetInstance()->SetListenerPosition( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( (float)std::sin(math::DegToRad(-gDebugCamera.GetAngle())), 0.0f, (float)std::cos(math::DegToRad(-gDebugCamera.GetAngle())),
																	0.0f, 1.0f, 0.0f );

		// for the skybox
		GameSystems::GetInstance()->GetGameCamera()->SetPosition(gDebugCamera.GetPosition());
		GameSystems::GetInstance()->GetGameCamera()->SetTarget(gDebugCamera.GetTarget());
	}
	else
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( m_LevelData.levelNearClip, m_LevelData.levelFarClip );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		m_GameCamera->SetupCamera();

#ifdef BASE_PLATFORM_iOS
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#else
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#endif // 

		gDebugCamera.SetPosition(GameSystems::GetInstance()->GetGameCamera()->GetPosition());
		gDebugCamera.SetTarget(GameSystems::GetInstance()->GetGameCamera()->GetTarget());
	}

	m_Level.DrawSkybox();
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetFogState( &m_LevelFog );
	renderer::OpenGL::GetInstance()->EnableLighting();
	renderer::OpenGL::GetInstance()->EnableLight(0);
	
	renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

	m_Level.DrawMainLevel();

	// player
	m_Player->Draw();

	renderer::OpenGL::GetInstance()->DisableLighting();

	m_Level.DrawSpriteAndEmitters();

	renderer::OpenGL::GetInstance()->EnableLighting();

	m_Level.DrawPickups();
	m_Level.DrawGates();

	renderer::OpenGL::GetInstance()->DisableLighting();

	m_Player->DrawEmitters();

	// draw level emitters
	GameSystems::GetInstance()->DrawEffectPool();

	m_Player->DrawAlpha();

	// weather effects
	if( m_FlyingEffect != 0 )
		m_FlyingEffect->Draw();
	if( m_SnowEffect != 0 )
		m_SnowEffect->Draw();
	if( m_RainEffect != 0 )
		m_RainEffect->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, m_BoundingAlpha );
	renderer::OpenGL::GetInstance()->DisableLighting();
	
	if( m_BoundingAreaEffect != 0 &&
		m_BoundingAreaEnabled )
		m_BoundingAreaEffect->Draw();

	fogState = m_LevelFog.bFogState;
	m_LevelFog.bFogState = false;
	renderer::OpenGL::GetInstance()->SetFogState( &m_LevelFog );
	m_LevelFog.bFogState = fogState;

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

//#ifdef _DEBUG
	// physics objects (have to be updated and then drawn)
	if( m_DevData.enableDebugDraw )
	{
		if( m_DevData.enablePhysicsDraw )
		{
			bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
			bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

			if( lightState )
				renderer::OpenGL::GetInstance()->DisableLighting();
			if( textureState )
				renderer::OpenGL::GetInstance()->DisableTexturing();

			//if( !m_WasPausedLastFrame )
			//	UpdatePhysics( m_LastDelta );

			if( lightState )
				renderer::OpenGL::GetInstance()->EnableLighting();
			if( textureState )
				renderer::OpenGL::GetInstance()->EnableTexturing();
		}

	#ifdef BASE_PLATFORM_WINDOWS
		m_Player->DrawDebug();
		m_Level.DrawDebug();
	#endif // 

			renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
			renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
			renderer::OpenGL::GetInstance()->DisableLighting();

			GL_CHECK;
		
			int offset = 10;
			int xPos = 345;
			int yPos = 300;
			if( core::app::IsLandscape() &&
			   renderer::OpenGL::GetInstance()->GetIsRotated() )
			{
				xPos = core::app::GetOrientationHeight() - 150;
				yPos = core::app::GetOrientationWidth() - 60;
			}
			else
			{
				xPos = core::app::GetOrientationWidth() - 150;
				yPos = core::app::GetOrientationHeight() - 60;
			}

			GLfloat *mat = renderer::OpenGL::GetInstance()->GetModelViewMatrix();

			// get the right and up vectors
			math::Vec3 right;
			right.X = mat[0];
			right.Y = mat[4];
			right.Z = mat[8];

			math::Vec3 up;
			up.X = mat[1];
			up.Y = mat[5];
			up.Z = mat[9];

			DBGPRINT( xPos, yPos-=offset, "DELTA (%.6f)", m_LastDelta );
			//DBGPRINT( xPos, yPos-=offset, "(%.2f  %.2f  %.2f)", m_Player->GetUnprojection().X, m_Player->GetUnprojection().Y, m_Player->GetUnprojection().Z );
			DBGPRINT( xPos, yPos-=offset, "ACCEL (%.2f  %.2f  %.2f)", input::gInputState.Accelerometers[0], input::gInputState.Accelerometers[1], input::gInputState.Accelerometers[2] );
			DBGPRINT( xPos, yPos-=offset, "CAM RIGHT (%.2f  %.2f  %.2f)", right.X, right.Y, right.Z );
			DBGPRINT( xPos, yPos-=offset, "CAM UP (%.2f  %.2f  %.2f)", up.X, up.Y, up.Z );
			DBGPRINT( xPos, yPos-=offset, "CAM POS (%.2f  %.2f  %.2f)", m_GameCamera->GetPosition().X, m_GameCamera->GetPosition().Y, m_GameCamera->GetPosition().Z );
			DBGPRINT( xPos, yPos-=offset, "CAM TARGET (%.2f  %.2f  %.2f)", m_GameCamera->GetTarget().X, m_GameCamera->GetTarget().Y, m_GameCamera->GetTarget().Z );
			DBGPRINT( xPos, yPos-=offset, "PLAYER ANGLE(%.2f  %.2f  %.2f)", m_Player->GetAngles().X, m_Player->GetAngles().Y, m_Player->GetAngles().Z );
			DBGPRINT( xPos, yPos-=offset, "PLAYER DIR(%.2f  %.2f  %.2f)", m_Player->GetDirection().X, m_Player->GetDirection().Y, m_Player->GetDirection().Z );
			DBGPRINT( xPos, yPos-=offset, "SPEED / TARGET (%.2f / %.2f)", m_Player->GetSpeed(), m_Player->GetTargetSpeed() );
			DBGPRINT( xPos, yPos-=offset, "FLY LOW TIME (%.2f)", m_Player->GetFlyLowTime() );
			DBGPRINT( xPos, yPos-=offset, "GROUND DISTANCE (%.2f)", m_Player->GetDistanceToGround() );

			DBGPRINT( xPos, yPos-=offset, "TARGETS %d / %d", GameSystems::GetInstance()->GetTargetsDestroyed(), m_Level.GetTotalTargets() );
			DBGPRINT( xPos, yPos-=offset, "OBJECTS %d / %d", GameSystems::GetInstance()->GetObjectsDestroyed(), m_Level.GetTotalDestroyable() );
			DBGPRINT( xPos, yPos-=offset, "BOUNDING ALPHA %d", m_BoundingAlpha );
	}
//#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Method: ContinueLevel
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::ContinueLevel()
{
	if( m_LevelCompleteMode )
		AudioSystem::GetInstance()->UnPause();
	
	m_LevelCompleteMode = false;
	m_Level.ResetGateGroup(true);

	m_GameMode = GameSystems::GetInstance()->GetGameMode();

	DBG_ASSERT( m_pLevelData != 0 );
	m_pLevelData->targetsDestroyed = 0;
	if( m_GameMode == GameSystems::GAMEMODE_TARGET )
		m_pLevelData->targetsTotal = m_Level.GetTotalTargets();
	else
		m_pLevelData->targetsTotal = 0;

	m_pLevelData->time = 6039.0f;

	m_Player->ResetRingPitch();
}


/////////////////////////////////////////////////////
/// Method: RestartLevel
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::RestartLevel()
{

}

/////////////////////////////////////////////////////
/// Method: ReplayLevel
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::ReplayLevel()
{
	m_InputSystem.ResetAllInput();
	MainGameUI* pUIState = 0;

	if( m_LevelCompleteMode )
		m_Player->StartAudio();
	
	m_LevelCompleteMode = false;

	m_Level.ResetTokens();
	m_Level.ResetGateGroup(false);
	m_Level.ResetTargets();

	GameSystems::GetInstance()->ResetTargetsDestroyed();
	GameSystems::GetInstance()->ResetLaps();
	GameSystems::GetInstance()->ResetEffectPool();
	GameSystems::GetInstance()->ResetGatesComplete();

	m_LastCountObjectsDestroyed = GameSystems::GetInstance()->GetObjectsDestroyed();

	m_GameMode = GameSystems::GetInstance()->GetGameMode();
	int gameModeIndex = GameSystems::GetInstance()->GetGameModeIndex();

	math::Vec3 spawnPos, spawnRot;
	char tmpString[32];

	if( GetStateManager()->GetSecondaryStateManager() )
		if( m_pState->GetId() == UI_MAINGAME )
			pUIState = static_cast<MainGameUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( m_GameMode == GameSystems::GAMEMODE_RINGRUN )
	{
		m_Level.SetGateGroup( gameModeIndex );
		spawnPos = m_Level.GetPlayerStart();
		spawnRot = m_Level.GetPlayerStartRot();

		snprintf( tmpString, 32, "0 / %d", GameSystems::GetInstance()->GetGatesTotal()-1 );
		pUIState->SetGamePoints( tmpString );
	}
	else if( m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		m_Level.GetRandomSpawn( &spawnPos, &spawnRot );
					
		snprintf( tmpString, 32, "0 / %d", m_Level.GetTotalTargets() );
		pUIState->SetGamePoints( tmpString );
	}
	else if( m_GameMode == GameSystems::GAMEMODE_FREEFLIGHT )
	{
		m_Level.GetRandomSpawn( &spawnPos, &spawnRot );
		pUIState->SetGamePoints( "" );

		//GameSystems::GetInstance()->ResetObjectsDestroyed();
	}

	m_Player->SetRespawnPosition(spawnPos, spawnRot);
	m_Player->Respawn();
	m_Player->ResetRingPitch();

	m_GameCamera->SetLerpSpeed( 5.0f );
	m_GameCamera->SetLerp( false );

	math::Vec3 playerPos = m_Player->GetPosition();
	math::Vec3 playerRot = m_Player->GetAngles();
	math::Vec3 playerDir = m_Player->GetDirection();
	//math::Vec3 playerUp = m_Player->GetUpVector();

	math::Vec3 origPos = playerPos;

	origPos = math::Vec3( playerPos.X + m_GameData.CAMERA_X_OFFSET, playerPos.Y + m_GameData.CAMERA_Y_OFFSET, playerPos.Z - m_GameData.CAMERA_Z_OFFSET );

	math::Vec3 camPos = math::RotateAroundPoint( 0, origPos, playerPos, math::DegToRad(playerRot.X) );
	camPos = math::RotateAroundPoint( 1, camPos, playerPos, math::DegToRad(playerRot.Y) );
	
	m_GameCamera->SetPosition( camPos );

	math::Vec3 camTarget = playerPos + (playerDir*m_GameData.CAMERA_TARGET_MULTIPLY);

	m_GameCamera->SetTarget( camTarget );

	math::Vec3 upVec( 0.0f, 1.0f, 0.0f );
	m_GameCamera->SetUpVector( upVec );
	m_GameCamera->SetRotation( playerRot );
	m_GameCamera->SetLerp( false );

	float playerSpeed = m_Player->GetTargetSpeed();
	float range = m_Player->GetSpeedRange().maxVal - m_Player->GetSpeedRange().minVal;
	float val = playerSpeed - m_Player->GetSpeedRange().minVal;

	pUIState->SetSpeedLever( val / range );

	// clear basic level data
	ProfileManager::GetInstance()->ClearActiveLevelData();
	
	m_pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( m_pLevelData != 0 );

	m_pLevelData->targetsDestroyed = 0;
	if( m_GameMode == GameSystems::GAMEMODE_TARGET )
		m_pLevelData->targetsTotal = m_Level.GetTotalTargets();
	m_pLevelData->time = 6039.0f;

	if( m_DevData.allowDebugCam )
	{
		math::Vec3 pos( 0.0f, 0.0f, 50.0f );
		math::Vec3 target( 0.0f, 0.0f, -500.0f );
		gDebugCamera.SetPosition( pos ); 
		gDebugCamera.SetTarget( target ); 
		gDebugCamera.Disable();
	}

	// reset all sounds
	m_Level.ResetAudio();

	m_FlyingEffect->Disable();
	m_FlyingEffect->Stop();

	if( m_SnowEffect != 0 )
	{
		math::Vec3 spawnPos = m_GameCamera->GetPosition();
		spawnPos.Y += m_GameData.SNOW_HEIGHT_ABOVE_PLAYER;

		m_SnowEffect->SetPos( spawnPos+(playerDir*m_GameData.SNOW_DISTANCE_IN_FRONT) );
	}

	if( m_RainEffect != 0 )
	{
		math::Vec3 spawnPos = m_GameCamera->GetPosition();
		spawnPos.Y += m_GameData.RAIN_HEIGHT_ABOVE_PLAYER;

		m_RainEffect->SetPos( spawnPos+(playerDir*m_GameData.RAIN_DISTANCE_IN_FRONT) );
	}

	m_BoundingAreaEnabled = false;
	m_BoundingAlpha = 0;

	m_GameTime = 0.0f;
	m_TokenTimerStarted = false;
	m_TokenCollectCountdownTimer = m_GameData.TOKEN_COLLECT_TIME;

	m_InitCountdown = false;
	m_LevelStartWait = false;

	// do just one frame set up
	Draw();
	Update( 0.01f );
	Draw();

	if( m_IsRaining )
		alSourcePause(m_RainSrcId);

	m_InitCountdown = true;
}

/////////////////////////////////////////////////////
/// Method: CheckAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::CheckAchievements( float deltaTime )
{
	int i=0;

	m_AchAircraftTimer		-= deltaTime;
	m_AchLevelPlayedTimer	-= deltaTime;
	m_AchWeatherTimer		-= deltaTime;
	m_AchTimeOfDayTimer		-= deltaTime;
	m_AchFreeFlightTimer	-= deltaTime;

	if( m_AchAircraftTimer <= 0.0f )
	{
		GameSystems::GetInstance()->AwardAchievement( 49+m_PlaneIndex, -1 );
		m_AchAircraftTimer		= m_GameData.ACH_PLANE_USED_TIMER;
	}

	if( m_AchLevelPlayedTimer <= 0.0f )
	{
		GameSystems::GetInstance()->AwardAchievement( 1+m_LevelIndex, -1 );
		m_AchLevelPlayedTimer	= m_GameData.ACH_LEVEL_PLAYED_TIMER;
	}

	if( m_AchWeatherTimer <= 0.0f )
	{
		if( m_IsRaining )
			GameSystems::GetInstance()->AwardAchievement( 47, -1 );
		else if( m_IsSnowing )
			GameSystems::GetInstance()->AwardAchievement( 48, -1 );

		m_AchWeatherTimer		= m_GameData.ACH_WEATHER_RAINSNOW_TIMER;
	}

	if( m_AchTimeOfDayTimer <= 0.0f )
	{
		GameSystems::GetInstance()->AwardAchievement( 9+m_TimeOfDay, -1 );

		m_AchTimeOfDayTimer		= m_GameData.ACH_TIMEOFDAY_TIMER;
	}

	if( m_AchFreeFlightTimer <= 0.0f )
	{
		GameSystems::GetInstance()->AwardAchievement( 68, -1 );
		m_AchFreeFlightTimer	= m_GameData.ACH_FREEFLIGHT_MODE_TIMER;
	}

	// all object destroyed
	if( !GameSystems::GetInstance()->HasAchievement( 58+m_LevelIndex ) )
	{
		if( GameSystems::GetInstance()->GetObjectsDestroyed() >= m_Level.GetTotalDestroyable() )
		{
			char text[64];
			snprintf( text, 64, "%s %d / %d", res::GetScriptString(2011), GameSystems::GetInstance()->GetObjectsDestroyed(), m_Level.GetTotalDestroyable() );
			GameSystems::GetInstance()->AwardAchievement( 58+m_LevelIndex, text );	

			m_LastCountObjectsDestroyed = GameSystems::GetInstance()->GetObjectsDestroyed();

			// check every level
			if( !GameSystems::GetInstance()->HasAchievement( 66 ) )
			{
				bool destroyedAll = true;
				for( i=0; i < m_GameData.TOTAL_LEVELS; ++i )
				{
					if( !GameSystems::GetInstance()->HasAchievement( 58+i ) )
						destroyedAll = false;
				}					
				if( destroyedAll )
					GameSystems::GetInstance()->AwardAchievement( 66, -1 );
			}
		}
	}
	
	// objects
	int curCount = GameSystems::GetInstance()->GetObjectsDestroyed();
	if( m_LastCountObjectsDestroyed != curCount )
	{
		char text[64];
		snprintf( text, 64, "%s %d / %d", res::GetScriptString(2011), GameSystems::GetInstance()->GetObjectsDestroyed(), m_Level.GetTotalDestroyable() );
		GameSystems::GetInstance()->AwardAchievement( -1, text );

		m_LastCountObjectsDestroyed = curCount;
	}

	// flight time awards
	if( !GameSystems::GetInstance()->HasAchievement( 80 ) )
		if( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) >= m_GameData.ACH_STUDENT_PILOT )
			GameSystems::GetInstance()->AwardAchievement( 80, -1 );

	if( !GameSystems::GetInstance()->HasAchievement( 81 ) )
		if( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) >= m_GameData.ACH_SPORT_PILOT )
			GameSystems::GetInstance()->AwardAchievement( 81, -1 );

	if( !GameSystems::GetInstance()->HasAchievement( 82 ) )
		if( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) >= m_GameData.ACH_RECREATIONAL_PILOT )
			GameSystems::GetInstance()->AwardAchievement( 82, -1 );

	if( !GameSystems::GetInstance()->HasAchievement( 83 ) )
		if( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) >= m_GameData.ACH_PRIVATE_PILOT )
			GameSystems::GetInstance()->AwardAchievement( 83, -1 );

	if( !GameSystems::GetInstance()->HasAchievement( 84 ) )
		if( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) >= m_GameData.ACH_COMMERCIAL_PILOT )
			GameSystems::GetInstance()->AwardAchievement( 84, -1 );

	if( !GameSystems::GetInstance()->HasAchievement( 85 ) )
		if( (ProfileManager::GetInstance()->GetProfile()->flightTime + m_FlightTime) >= m_GameData.ACH_AIRLINE_TRANSPORT_PILOT )
			GameSystems::GetInstance()->AwardAchievement( 85, -1 );
}

/////////////////////////////////////////////////////
/// Function: PhysicsCallbackRayPick
/// Params: [in]data, [in]o1, [in]o2
///
/////////////////////////////////////////////////////
void PhysicsCallbackRayPick (void *data, dGeomID o1, dGeomID o2)
{
	int i=0;

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);

	// exit without doing anything if the two bodies are connected by a joint
	if (b1 && b2 && dAreConnected (b1,b2)) 
		return;

	dContact contact[MAX_CONTACTS];

	// run the collision
	int numCollisions = dCollide( o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact) );

	if( numCollisions <= 0 ) 
		return;

	physics::PhysicsIdentifier* pGeomData1 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o1));
	physics::PhysicsIdentifier* pGeomData2 = reinterpret_cast<physics::PhysicsIdentifier*>(dGeomGetData(o2));

	// check for a contact point between a ray and selectable
	DBG_ASSERT( (pGeomData1 != 0) && (pGeomData2 != 0) );

	// if no geom is the ray, don't care
	if( pGeomData1->GetBaseId() != PHYSICSBASICID_RAYTESTER &&
		pGeomData2->GetBaseId() != PHYSICSBASICID_RAYTESTER )
		return;

	RayObject* pRay = 0;

	// only want breakables and rays
	if( (pGeomData1->GetBaseId() == PHYSICSBASICID_RAYTESTER || 
		pGeomData2->GetBaseId() == PHYSICSBASICID_RAYTESTER) ) 
	{
		for( i=0; i < numCollisions; ++i )
		{
			bool newHit = false;

			if( pGeomData1->GetBaseId() == PHYSICSBASICID_RAYTESTER )
			{
				pRay = reinterpret_cast<RayObject*>(dGeomGetData(o1)); 

				if( pRay->totalHits < MAX_RAY_HITS )
				{
					pRay->rayHits[pRay->nextHitIndex].pos = math::Vec3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
					pRay->rayHits[pRay->nextHitIndex].otherGeom = o2;
					pRay->rayHits[pRay->nextHitIndex].n = math::Vec3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );
					pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o2);

					//if( FacingCamera( math::Normalise(pRay->rayDir), pRay->rayHits[pRay->nextHitIndex].n ) )
					{
						newHit = true;

						pRay->hasHits = true;
						pRay->totalHits++;

						math::Vec3 distVec = pRay->rayHits[pRay->nextHitIndex].pos - pRay->rayPos;
						pRay->rayHits[pRay->nextHitIndex].distance = distVec.length();
					}
				}
			}
			else
			{
				pRay = reinterpret_cast<RayObject*>(dGeomGetData(o2)); 

				if( pRay->totalHits < MAX_RAY_HITS )
				{
					pRay->rayHits[pRay->nextHitIndex].pos = math::Vec3( contact[i].geom.pos[0], contact[i].geom.pos[1], contact[i].geom.pos[2] );
					pRay->rayHits[pRay->nextHitIndex].otherGeom = o1;
					pRay->rayHits[pRay->nextHitIndex].n = math::Vec3( contact[i].geom.normal[0], contact[i].geom.normal[1], contact[i].geom.normal[2] );
					pRay->rayHits[pRay->nextHitIndex].pData = dGeomGetData(o1);

					//if( FacingCamera( math::Normalise(pRay->rayDir), pRay->rayHits[pRay->nextHitIndex].n ) )
					{
						newHit = true;

						pRay->hasHits = true;
						pRay->totalHits++;

						math::Vec3 distVec = pRay->rayHits[pRay->nextHitIndex].pos - pRay->rayPos;
						pRay->rayHits[pRay->nextHitIndex].distance = distVec.length();
					}
				}
			}

			if( newHit == true )
				pRay->nextHitIndex++;

			if( pRay->nextHitIndex >= MAX_RAY_HITS )
				pRay->nextHitIndex = 0;
		}
	}
}

