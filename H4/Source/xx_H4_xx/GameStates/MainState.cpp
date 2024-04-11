
/*===================================================================
	File: MainState.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"
#include "Camera/GameCamera.h"

#include "AppConsts.h"
#include "H4.h"

#include "Resources/ModelResources.h"
#include "Resources/AnimatedResources.h"
#include "GameEffects/FullscreenEffects.h"
#include "GameEffects/InsectAttack.h"

#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/MainStateUINormal.h"
#include "GameStates/UI/MainStateUITablet.h"

#include "GameStates/FrontendState.h"
#include "GameStates/EndingState.h"
#include "GameStates/MainState.h"

#include "GameStates/UI/GeneratedFiles/gameover.hgm.h"
#include "GameStates/UI/GeneratedFiles/gameover.hui.h"

namespace
{
	input::Input keyboardInput;

	bool fogToggle = true;
	bool collisionToggle = true; 

	const int LOAD_FADE = 15;
	const int DEATH_FADE = 5;
	const int MICROGAME_FADE = 15;
	const float SHAKE_ALLOW = 0.5f;

	const int TRACKS = 6;
	const float MUSIC_VOLUME = 1.0f;
	const float MUSIC_MIN_TIME = 120.0f;
	const float MUSIC_MAX_TIME = 240.0f;
	const char* music_files[] = 
	{ 
		"mp3/Prisoner84-catch-my-breath-2.mp3",
		"mp3/Prisoner84-my-teeth-are-chattering.mp3",
		"mp3/Prisoner84-ominous-hum.mp3",

		"mp3/Prisoner84-electrical-storm.mp3",
		"mp3/Prisoner84-haunted.mp3",
		"mp3/Prisoner84-ptsd.mp3",
	};
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MainState::MainState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
{
	m_LastDelta = 0.0f;
	
	m_StartSilence = true;
	m_MusicTimer = 0.0f;

	m_Player = 0;
	m_GameCamera = 0;
	m_EnemyManagement = 0;
	m_DecalSystem = 0;
	//m_ScriptedSequencePlayer = 0;

	m_MicroGame = 0;
	m_IsMicroGameActive = false;

	m_MicroGameTransitionIn = false;
	m_MicroGameTransitionInShow = false;
	m_MicroGameTransitionToQuit = false;
	m_MicroGameTransitionToMainGame = false;

	m_LevelName = 0;
	m_LoadTransitionState = LoadState_TransitionIn;
	m_LoadStarted = false;
	GameSystems::GetInstance()->SetChangeStageFlag(true);

	m_FirstLoad = true;
	m_FirstLoadTime = 2.0f;	
	m_TransitionAlpha = 255;
	
	m_EnabledAccelerometer = false;
	m_ShakeAllowTime = 0.0f;

	m_SelectedToQuit = false;
	m_SelectedToContinue = false;
	m_ShowDeadState = false;

	m_MovementTimer = 0.0f;
	m_CurrentMovement = math::Vec3( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ), 0.0f );
	m_MoveAlpha = math::RandomNumber( 64, 64 );

	m_CurrentGameTime = 0.0f;

	// need to set up the variable list
	GameSystems::GetInstance()->LoadGameData();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainState::~MainState()
{
	if( m_pUIState != 0 )
	{
		delete m_pUIState;
		m_pUIState = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::Setup()
{
	m_Player = &GameSystems::GetInstance()->GetPlayer();
	m_GameCamera = GameSystems::GetInstance()->GetGameCamera();
	m_EnemyManagement = &GameSystems::GetInstance()->GetEnemyManager();
	m_DecalSystem = &GameSystems::GetInstance()->GetDecalSystem();

	//m_ScriptedSequencePlayer = GameSystems::GetInstance()->GetScriptedSequencePlayer();

	GameSystems::GetInstance()->CallLevelOnCreate();

	m_StageData = m_pScriptData->GetActiveStageData();

	// how many enemies can be created
	int totalEnemies			= math::RandomNumber( m_StageData.minEnemies, m_StageData.maxEnemies );
	int totalFloatingHeads		= math::RandomNumber( m_StageData.minFloatingHeads, m_StageData.maxFloatingHeads );
	int totalHangingSpiders		= math::RandomNumber( m_StageData.minHangingSpiders, m_StageData.maxHangingSpiders );
	int totalAnts				= math::RandomNumber( m_StageData.minAntSpawn, m_StageData.maxAntSpawn );

	m_EnemyManagement->CreateLevelEnemies( totalEnemies );
	m_EnemyManagement->CreateLevelFloatingHeads( totalFloatingHeads );
	m_EnemyManagement->CreateLevelHangingSpiders( totalHangingSpiders );
	//m_EnemyManagement->CreateLevelCrawlingSpiders( totalEnemies );
	m_EnemyManagement->CreateLevelInsects( totalAnts );

	// fog
	fogToggle = m_FogState.bFogState = m_StageData.fogState;
	m_FogState.eFogMode = m_StageData.fogMode;
	m_FogState.fFogNearClip = m_StageData.fogNearClip;
	m_FogState.fFogFarClip = m_StageData.fogFarClip;
	m_FogState.fFogDensity = m_StageData.fogDensity;
	m_FogState.vFogColour = m_StageData.fogColour;

	// camera
	math::Vec3 playerPos = m_Player->GetPosition();
	float angle = m_Player->GetRotationAngle();

	math::Vec3 camPos = math::Vec3( playerPos.X+m_CameraData.cam_posx_offset, playerPos.Y+m_CameraData.cam_posy_offset, playerPos.Z+m_CameraData.cam_posz_offset );
	math::Vec3 camTarget = math::Vec3( playerPos.X+m_CameraData.cam_targetx_offset, playerPos.Y+m_CameraData.cam_targety_offset, playerPos.Z+m_CameraData.cam_targetz_offset);

	camPos = math::RotateAroundPoint( camPos, playerPos, math::DegToRad(angle) );
	camTarget = math::RotateAroundPoint( camTarget, playerPos, math::DegToRad(angle) );

	m_GameCamera->SetPosition( camPos );
	m_GameCamera->SetTarget( camTarget );

	// reset continue values 
	m_DeathFadeComplete = false;
	m_DeathFadeColour = math::Vec4Lite( 128, 0, 0, 0 );
	m_ShowDeadState = false;
	m_SelectedToContinue = false;
	m_SelectedToQuit = false;

	m_pUIState->SetDeathContinueFlag( false );
	m_pUIState->Update(0.0f);
	
	// reset on load/start
	m_CurrentGameTime = 0.0f;
	m_EnemyManagement->SetNumGameKills(0);

	Draw();
	m_pUIState->Draw();

	// manually do 1 single update
	GameSystems::GetInstance()->UpdateLevel(0.016f);
	m_EnemyManagement->Update(0.016f);
	m_Player->Update(0.016f);
	m_GameCamera->Update( 0.016f );
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::Enter()
{
	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();
	m_CameraData = m_pScriptData->GetCameraData();
	m_StageData = m_pScriptData->GetActiveStageData();

	renderer::OpenGL::GetInstance()->ClearColour( m_StageData.clearColour.R, m_StageData.clearColour.G, m_StageData.clearColour.B, 1.0f );
	//renderer::OpenGL::GetInstance()->SetPerspective( GAME_FOV, static_cast<float>(core::app::GetOrientationWidth())/static_cast<float>(core::app::GetOrientationHeight()), m_StageData.nearClip, m_StageData.farClip );

	GameSystems::GetInstance()->CreatePhysics();

	//GameSystems::GetInstance()->CreateScriptedSequencePlayer();

	GameSystems::GetInstance()->CreatePlayer();
	GameSystems::GetInstance()->CreateGameCamera();
	GameSystems::GetInstance()->CreateEnemyManager();
	GameSystems::GetInstance()->CreateDecalSystem();

	GameSystems::GetInstance()->LoadNewStage( );
	
	if( core::app::IsTablet() )
	{
		m_pUIState = new MainStateUITablet( *m_pStateManager, m_InputSystem );
		DBG_ASSERT( (m_pUIState != 0) );
		m_pUIState->Enter();	
	}
	else
	{
		m_pUIState = new MainStateUINormal( *m_pStateManager, m_InputSystem );
		DBG_ASSERT( (m_pUIState != 0) );
		m_pUIState->Enter();	
	}

	Setup();

	LoadNoiseTextures();

	InitialiseInsectAttack();

	// light0
	renderer::OpenGL::GetInstance()->EnableLight( 0 );
	math::Vec4 m_LightPos = math::Vec4( 0.1f, 1.0f, 0.1f, 0.0f );
	math::Vec4 m_LightDiffuse = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	math::Vec4 m_LightAmbient = math::Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	math::Vec4 m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );
	renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LightAmbient);
	renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LightSpecular );	
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 1.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.0f );

	// fog
	m_FogState.bFogState = m_StageData.fogState;
	m_FogState.eFogMode = m_StageData.fogMode;
	m_FogState.fFogNearClip = m_StageData.fogNearClip;
	m_FogState.fFogFarClip = m_StageData.fogFarClip;
	m_FogState.fFogDensity = m_StageData.fogDensity;
	m_FogState.vFogColour = m_StageData.fogColour;

	// camera
	math::Vec3 playerPos = m_Player->GetPosition();
	float angle = m_Player->GetRotationAngle();

	math::Vec3 camPos = math::Vec3( playerPos.X+m_CameraData.cam_posx_offset, playerPos.Y+m_CameraData.cam_posy_offset, playerPos.Z+m_CameraData.cam_posz_offset );
	math::Vec3 camTarget = math::Vec3( playerPos.X+m_CameraData.cam_targetx_offset, playerPos.Y+m_CameraData.cam_targety_offset, playerPos.Z+m_CameraData.cam_targetz_offset);

	camPos = math::RotateAroundPoint( camPos, playerPos, math::DegToRad(angle) );
	camTarget = math::RotateAroundPoint( camTarget, playerPos, math::DegToRad(angle) );

	m_GameCamera->SetPosition( camPos );
	m_GameCamera->SetTarget( camTarget );

	m_LevelName = GameSystems::GetInstance()->GetLevelName();
	
	m_pUIState->SetLevelName( m_LevelName );

	// get the UI set up before any fade in
	m_pUIState->Update(0);

	// death continue mesh
	m_ContinueMesh = res::LoadModel( 1008 );
	DBG_ASSERT( (m_ContinueMesh != 0) );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_ContinueElements.Load( "hui/gameover.hui", srcAssetDims, m_ContinueMesh );

	// disable rank data
	m_ContinueMesh->SetMeshDrawState( -1, false );
	m_ContinueElements.ChangeElementDrawState( false );

	m_ContinueElements.ChangeElementDrawState( GAMEOVER_LABEL_CONTINUE, true );
	m_ContinueElements.ChangeElementDrawState( GAMEOVER_LABEL_NO, true );
	m_ContinueElements.ChangeElementDrawState( GAMEOVER_LABEL_YES, true );
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::Exit()
{
	CleanupInsectAttack();

	RemoveNoiseTextures();

	m_pUIState->Exit();
	res::RemoveModel( m_ContinueMesh );

	GameSystems::GetInstance()->DestroyPhysics();

	GameSystems::GetInstance()->DestroyDecalSystem();

	GameSystems::GetInstance()->DestroyEnemyManager();

	GameSystems::GetInstance()->DestroyGameCamera();

	//GameSystems::GetInstance()->DestroyScriptedSequencePlayer();

	GameSystems::GetInstance()->DestroyPlayer();

	GameSystems::GetInstance()->DestroyLevel();

	res::ClearModelMap();
	res::ClearAnimatedModelMap();

	//if( !H4::GetHiResMode() )
	//	renderer::ClearTextureMap();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int MainState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int MainState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: UpdateLoadState
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::UpdateLoadState()
{
	switch( m_LoadTransitionState )
	{
		case LoadState_None:
		{
			if( GameSystems::GetInstance()->GetChangeStageFlag() )
			{
				m_LoadTransitionState = LoadState_TransitionOut;
			}
		}break;
		case LoadState_TransitionIn:
		{
			// fade back in
			if( !m_FirstLoad )
				m_TransitionAlpha -= LOAD_FADE;
			else
			{
				m_FirstLoadTime -= m_LastDelta;
				if( m_FirstLoadTime <= 0.0f )
				{
					m_FirstLoadTime = 0.0f;
					m_FirstLoad = false;
				}
			}
			
			m_Player->ClearMovement(true);
			m_Player->DisablePlayerControls();

			core::app::SetSmoothUpdate(false);

			if( m_TransitionAlpha < 0 )
			{
				m_TransitionAlpha = 0;
				m_LoadTransitionState = LoadState_None;
				GameSystems::GetInstance()->SetChangeStageFlag(false);

				m_Player->EnablePlayerControls();

				m_pUIState->SetLevelName( "" );
				m_pUIState->Update(0.0f);
				
				core::app::SetSmoothUpdate(true);
			}
		}break;
		case LoadState_TransitionOut:
		{
			m_TransitionAlpha += LOAD_FADE;

			m_Player->ClearMovement(true);
			m_Player->DisablePlayerControls();
			
			core::app::SetSmoothUpdate(false);

			if( !GameSystems::GetInstance()->GetEndGameState() )
			{
				if( GameSystems::GetInstance()->GetChangeStageFlag() )
				{
					m_LevelName = GameSystems::GetInstance()->GetLevelName();

					m_pUIState->SetLevelName( m_LevelName );
					m_pUIState->ClearSay();
					
					GameSystems::GetInstance()->ClearText();
					GameSystems::GetInstance()->SetTalkState( false );
					
					if( m_TransitionAlpha > 255 )
					{
						m_TransitionAlpha = 255;

						if( !m_LoadStarted )
						{
							m_LoadTimer = 0.0f;

							EndInsectAttack( );
							
							// copy
							int gameKills = m_EnemyManagement->GetNumGameKills();

							GameSystems::GameSaveData& data = GameSystems::GetInstance()->GetSaveData();
							
							data.timePlayed += m_CurrentGameTime;
							data.numKills += gameKills;
							
							GameSystems::GetInstance()->SetSaveData( data );

							if( m_DevData.enableSave )
								GameSystems::GetInstance()->SaveGameData();

							GameSystems::GetInstance()->DestroyStage();
							GameSystems::GetInstance()->LoadNewStage();

							m_LoadStarted = true;
						}
						else
						{
							m_LoadTimer += m_LastDelta;
						}

						if( m_LoadTimer >= 2.0f )
						{
							// check to see if new level was requested
							Setup();
							GameSystems::GetInstance()->SetChangeStageFlag(false);

							m_LoadTransitionState = LoadState_TransitionIn;
							m_LoadStarted = false;

							m_pUIState->Update(0.0f);
						}
					}
				}
				else
				{
					// quit to main menu
					if( m_TransitionAlpha > 255 )
					{
						m_TransitionAlpha = 255;
					}
				}
			}
			else if( GameSystems::GetInstance()->GetEndGameState() )
			{
				if( m_TransitionAlpha > 255 )
				{
					m_TransitionAlpha = 255;

					// set costume flags
					GameSystems::GetInstance()->LoadGeneralData();

					GameSystems::GeneralData& generalData = GameSystems::GetInstance()->GetGeneralData();

					if( GameSystems::GetInstance()->HasItem(7001) )
						generalData.costume2Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7002) )
						generalData.costume3Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7003) )
						generalData.costume4Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7004) )
						generalData.costume5Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7005) )
						generalData.costume6Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7006) )
						generalData.costume7Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7007) )
						generalData.costume8Unlocked = 1;
					if( GameSystems::GetInstance()->HasItem(7008) )
						generalData.costume9Unlocked = 1;

					GameSystems::GetInstance()->SetGeneralData(generalData);

					GameSystems::GetInstance()->SaveGeneralData();

					// copy
					int gameKills = m_EnemyManagement->GetNumGameKills();

					GameSystems::GameSaveData& data = GameSystems::GetInstance()->GetSaveData();
					
					data.timePlayed += m_CurrentGameTime;
					data.numKills += gameKills;
					
					GameSystems::GetInstance()->SetSaveData( data );

					if( m_DevData.enableSave )
						GameSystems::GetInstance()->SaveGameData();

					// game is over, go to the outro
					ChangeState( new EndingState( 1, *m_pStateManager, m_InputSystem) );
					return;
				}
			}

			return;
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateMicroGameAndTransitions
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
bool MainState::UpdateMicroGameAndTransitions( float deltaTime )
{
	// check the microgame state
	m_IsMicroGameActive = GameSystems::GetInstance()->IsMicroGameActive();
	if( m_IsMicroGameActive )
	{
		if( m_MicroGame == 0 &&
			!m_MicroGameTransitionToMainGame )
		{
			m_MicroGame = GameSystems::GetInstance()->GetMicroGame();
			DBG_ASSERT( (m_MicroGame != 0) );

			m_MicroGameTransitionIn = true;
			m_TransitionAlpha = 0;
		}

		if( m_MicroGameTransitionIn )
		{
			// FADE TO BLACK - main game disappear
			m_TransitionAlpha += MICROGAME_FADE;

			m_Player->ClearMovement(false);

			if( m_TransitionAlpha > 255 )
			{
				m_TransitionAlpha = 255;
			
				m_MicroGame->OnEnter();

				m_MicroGameTransitionIn = false;
				m_MicroGameTransitionInShow = true;
			}
		}
		else if( m_MicroGameTransitionInShow )
		{
			// FADE TO MICROGAME - microgame appears
			m_TransitionAlpha -= MICROGAME_FADE;

			m_Player->ClearMovement(false);

			if( m_TransitionAlpha < 0 )
			{
				m_TransitionAlpha = 0;
				m_MicroGameTransitionInShow = false;
				
				core::app::SetSmoothUpdate(true);
				return(false);
			}
		}
		else if( m_MicroGameTransitionToQuit )
		{
			// FADE TO BLACK - microgame disappear
			m_TransitionAlpha += MICROGAME_FADE;

			m_Player->ClearMovement(false);

			// load request, show the level name
			if( GameSystems::GetInstance()->GetChangeStageFlag() )
			{
				m_LevelName = GameSystems::GetInstance()->GetLevelName();

				m_pUIState->SetLevelName( m_LevelName );
			}

			if( m_TransitionAlpha > 255 )
			{
				m_TransitionAlpha = 255;
				m_MicroGame->OnExit();
				GameSystems::GetInstance()->EndMicroGame( 0 );

				// check to see if another microgame has been requested
				MicroGame *pNewGame = GameSystems::GetInstance()->GetMicroGame();

				if( (pNewGame != 0) )
				{
					m_MicroGame = pNewGame;
					m_IsMicroGameActive = true;

					m_MicroGame->OnEnter();

					m_MicroGameTransitionToQuit = false;
					m_MicroGameTransitionInShow = true;
				}
				else if( GameSystems::GetInstance()->GetChangeStageFlag() )
				{
					// load request
					m_MicroGame = 0;
					m_IsMicroGameActive = false;

					m_MicroGameTransitionToQuit = false;
					m_MicroGameTransitionToMainGame = false;

					return(false);
				}
				else
				{
					// no pending, clear
					m_MicroGame = 0;
					m_IsMicroGameActive = false;

					m_MicroGameTransitionToQuit = false;
					m_MicroGameTransitionToMainGame = true;

					m_pUIState->Update(0.0f);
				}
			}
		}
		else if( m_MicroGameTransitionToMainGame )
		{
			// handled outside this
			m_MicroGame = 0;
			m_IsMicroGameActive = false;
			GameSystems::GetInstance()->EndMicroGame( 0 );
		}
		else
		{
			// normal update
			//UpdateB2DPhysics( deltaTime );

			// is it complete
			if( m_MicroGame->IsComplete() )
			{
				m_MicroGameTransitionToQuit = true;
				//core::app::SetSmoothUpdate(false);
			}
			else
			{
				m_MicroGame->Update( deltaTime );
			}
			
			return(m_MicroGameTransitionToQuit);
		}

		return(true);
	}
	else
	{
		if( m_MicroGameTransitionToMainGame )
		{
			m_TransitionAlpha -= MICROGAME_FADE;

			m_Player->ClearMovement(false);

			if( m_TransitionAlpha < 0 )
			{
				m_TransitionAlpha = 0;
				m_MicroGameTransitionToMainGame = false;

				core::app::SetSmoothUpdate(true);
				return(false);
			}
		}
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MainState::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	if( !m_DevData.disableMusic )
	{
		if( !AudioSystem::GetInstance()->IsPlaying() )
		{
			if( m_StartSilence )
			{
				m_MusicTimer = math::RandomNumber( MUSIC_MIN_TIME, MUSIC_MAX_TIME );
				m_StartSilence = false;
			}
			else
			{
				m_MusicTimer -= m_LastDelta;

				if( m_MusicTimer <= 0.0f )
				{
					AudioSystem::GetInstance()->StopBGMTrack();
					AudioSystem::GetInstance()->ClearBGMTrack();
					AudioSystem::GetInstance()->PlayBGMTrack( music_files[ math::RandomNumber( 0, TRACKS-1) ], false );
					AudioSystem::GetInstance()->SetBGMTrackVolume( MUSIC_VOLUME );

					m_StartSilence = true;
				}
			}
		}
	}

	// don't check for normal UI when dead
	if( !m_Player->IsDead() &&
		m_TransitionAlpha == 0.0f)
		m_pUIState->Update( deltaTime );
	
	if( m_pUIState->HasQuit() )
	{
		// copy
		int gameKills = m_EnemyManagement->GetNumGameKills();

		GameSystems::GameSaveData& data = GameSystems::GetInstance()->GetSaveData();
		
		data.timePlayed += m_CurrentGameTime;
		data.numKills += gameKills;
		
		GameSystems::GetInstance()->SetSaveData( data );

		if( m_DevData.enableSave )
			GameSystems::GetInstance()->SaveGameData();

		ChangeState( new FrontendState(*m_pStateManager, m_InputSystem) );
		return;
	}

	if( m_pUIState->IsPaused() )
		return;
	
	// update the time played
	m_CurrentGameTime += deltaTime;

	//m_ScriptedSequencePlayer->Update(deltaTime);

	// if doing microgame transitions, bail early
	if( UpdateMicroGameAndTransitions(deltaTime) )
	{
		core::app::SetSmoothUpdate(false);
		return;
	}
	else if(m_IsMicroGameActive)
	{
		return;
	}
	
	// do any transitions
	UpdateLoadState();
	if( m_LoadTransitionState != LoadState_None )
		return;

	if( m_Player->IsDead() )
	{
		UpdateDeathState( deltaTime );
		return;	
	}

	// normal update
	UpdateB2DPhysics( deltaTime );

	if( keyboardInput.IsKeyPressed( input::KEY_4, true ) )
	{
		fogToggle = !fogToggle;
	}

	if( keyboardInput.IsKeyPressed( input::KEY_5, true ) )
	{
		if( gDebugCamera.IsEnabled() )
			gDebugCamera.Disable();	
		else
			gDebugCamera.Enable();
	}

	// debug inventory fill
	if( keyboardInput.IsKeyPressed( input::KEY_F1, true ) )
	{
		GameSystems::GetInstance()->AddItem( 6001 );
		GameSystems::GetInstance()->AddItem( 6004 );
	}
	if( keyboardInput.IsKeyPressed( input::KEY_BACKSPACE, true ) )
	{
		collisionToggle = !collisionToggle;
		if(collisionToggle)
			m_Player->Debug_EnablePlayerCollision();
		else
			m_Player->Debug_DisablePlayerCollision();
	}

	if( keyboardInput.IsKeyPressed( input::KEY_ENTER, true ) )
		ApplyShake();

	// update the active camera
	if( gDebugCamera.IsEnabled() )
		gDebugCamera.Update( deltaTime );

	//  don't allow any movement while player is talking
	if( !GameSystems::GetInstance()->IsTalking() ) 
	{
		GameSystems::GetInstance()->UpdateLevel(deltaTime);

		m_EnemyManagement->Update(deltaTime);

		m_Player->Update(deltaTime);
	}
	else
	{
		m_Player->ClearMovement(false);
		m_Player->UpdateModel( deltaTime );
	}

	if( !m_Player->IsDead() &&
		IsInsectAttackActive() )
	{
		if( !m_EnabledAccelerometer )
		{
			// enable
			const float ACC_FREQUENCY = 100.0f;
			core::app::SetAccelerometerState( true, ACC_FREQUENCY );

			m_EnabledAccelerometer = true;
		}
		
#if defined(BASE_PLATFORM_ANDROID)
		const float SHAKE_X = 0.9f;
		const float SHAKE_Y = 0.9f;
		const float SHAKE_Z = 0.9f;
#else
		const float SHAKE_X = 0.9f;
		const float SHAKE_Y = 1.05f;
		const float SHAKE_Z = 1.05f;
#endif 
		if( m_EnabledAccelerometer )
		{
			m_ShakeAllowTime += deltaTime;
			
			if( m_ShakeAllowTime >= SHAKE_ALLOW )
			{
				if( (std::abs( input::gInputState.Accelerometers[0] ) > SHAKE_X) ||
				   (std::abs( input::gInputState.Accelerometers[1] ) > SHAKE_Y) ||
				   (std::abs( input::gInputState.Accelerometers[2] ) > SHAKE_Z) )
				{
					ApplyShake();
					m_ShakeAllowTime = 0.0f;
				}
				else
				{
					m_ShakeAllowTime = SHAKE_ALLOW;
				}
			}
		}
		
		UpdateInsectAttack( deltaTime );
	}
	else if( m_EnabledAccelerometer )
	{
		core::app::SetAccelerometerState(false);

		m_EnabledAccelerometer = false;
		EndInsectAttack( );
	}

	if( m_GameCamera != 0 )
	{
		if( m_LoadTransitionState == LoadState_None ||
			m_LoadTransitionState == LoadState_TransitionIn )
			m_GameCamera->Update( deltaTime );
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::Draw()
{
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	// micro game takes complete control
	if( m_IsMicroGameActive )
	{
		if( !m_MicroGameTransitionIn && !m_MicroGameTransitionToMainGame)
		{
			m_MicroGame->Draw();

			if( m_MicroGameTransitionInShow || m_MicroGameTransitionToQuit )
			{
				renderer::OpenGL::GetInstance()->DisableLighting();

				renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
				renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

				DrawFullscreenQuad( 0.0f, m_TransitionAlpha );
			}
			return;
		}
	}

	if( m_StageData.fogSetClearToMatch )
		renderer::OpenGL::GetInstance()->ClearColour( m_FogState.vFogColour.R, m_FogState.vFogColour.G, m_FogState.vFogColour.B, 1.0f );
	else
		renderer::OpenGL::GetInstance()->ClearColour( m_StageData.clearColour.R, m_StageData.clearColour.G, m_StageData.clearColour.B, 1.0f );

	// fog off
	if( fogToggle )
		m_FogState.bFogState = false;

	if( gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.1f, 1000.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		math::Vec3 camPos = gDebugCamera.GetPosition();
		math::Vec3 camTarget = gDebugCamera.GetTarget();
		float camAngle = gDebugCamera.GetAngle();

		renderer::OpenGL::GetInstance()->SetLookAt( camPos.X, camPos.Y, camPos.Z, camTarget.X, camTarget.Y, camTarget.Z );
	
		float sinVal, cosVal;
		math::sinCos( &sinVal, &cosVal, math::DegToRad(camAngle) );

		snd::SoundManager::GetInstance()->SetListenerPosition( camPos.X, camPos.Y, camPos.Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( sinVal, 0.0f, -cosVal, 0.0f, 1.0f, 0.0f );

		m_FogState.bFogState = fogToggle;
	}
	else
	{
		if( m_DevData.enableFPSMode || 
			m_Player->GetFPSMode() )
		{
			renderer::OpenGL::GetInstance()->SetNearFarClip( 0.05f, m_StageData.farClip );
			m_Player->SetFPSMode( true );
		}
		else
			renderer::OpenGL::GetInstance()->SetNearFarClip( m_StageData.nearClip, m_StageData.farClip );

		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		m_GameCamera->SetupCamera();

		m_FogState.bFogState = fogToggle;

		math::Vec3 listenPos = m_GameCamera->GetPosition();
		
		math::Vec3 camTarget = m_GameCamera->GetTarget();
		math::Vec3 camDir = camTarget - listenPos;
		camDir.normalise();
		math::Vec3 north( 0.0f, 0.0f, 1.0f );

		float sinVal, cosVal;
		math::sinCos( &sinVal, &cosVal, math::DegToRad(m_Player->GetRotationAngle()) );

		snd::SoundManager::GetInstance()->SetListenerPosition( listenPos.X, listenPos.Y, listenPos.Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( sinVal, 0.0f, cosVal, 0.0f, 1.0f, 0.0f );
	}

	GameSystems::GetInstance()->DrawLevelSkybox();

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	// fog on
	renderer::OpenGL::GetInstance()->SetFogState( &m_FogState );

	renderer::OpenGL::GetInstance()->DisableLighting();

	// draw opaque level
	GameSystems::GetInstance()->DrawLevel( false );

	renderer::OpenGL::GetInstance()->EnableLighting();
	
	// draw enemies
	bool pauseFlag = GameSystems::GetInstance()->IsTalking() || m_pUIState->IsPaused();
	m_EnemyManagement->Draw(pauseFlag);
	
	// fog off
	if( fogToggle )
		m_FogState.bFogState = false;

	renderer::OpenGL::GetInstance()->SetFogState( &m_FogState );

	// draw player
	m_Player->Draw(pauseFlag);

	//m_ScriptedSequencePlayer->Draw();

	renderer::OpenGL::GetInstance()->DisableLighting();

	// fog on
	if( fogToggle )
		m_FogState.bFogState = true;

	renderer::OpenGL::GetInstance()->SetFogState( &m_FogState );

	// draw decals
	m_DecalSystem->DrawDecals();

	// draw level emitters
	GameSystems::GetInstance()->DrawLevelEmitters();

	// draw sorted meshes level
	GameSystems::GetInstance()->DrawLevel( true );

	// fog off
	if( fogToggle )
		m_FogState.bFogState = false;

	renderer::OpenGL::GetInstance()->SetFogState( &m_FogState );

//#################### DEBUGGING ####################
	if( m_DevData.enableDebugDraw )
	{
		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );

		bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
		bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

		if( lightState )
			renderer::OpenGL::GetInstance()->DisableLighting();
		if( textureState )
			renderer::OpenGL::GetInstance()->DisableTexturing();
		
		// physics objects (have to be updated and then drawn)
		if( m_DevData.enablePhysicsDraw )
		{
			glPushMatrix();
				glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
				physics::PhysicsWorldB2D::GetWorld()->DrawDebugData();
			glPopMatrix();
		}

#ifdef BASE_PLATFORM_WINDOWS
		m_Player->DrawDebug();
		m_EnemyManagement->DrawDebug();
		GameSystems::GetInstance()->DrawDebug();
#endif // BASE_PLATFORM_WINDOWS
		
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

		if( lightState )
			renderer::OpenGL::GetInstance()->EnableLighting();
		if( textureState )
			renderer::OpenGL::GetInstance()->EnableTexturing();
	}
//#################### END DEBUGGING ####################

	// noise
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	if( m_DevData.enableNoise )
		DrawNoise( 0.0f, m_LastDelta );

	if( IsInsectAttackActive() )
		DrawInsectAttack();
	
	// draw any load state
	DrawLoadState();

	if( m_MicroGameTransitionIn || m_MicroGameTransitionToMainGame )
		DrawFullscreenQuad( 0.0f, m_TransitionAlpha );

	// 2D draws
#ifdef _DEBUG
	{	
#ifdef BASE_PLATFORM_WINDOWS
		//m_Player->DrawDebug2D();
		//m_EnemyManagement->DrawDebug2D();
		//GameSystems::GetInstance()->DrawDebug2D();
#endif // 
	}
#endif // _DEBUG

	m_pUIState->Draw();

	if( m_Player->IsDead() )
	{
		// must be loading
		if( m_LoadTransitionState == LoadState_None )
			DrawDeathState();
	}

/*
#ifdef _DEBUG
	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );

	renderer::OpenGL::GetInstance()->EnableTexturing();
	DBGPRINT( 240, 30, "Time %.2f", m_CurrentGameTime );
	DBGPRINT( 240, 20, "STATE %d", m_StartSilence );
	DBGPRINT( 240, 10, "MUSIC %.2f", m_MusicTimer );
#endif // _DEBUG
*/
}

/////////////////////////////////////////////////////
/// Method: DrawLoadState
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::DrawLoadState()
{
	switch( m_LoadTransitionState )
	{
		case LoadState_None:
		break;
		case LoadState_TransitionIn:
		{
			// fade back in
			DrawFullscreenQuad( 0.0f, m_TransitionAlpha );

			// draw the name
		}break;
		case LoadState_TransitionOut:
		{
			// fade out then load
			DrawFullscreenQuad( 0.0f, m_TransitionAlpha );
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateB2DPhysics
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::UpdateB2DPhysics( float deltaTime )
{
	const int PHYSICS_VEL_ITERATIONS = 8;
	const int PHYSICS_POS_ITERATIONS = 3;

#ifdef BASE_PLATFORM_WINDOWS
		const float PHYSICS_TIMESTEP = 1.0f/60.0f;

		bool frameLock30 = m_DevData.frameLock30;
		bool frameLock60 = m_DevData.frameLock60;

		if( frameLock30 )
		{
			if( wglSwapIntervalEXT )
				wglSwapIntervalEXT( 2 );		
		}
		else if( frameLock60 )
		{
			if( wglSwapIntervalEXT )
				wglSwapIntervalEXT( 1 );
		}
#else
	const float PHYSICS_TIMESTEP = 1.0f/60.0f;
	//const float PHYSICS_TIMESTEP = deltaTime;
#endif
	
		physics::PhysicsWorldB2D::GetWorld()->Step( PHYSICS_TIMESTEP, PHYSICS_VEL_ITERATIONS, PHYSICS_POS_ITERATIONS );
}

/////////////////////////////////////////////////////
/// Method: UpdateDeathState
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::UpdateDeathState( float deltaTime )
{

	// should only be fading
	if( m_SelectedToQuit )
	{
		m_TransitionAlpha += LOAD_FADE;
			
		if( m_TransitionAlpha > 255 )
		{
			m_TransitionAlpha = 255;

			// do not want to save data here
			ChangeState( new FrontendState(*m_pStateManager, m_InputSystem) );
		}
		return;
	}
	else if( m_SelectedToContinue )
	{
		m_TransitionAlpha += LOAD_FADE;
		
		m_LevelName = GameSystems::GetInstance()->GetLevelName();

		m_pUIState->SetDeathContinueFlag( true );
		m_pUIState->SetLevelName( m_LevelName );

		if( m_TransitionAlpha > 255 )
		{
			m_TransitionAlpha = 255;
			GameSystems::GetInstance()->ContinueGame();

			core::app::SetSmoothUpdate(false);
			
			m_pUIState->Update(0);
			m_ShowDeadState = false;
		}

		return;
	}

	m_MovementTimer += deltaTime;

	if( m_MovementTimer >= 0.1f )
	{
		m_MovementTimer = 0.0f;
		m_CurrentMovement = math::Vec3( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ), 0.0f );
		m_MoveAlpha = math::RandomNumber( 64, 64 );
	}

	m_Player->DisablePlayerControls();


	m_ContinueElements.Update( TOUCH_SIZE*0.5f, 1, deltaTime );

	if( m_DeathFadeComplete )
	{
		if( m_ContinueElements.CheckElementForSingleTouch( GAMEOVER_LABEL_NO ) )
		{
			m_TransitionAlpha = 0;

			m_SelectedToQuit = true; 
			return;
		}
		else if( m_ContinueElements.CheckElementForSingleTouch( GAMEOVER_LABEL_YES ) )
		{
			m_TransitionAlpha = 0;

			m_SelectedToContinue = true;
			return;
		}
	}
	else
	{
		m_DeathFadeColour.A += DEATH_FADE;
		if( m_DeathFadeColour.A >= 255 )
		{
			 m_DeathFadeColour.A = 255;
			 m_DeathFadeComplete = true;

			 AudioSystem::GetInstance()->StopAll();
			 AudioSystem::GetInstance()->ClearOneShotList();
		}
	}

	m_ShowDeadState = true;
}

/////////////////////////////////////////////////////
/// Method: DrawDeathState
/// Params: None
///
/////////////////////////////////////////////////////
void MainState::DrawDeathState()
{
	DrawFullscreenQuad( m_DeathFadeColour );
	
	if( m_ShowDeadState )
	{
		renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		if( m_DeathFadeComplete )
		{
			m_ContinueMesh->Draw();

			// draw shifted ghost text
			m_ContinueMesh->EnableDefaultMeshColour();
			renderer::OpenGL::GetInstance()->DepthMode( true, GL_LEQUAL );
			glPushMatrix();
				math::Vec4Lite defaultColour( 255,255,255, m_MoveAlpha );
				m_ContinueMesh->SetDefaultMeshColour( defaultColour );
				
				glTranslatef( m_CurrentMovement.X, m_CurrentMovement.Y, m_CurrentMovement.Z );
				m_ContinueMesh->Draw();

				renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255, m_MoveAlpha );
				renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
				m_ContinueElements.Draw();

			glPopMatrix();
			m_ContinueMesh->DisableDefaultMeshColour();

			renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255, 255 );
			renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
			m_ContinueElements.Draw();
		}
	}

	if( m_SelectedToQuit || m_SelectedToContinue )
		DrawFullscreenQuad( 0.0f, m_TransitionAlpha );
}

