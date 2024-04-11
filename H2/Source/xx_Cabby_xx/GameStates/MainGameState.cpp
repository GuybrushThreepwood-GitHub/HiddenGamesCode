
/*===================================================================
	File: MainGameState.cpp
	Game: Cabby

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

#include "CabbyConsts.h"
#include "Cabby.h"

#include "Effects/Emitter.h"

#include "Audio/CustomerAudio.h"

#include "Resources/ModelResources.h"

#include "GameStates/IBaseGameState.h"
#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/Cabby/LightAccess.h"
#include "ScriptAccess/Cabby/FogAccess.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/LevelCompleteCareerUI.h"
#include "GameStates/UI/LevelCompleteArcadeUI.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/UI/GameOverArcadeUI.h"
#include "GameStates/UI/GameOverCareerUI.h"
#include "GameStates/UI/GameOverCareerContinueUI.h"
#include "GameStates/UI/UnlockUI.h"
#include "GameStates/MainGameState.h"

#include "Profiles/ProfileManager.h"

namespace
{
	const float RESPAWN_TIMER = 2.5f;
	const float ROTATION_PER_FRAME = 30.0f;

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
	m_LevelCompleteCareer = false;
	m_LevelCompleteArcade = false;
	m_GameOver = false;

	m_bFullZoomIn = false;
	m_bFullZoomOut = false;
	m_WasPausedLastFrame = false;

	m_DeadFirstFrame = true;
	m_PlayerDeadTimer = 0.0f;

	m_RescueFirstFrame = true;
	m_PlayerRescueTimer = 0.0f;

	m_ExtraLifeId = AudioSystem::GetInstance()->AddAudioFile( 102/*"extralife.wav"*/ );

	m_TimerTick = AudioSystem::GetInstance()->AddAudioFile( 106/*"arcade_timer_tick.wav"*/ );
	m_TimerEnd = AudioSystem::GetInstance()->AddAudioFile( 107/*"arcade_timer_end.wav"*/ );
	m_TickTime = 0.0f; 

	m_CalledRespawnFade = false;
	m_CalledRescueFade = false;

	m_pProfile = 0;
	m_pLevelData = 0;

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainGameState::~MainGameState()
{
	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}

	PhysicsWorld::SetPlayer( 0 );

	GameSystems::GetInstance()->SetCustomerManager( 0 );

	delete m_Customers;
	m_Customers = 0;

	m_Level.Release();

	// clear the vehicle
	m_pScriptData->ReleaseVehicleList();

	PhysicsWorld::Destroy();
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
	m_PlayerInWater = false;

	b2Vec2 areaMin( -1000.0f, -1000.0f );
	b2Vec2 areaMax(1000.0f, 1000.0f);
	b2Vec2 gravity(0.0f,1.0f);
	
	renderer::OpenGL::GetInstance()->SetMSAAState(true);
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	PhysicsWorld::Create( areaMin, areaMax, gravity, m_DevData.enablePhysicsDraw );

	// load from script
	script::LuaCallFunction( "PostPhysicsSetup", 0, 0 );

	m_Level.Initialise();
	m_Level.LoadMeshData( m_LevelData.levelModel );
	m_Level.LoadBackgroundModel( m_LevelData.levelBackground );
	m_Level.LoadForegroundModel( m_LevelData.levelForeground );
	m_Level.LoadComplexPhysicsData( m_LevelData.levelPhysicsComplex, PhysicsWorld::GetWorld() );
	m_Level.LoadData( m_LevelData.levelData );

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
		m_Level.SetupStarLocation();

	m_Level.SetCustomerMaxSpawn( m_LevelData.levelCustomerMaxSpawn );

	// call the vehicle setup
	int vehicleId = m_pScriptData->GetDefaultVehicle();

	// get pack and vehicle offset
	int packId = vehicleId / m_GameData.MAX_ITEMS_PER_PACK;
	int vehicleOffsetId = vehicleId % m_GameData.MAX_ITEMS_PER_PACK;

	script::LuaCallFunction( m_pScriptData->GetVehiclePackList()[packId]->pPackVehicleInfo[vehicleOffsetId].setupFunction, 0, 0 );

	if( m_pScriptData->GetVehicleList()[0]->customerCalls == 1 )
		SetCustomerAccent( CustomerAudio::CUSTOMER_COCKNEY );
	else
		SetCustomerAccent( CustomerAudio::CUSTOMER_AMERICAN );

	m_Player = new Player( m_InputSystem, *m_pScriptData->GetVehicleList()[0], m_Level );
	DBG_ASSERT( m_Player != 0 );

	m_Player->Setup();

	math::Vec3 playerStart( m_Level.GetPlayerStart().X, m_Level.GetPlayerStart().Y, 0.0f );
	if( vehicleId == 13 ) // move tardis up more
		playerStart.Y += 0.5f;
	m_Player->SetRespawnPosition(playerStart, m_Level.GetPlayerStartDirection());
	m_Player->Respawn();

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
		m_Player->SetLevelCustomerTotal( m_LevelData.levelArcadeCustomers );
	else
		m_Player->SetLevelCustomerTotal( m_LevelData.levelCustomerTotal );

	PhysicsWorld::SetPlayer( m_Player );

	m_Customers = new CustomerManagement( *m_Player, m_Level );
	DBG_ASSERT( m_Customers != 0 );

	GameSystems::GetInstance()->SetCustomerManager( m_Customers );

	m_CamZoomOut = m_LevelData.levelCameraZ;

	m_CamPos.X = playerStart.X + m_LevelData.levelCameraOffsetX;
	m_CamPos.Y = playerStart.Y + m_LevelData.levelCameraOffsetY;
	m_CamPos.Z = m_LevelData.levelCameraZ;

	m_CamLookAt.X = playerStart.X;
	m_CamLookAt.Y = playerStart.Y + m_LevelData.levelCamCenterY;
	m_CamLookAt.Z = m_LevelData.levelCamCenterZ;

	m_NumFramesX = 0;
	m_NumFramesY = 0;

	// clear basic level data
	ProfileManager::GetInstance()->ClearActiveLevelData();
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
	{
		m_GameTime = 0.0f;

		m_Player->SetLivesCount(m_pProfile->currentLives);
		m_Player->SetTotalMoney(m_pProfile->currentMoney);
		m_Player->SetFuelCount(static_cast<int>(m_pProfile->currentFuel));
	}
	else
	{
		m_GameTime = m_LevelData.levelArcadeTime;

		m_Player->SetLivesCount(m_GameData.DEFAULT_LIVES_COUNT);
		m_Player->SetTotalMoney(0);
		m_Player->SetFuelCount(100);
	}

	if( m_DevData.allowDebugCam )
	{
		math::Vec3 pos( 0.0f, 0.0f, 50.0f );
		math::Vec3 target( 0.0f, 0.0f, -500.0f );
		gDebugCamera.SetPosition( pos );
		gDebugCamera.SetTarget( target ); 
		gDebugCamera.Disable();
	}

	// detail map
	//m_Level.DetailMapSetup(2000, 12.0f, 12.0f);
	//m_Level.DetailMapSetup(2001, 10.0f, 10.0f);
	//m_Level.DetailMapSetup(2002, 5.0f, 5.0f);
	if ( m_GameData.ALLOW_DETAIL_MAPPING )
		m_Level.DetailMapSetup(m_LevelData.detailMap, m_LevelData.detailMapScaleX, m_LevelData.detailMapScaleY);

	switch( math::RandomNumber(Level::LEVEL_DAY, Level::LEVEL_NIGHT) )
	{
		// day
		case Level::LEVEL_DAY:
		{
			m_Level.SetTimeOfDay( Level::LEVEL_DAY );
			m_LevelFog = GetFogData( m_LevelData.levelFogDay );
			m_LevelLight = GetLightData( m_LevelData.levelDayLight );
			m_Level.SetupBGQuad( m_LevelData.levelBGTextureDay, m_LevelData.levelBGDayUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 

		}break;
		// afternoon
		case Level::LEVEL_AFTERNOON:
		{
			m_Level.SetTimeOfDay( Level::LEVEL_AFTERNOON );
			m_LevelFog = GetFogData( m_LevelData.levelFogAfternoon );
			m_LevelLight = GetLightData( m_LevelData.levelAfternoonLight );
			m_Level.SetupBGQuad( m_LevelData.levelBGTextureAfternoon, m_LevelData.levelBGAfternoonUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 
		}break;
		// night
		case Level::LEVEL_NIGHT:
		{
			m_Level.SetTimeOfDay( Level::LEVEL_NIGHT );
			m_LevelFog = GetFogData( m_LevelData.levelFogNight );
			m_LevelLight = GetLightData( m_LevelData.levelNightLight );
			m_Level.SetupBGQuad( m_LevelData.levelBGTextureNight, m_LevelData.levelBGNightUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 
		}break;
		default:
		{
			m_LevelFog.bFogState = false;
			m_LevelLight = GetLightData( "defaultLightSettings" );	
		}break;
		
	}

	m_LevelLightBackup = m_LevelLight;
	m_LevelFogBackup = m_LevelFog;

	m_WaterLight = GetLightData( m_LevelData.levelNightLight );
	m_WaterFog = GetFogData( m_LevelData.levelFogNight );

	m_PlayerNightLight = GetLightData( "nightPlayerLightSettings" );
	m_Player->SetNightMode(m_LevelLight.showVehicleLight);

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

	// everything is loaded, inform the Start Level UI
	m_pState = 0;
	m_pState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( m_pState->GetId() == UI_LEVELSTART )
	{
		LevelStartUI* pUIState = static_cast<LevelStartUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
		pUIState->SetFinishedLoading();

		m_LevelStartWait = true;
	}

	m_pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( m_pLevelData != 0 );
	m_pLevelData->levelArcadeCustomers = m_LevelData.levelArcadeCustomers;
	m_pLevelData->levelArcadeFuel = m_LevelData.levelArcadeFuel;
	m_pLevelData->levelArcadeMoney = m_LevelData.levelArcadeMoney;

	// turn accelerometer on with 30 updates per sec
	if( m_pProfile->controlType == 1 )
		core::app::SetAccelerometerState(true, ACCELEROMETER_FREQ);
	else
		core::app::SetAccelerometerState(false);

	// unlock
	m_pVehicleModel = 0;
	m_Rotation = 0.0f;
	m_UnlockVehicleId = -1;

	m_CameraWaterState = -1;
	m_CameraCollision.vCenterPoint = m_CamPos;
	m_CameraCollision.fRadius = 0.5f;
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Exit()
{
	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}
	m_Rotation = 0.0f;
	m_UnlockVehicleId = -1;

	// turn off lights for possible complete state
	renderer::OpenGL::GetInstance()->DisableLight( 0 );
	renderer::OpenGL::GetInstance()->DisableLight( 1 );

	core::app::SetAccelerometerState(false);
	
	renderer::OpenGL::GetInstance()->SetMSAAState(false);
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
	m_pState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	MainGameUI* pUIState = 0;

	if( m_pState->GetId() == UI_MAINGAME )
		pUIState = static_cast<MainGameUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	else 
	if( m_pState->GetId() == UI_UNLOCK )
	{
		UnlockUI* pUnlockState = static_cast<UnlockUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		// special branch to draw unlocks at the end of arcade without unloading the level
		if( ProfileManager::GetInstance()->GetVehicleUnlockState() &&
			 m_UnlockVehicleId != ProfileManager::GetInstance()->GetVehicleUnlockId() &&
			 pUnlockState->FinishedFade() )
		{
			if( m_pVehicleModel != 0 )
			{
				res::RemoveModel( m_pVehicleModel );
				m_pVehicleModel = 0;
			}

			renderer::OpenGL::GetInstance()->DisableLight( 1 );

			m_UnlockVehicleId = ProfileManager::GetInstance()->GetVehicleUnlockId();
			DBG_ASSERT( m_UnlockVehicleId != -1 );

			int packId = m_UnlockVehicleId / m_GameData.MAX_ITEMS_PER_PACK;
			int vehicleOffsetId = m_UnlockVehicleId % m_GameData.MAX_ITEMS_PER_PACK;

			const char* funcCall = GetScriptDataHolder()->GetVehiclePackList()[packId]->pPackVehicleInfo[vehicleOffsetId].setupFunction;

			// call the vehicle setup
			script::LuaCallFunction( funcCall, 0, 0 );

			// bit of a mess up, index 0 will be the current playing vehicle, 1 will be the unlock
			std::size_t listSize = m_pScriptData->GetVehicleList().size();
			ScriptDataHolder::VehicleScriptData* pVehicleData = m_pScriptData->GetVehicleList()[listSize-1];
			DBG_ASSERT( pVehicleData != 0 );

			m_pVehicleModel = res::LoadModel( pVehicleData->modelIndex );

			// by default disable everything
			m_pVehicleModel->SetMeshDrawState( -1, false );

			// main body
			m_pVehicleModel->SetMeshDrawState( pVehicleData->mainBodyMesh, true );

			// permanent meshes
			unsigned int i=0;
			if( pVehicleData->hasPermanentMeshes )
			{
				for( i=0; i < static_cast<unsigned int>(pVehicleData->permanentSubMeshCount); ++i )
					m_pVehicleModel->SetMeshDrawState( pVehicleData->permanentSubMeshList[i], true );
			}

			// remove the vehicle, it's not to be used
			if( pVehicleData->pLandingGearDef != 0 )
			{
				delete pVehicleData->pLandingGearDef;
				pVehicleData->pLandingGearDef = 0;
			}
			PhysicsWorld::GetWorld()->DestroyBody( pVehicleData->pBody );
			delete pVehicleData;

			m_pScriptData->GetVehicleList().pop_back();
		}

		// normal update
		m_Rotation += ROTATION_PER_FRAME*deltaTime;
		return;
	}

	if( m_GameOver ||
		m_LevelStartWait || 
		m_LevelCompleteCareer ||
		m_LevelCompleteArcade )
		return;

	m_LastDelta = deltaTime;

		if( m_pState->GetId() == UI_MAINGAME &&
			!pUIState->IsPaused() &&
			!pUIState->IsTimeUp() &&
			!m_Player->IsDead() )
		{
			m_bFullZoomIn = false;
			m_bFullZoomOut = false;

			// zoom in when on a landing zone
			if( m_Player->IsOnGroundIdle() &&
				(m_Player->IsInContactWithWorld() && m_Player->IsInLandingZone()) &&
				PhysicsWorld::ContactNormal.y >= 0.9f )
			{
				/*if( m_CamZoomOut > m_LevelData.levelCloseZoom )
					m_CamZoomOut -= m_LevelData.levelCloseZoomSpeed*m_LastDelta;
				else
				{
					m_bFullZoomIn = true;
					m_CamZoomOut = m_LevelData.levelCloseZoom;
				}*/
				m_bFullZoomIn = true;
			}
			else
			{
				if( m_CamZoomOut < m_LevelData.levelCameraZ )
					m_CamZoomOut += m_LevelData.levelCloseZoomSpeed*m_LastDelta;
				else
				{
					m_bFullZoomOut = true;
					m_CamZoomOut = m_LevelData.levelCameraZ;
				}
			}

			math::Vec3 newPos( 0.0f, 0.0f, 0.0f );
			math::Vec3 newLookAt( 0.0f, 0.0f, 0.0f );

			newPos.X = m_Player->GetBody()->GetPosition().x + m_LevelData.levelCameraOffsetX;
			newPos.Y = m_Player->GetBody()->GetPosition().y + m_LevelData.levelCameraOffsetY;
			newPos.Z = m_CamZoomOut;

			const float CAM_CATCHUP_POS = 4.0f;
			m_CamPos = math::Lerp( m_CamPos,newPos, m_LastDelta*CAM_CATCHUP_POS );

			newLookAt.Z = m_LevelData.levelCamCenterZ;
			
			const float FAST_X_OFFSET = 75.0f;

			if( m_Player->GetBody()->GetLinearVelocity().x > 1.0f )
			{
				m_NumFramesX++;

				if( m_NumFramesX >= 10 )
				{
					newLookAt.X = m_Player->GetBody()->GetPosition().x + FAST_X_OFFSET;
					m_NumFramesX = 10;
				}
			}
			else 
			if( m_Player->GetBody()->GetLinearVelocity().x < -1.0f  )
			{
				m_NumFramesX++;

				if( m_NumFramesX >= 10 )
				{
					newLookAt.X = m_Player->GetBody()->GetPosition().x - FAST_X_OFFSET;
					m_NumFramesX = 10;
				}
			}
			else
			{
				if( m_NumFramesX <= 0 )
				{
					newLookAt.X = m_Player->GetBody()->GetPosition().x;
					m_NumFramesX = 0;
				}
				else
					m_NumFramesX--;
			}

			const float FAST_Y_OFFSET_CLIMB = 125.0f;
			const float FAST_Y_OFFSET_FALL = 130.0f;

			if( m_Player->GetBody()->GetLinearVelocity().y > 3.5f )
			{
				m_NumFramesY++;

				if( m_NumFramesY >= 10 )
				{
					newLookAt.Y = m_Player->GetBody()->GetPosition().y + FAST_Y_OFFSET_CLIMB;
					m_NumFramesY = 10;
				}
			}
			else 
			if( m_Player->GetBody()->GetLinearVelocity().y < -2.0f  )
			{
				m_NumFramesY++;

				if( m_NumFramesY >= 10 )
				{
					newLookAt.Y = m_Player->GetBody()->GetPosition().y - FAST_Y_OFFSET_FALL;
					m_NumFramesY = 10;
				}
			}
			else
			{
				if( m_NumFramesY <= 0 )
				{
					newLookAt.Y = m_Player->GetBody()->GetPosition().y + m_LevelData.levelCamCenterY;
					m_NumFramesY = 0;
				}
				else
					m_NumFramesY--;
			}

			const float CAM_CATCHUP_LOOKAT = 1.0f;
			m_CamLookAt = math::Lerp( m_CamLookAt, newLookAt, m_LastDelta*CAM_CATCHUP_LOOKAT ) ;
		}
		m_CameraCollision.vCenterPoint = m_CamPos;


#ifdef _DEBUG
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

		m_Player->SetNightMode(m_LevelLight.showVehicleLight);

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
		
		m_Level.SetupBGQuad( m_LevelData.levelBGTextureDay, m_LevelData.levelBGDayUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 
		m_Level.SetTimeOfDay( Level::LEVEL_DAY );

		m_Player->SetNightMode(m_LevelLight.showVehicleLight);

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
		
		m_Level.SetupBGQuad( m_LevelData.levelBGTextureAfternoon, m_LevelData.levelBGAfternoonUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 
		m_Level.SetTimeOfDay( Level::LEVEL_AFTERNOON );

		m_Player->SetNightMode(m_LevelLight.showVehicleLight);

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
		
		m_Level.SetupBGQuad( m_LevelData.levelBGTextureNight, m_LevelData.levelBGNightUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 
		m_Level.SetTimeOfDay( Level::LEVEL_NIGHT );

		m_Player->SetNightMode(m_LevelLight.showVehicleLight);

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
#endif // _DEBUG

    bool pausedLast = false;
	
    if( GetStateManager()->GetSecondaryStateManager() )
	{
		if( m_pState->GetId() == UI_MAINGAME )
		{
			if( pUIState->IsPaused() )
			{
				if( !m_WasPausedLastFrame )
				{	
					AudioSystem::GetInstance()->Pause();

					m_Player->StopAllAudio();
					pausedLast = m_WasPausedLastFrame = true;
                    
                    m_Player->SetWasPaused(true);
				}
				return;
			}
			else
			{
				if( m_WasPausedLastFrame )
				{
					AudioSystem::GetInstance()->UnPause();

					m_WasPausedLastFrame = false;
				}
			}

			// update game time
			if( !pUIState->IsTimeUp() && 
				!m_Player->IsDead() &&
				!m_Player->HasSurvivedFuelRunout() )
			{
				if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
				{
					m_GameTime -= deltaTime;
					if( m_GameTime-0.99f <= 4.0f &&
						m_GameTime > 1.5f )
					{
						m_TickTime += deltaTime;
						
						if( m_TickTime >= 1.0f )
						{
							AudioSystem::GetInstance()->PlayAudio( m_TimerTick, zeroVec, true );
							m_TickTime = 0.0f;
						}
					}

					if( m_GameTime <= 1.1f )
					{
						m_Player->StopAllAudio();

						m_GameTime = 0.0f;
						m_TickTime = 0.0f;

						AudioSystem::GetInstance()->PlayAudio( m_TimerEnd, zeroVec, true );

						pUIState->SetGameTime( m_GameTime );

						int moneyToAdd = m_Player->GetMoneyToAdd();
						m_pLevelData->moneyEarned += moneyToAdd;

						m_pLevelData->starCollected = m_Level.WasStarPickedUp();
						m_pLevelData->customerCountStar = (m_pLevelData->customersTransported >= m_LevelData.levelArcadeCustomers) ? true : false;
						m_pLevelData->moneyStar = (m_pLevelData->moneyEarned >= m_LevelData.levelArcadeMoney) ? true : false;

						// arcade mode is over
						m_LevelCompleteArcade = true;
						pUIState->DoTimeUp();

						return;
					}
				}
				else
					m_GameTime += deltaTime;

				pUIState->SetGameTime( m_GameTime );
			}
		}
	}

	if( m_PlayerInWater != m_Player->InWater() )
	{
		m_PlayerInWater = m_Player->InWater();
	}	

	CheckCameraAgainstWater();

	if( m_Player->IsDead() )
	{
		if( m_DeadFirstFrame )
		{
			m_PlayerDeadTimer = RESPAWN_TIMER;

			if( m_Player->HasPassenger() )
			{
				m_pLevelData->customersLost += 1;
			}
			else if( m_Player->HasCargo() )
			{
				m_pLevelData->parcelsLost += 1;
			}

			int lives = m_Player->GetLivesCount();
			lives -= 1;
			m_pLevelData->taxisLost += 1;

			if( lives <= 0 )
				lives = 0;

			// reset extra life total
			if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
				m_pProfile->extraLifeTotal = 0;

			m_Player->SetLivesCount(lives);

			m_Player->SpawnCrashCharacters();
			
			m_DeadFirstFrame = false;
		}

		m_PlayerDeadTimer -= deltaTime;
		if( m_PlayerDeadTimer <= 0.0f &&
			m_Player->GetLivesCount() > 0 )
		{
			if( m_pState->GetId() == UI_MAINGAME &&
				!pUIState->IsPaused() &&
				!pUIState->IsTimeUp() )
			{
				if( !m_CalledRespawnFade )
				{
					pUIState->DoRespawnFadeToBlack();
					m_CalledRespawnFade = true;
				}
			}
		}
		
		// do the repsawn
		if( m_PlayerDeadTimer <= 0.0f &&
			m_Player->GetLivesCount() > 0 )
		{
			if( m_Player->IsDead() )
			{
				if( m_pState->GetId() == UI_MAINGAME &&
					!pUIState->IsPaused() &&
					!pUIState->IsTimeUp() &&
					pUIState->IsRespawnFadeToBlackComplete( ))
				{
					m_PlayerDeadTimer = 0.0f;

					m_Player->Respawn();
					m_Level.ClearPhysicsObjects();
					m_Level.ResetPhysicsObjects();

					m_PlayerInWater = m_Player->InWater();
					if( m_Player->InWater() )
					{
						m_LevelLight = m_WaterLight;
						m_LevelFog = m_WaterFog;
						renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
						renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
						renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
						renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

						m_Player->SetNightMode(m_LevelLight.showVehicleLight);
					}
					else
					{
						m_LevelLight = m_LevelLightBackup;
						m_LevelFog = m_LevelFogBackup;

						renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
						renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
						renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
						renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

						m_Player->SetNightMode(m_LevelLight.showVehicleLight);
					}

					m_bFullZoomIn = true;
					m_bFullZoomOut = false;
					m_CamZoomOut = m_LevelData.levelCameraZ;//m_LevelData.levelCloseZoom;

					m_CamPos.X = m_Player->GetBody()->GetPosition().x + m_LevelData.levelCameraOffsetX;
					m_CamPos.Y = m_Player->GetBody()->GetPosition().y + m_LevelData.levelCameraOffsetY;
					m_CamPos.Z = m_LevelData.levelCameraZ;//m_CamZoomOut;

					m_CamLookAt.X = m_Player->GetBody()->GetPosition().x;
					m_CamLookAt.Y = m_Player->GetBody()->GetPosition().y;
					m_CamLookAt.Z = m_LevelData.levelCamCenterZ;

					m_NumFramesX = 0;
					m_NumFramesY = 0;

					m_CalledRespawnFade = false;
				}
			}
		}
	}
	else
		m_DeadFirstFrame = true;

	if( m_Player->HasSurvivedFuelRunout() )
	{
		if( m_RescueFirstFrame )
		{
			m_PlayerRescueTimer = RESPAWN_TIMER;

			m_RescueFirstFrame = false;
		}

		m_PlayerRescueTimer -= deltaTime;
		if( m_PlayerRescueTimer <= 0.0f )
		{
			if( m_pState->GetId() == UI_MAINGAME &&
				!pUIState->IsPaused() &&
				!pUIState->IsTimeUp() )
			{
				if( !m_CalledRescueFade )
				{
					pUIState->DoRescueFadeToBlack();
					m_CalledRescueFade = true;
				}
			}
		}

		// do the repsawn
		if( m_PlayerRescueTimer <= 0.0f )
		{
			if( m_pState->GetId() == UI_MAINGAME &&
				!pUIState->IsPaused() &&
				!pUIState->IsTimeUp() &&
				pUIState->HasFlyByFinished( ))
			{
				m_PlayerRescueTimer = 0.0f;

				m_Player->Respawn();
				m_Level.ClearPhysicsObjects();
				m_Level.ResetPhysicsObjects();

				m_PlayerInWater = m_Player->InWater();
				if( m_Player->InWater() )
				{
					m_LevelLight = m_WaterLight;
					m_LevelFog = m_WaterFog;
					renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
					renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
					renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
					renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

					m_Player->SetNightMode(m_LevelLight.showVehicleLight);
				}
				else
				{
					m_LevelLight = m_LevelLightBackup;
					m_LevelFog = m_LevelFogBackup;

					renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
					renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
					renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
					renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
					renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

					m_Player->SetNightMode(m_LevelLight.showVehicleLight);
				}

				m_bFullZoomIn = true;
				m_bFullZoomOut = false;
				m_CamZoomOut = m_LevelData.levelCameraZ;//m_LevelData.levelCloseZoom;

				m_CamPos.X = m_Player->GetBody()->GetPosition().x + m_LevelData.levelCameraOffsetX;
				m_CamPos.Y = m_Player->GetBody()->GetPosition().y + m_LevelData.levelCameraOffsetY;
				m_CamPos.Z = m_LevelData.levelCameraZ;//m_CamZoomOut;

				m_CamLookAt.X = m_Player->GetBody()->GetPosition().x;
				m_CamLookAt.Y = m_Player->GetBody()->GetPosition().y;
				m_CamLookAt.Z = m_LevelData.levelCamCenterZ;

				m_NumFramesX = 0;
				m_NumFramesY = 0;

				m_pProfile->towTruckShouldUnlock = true;

				m_CalledRescueFade = false;
			}
		}
		
	}
	else
		m_RescueFirstFrame = true;

	// once dead, stop everything
	if( m_Player->GetLivesCount() == 0 &&
		m_PlayerDeadTimer <= 0.0f )
	{
		if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
		{
			int moneyToAdd = m_Player->GetMoneyToAdd();

			m_pLevelData->moneyEarned += moneyToAdd;

			m_pProfile->currentLives = m_Player->GetLivesCount();
			m_pProfile->currentFuel = static_cast<float>( m_Player->GetFuelCount() );
			m_pProfile->currentMoney = m_Player->GetTotalMoney()+moneyToAdd;

			m_pProfile->totalMoneyEarned			+= m_pLevelData->moneyEarned;

			m_pProfile->fuelPurchased				+= m_pLevelData->fuelPurchased;
			m_pProfile->perfectFares				+= m_pLevelData->perfectFares;
			m_pProfile->totalCustomersTransported	+= m_pLevelData->customersTransported;
			m_pProfile->totalCustomersLost			+= m_pLevelData->customersLost;
			m_pProfile->totalParcelsTransported		+= m_pLevelData->parcelsTransported;
			m_pProfile->totalParcelsLost			+= m_pLevelData->parcelsLost;
			m_pProfile->taxisLost					+= m_pLevelData->taxisLost;

			// clamp all values
			math::Clamp( &m_pProfile->currentLives, 0, 99 );
			math::Clamp( &m_pProfile->currentFuel, 0.0f, 100.0f );
			math::Clamp( &m_pProfile->currentMoney, -999999, 999999 );

			math::Clamp( &m_pProfile->totalMoneyEarned, -999999, 999999 );
			math::Clamp( &m_pProfile->fuelPurchased, -999999, 999999 );
			math::Clamp( &m_pProfile->perfectFares, 0, 99 );
			math::Clamp( &m_pProfile->totalCustomersTransported, 0, 9999 );
			math::Clamp( &m_pProfile->totalCustomersLost, 0, 9999 );
			math::Clamp( &m_pProfile->totalParcelsTransported, 0, 9999 );
			math::Clamp( &m_pProfile->totalParcelsLost, 0, 9999 );
			math::Clamp( &m_pProfile->taxisLost, 0, 9999 );

		}
		else
		{
			int moneyToAdd = m_Player->GetMoneyToAdd();
			m_pLevelData->moneyEarned += moneyToAdd;
		}

		m_Player->StopAllAudio();

		ProfileManager::GetInstance()->ClearActiveLevelData();

		if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
		{
			m_pStateManager->ChangeSecondaryState( new GameOverArcadeUI(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );	
		}
		else
		{
			// is there enough money to buy new taxis
			if( m_pProfile->currentMoney >= m_GameData.CONTINUE_COST )
				m_pStateManager->ChangeSecondaryState( new GameOverCareerContinueUI(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );
			else
				m_pStateManager->ChangeSecondaryState( new GameOverCareerUI(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
		}
		
		m_GameOver = true;

		// state has changed
		return;
	}

	// physics update
	if( !m_DevData.enablePhysicsDraw &&
		!m_WasPausedLastFrame )
	{
		UpdatePhysics( deltaTime );
	}

	m_Customers->Update( deltaTime );

	m_Level.Update( deltaTime );

	m_Player->CheckAgainstLevel( m_Level );
	m_Player->Update( deltaTime );
    
    if( pausedLast && !m_WasPausedLastFrame )
        m_Player->SetWasPaused(false);
    else
    {
        if( !pausedLast )
            m_Player->SetWasPaused(false);
    }
    
	// set the correct UI elements
	if( GetStateManager()->GetSecondaryStateManager() )
	{
		int currAmount = m_Player->GetTotalMoney();
		int moneyToAdd = m_Player->GetMoneyToAdd();
		if( moneyToAdd > 0 )
		{
			currAmount+=moneyToAdd;
			if( currAmount > 999999 )
				currAmount = 999999;

			m_Player->AddMoney( -moneyToAdd );
			m_Player->SetTotalMoney( currAmount );

			m_pLevelData->moneyEarned += moneyToAdd;
			m_Player->ClearMoneyToAdd();

			if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
			{
				m_pProfile->extraLifeTotal += moneyToAdd;

				// hit extra life counter
				if( m_pProfile->extraLifeTotal > m_GameData.EXTRALIFE_REWARD )
				{
					m_pProfile->extraLifeTotal = 0;
					m_pProfile->currentLives += 1;
					if( m_pProfile->currentLives > 99 )
						m_pProfile->currentLives = 99;

					int newLife = m_Player->GetLivesCount() + 1;
					if( newLife > 99 )
						newLife = 99;

					m_Player->SetLivesCount( newLife );

					m_Player->ShowExtraLife();

					// play jingle
					ALuint sourceId = snd::INVALID_SOUNDSOURCE;
					sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

					if( sourceId != snd::INVALID_SOUNDSOURCE &&
						m_ExtraLifeId != snd::INVALID_SOUNDBUFFER )
					{
						AudioSystem::GetInstance()->PlayAudio( sourceId, m_ExtraLifeId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
					}
				}
			}
		}
		else if( moneyToAdd < 0 )
		{
			currAmount-=std::abs(moneyToAdd);

			m_Player->AddMoney( std::abs(moneyToAdd) );
			m_Player->SetTotalMoney( currAmount );
			m_Player->ClearMoneyToAdd();

			m_pLevelData->moneyEarned -= std::abs(moneyToAdd);
		}
	}

	// change state, if player is in contact with world, idle, and on the HQ
	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE &&
		m_Player->GetCustomersComplete() >= m_Player->GetLevelCustomerTotal() &&
		(m_Player->IsInContactWithWorld() && m_Player->IsInLandingZone()) &&
		m_Player->IsOnGroundIdle() &&
		m_bFullZoomIn &&
		m_Player->GetLandingZoneType() == Level::ePortType_HQ &&
		m_Player->GetFuelCount() > 0 )
	{

		int moneyToAdd = m_Player->GetMoneyToAdd();

		m_pLevelData->moneyEarned += moneyToAdd;
			
		m_pProfile->currentLives = m_Player->GetLivesCount();
		m_pProfile->currentFuel = static_cast<float>( m_Player->GetFuelCount() );
		m_pProfile->currentMoney = m_Player->GetTotalMoney()+moneyToAdd;

		m_pProfile->currentPerfectFares	+= m_pLevelData->perfectFares;
		m_pProfile->currentCustomersLost += m_pLevelData->customersLost;
		m_pProfile->currentLivesLost	+= m_pLevelData->taxisLost;

		m_pProfile->totalMoneyEarned			+= m_pLevelData->moneyEarned;

		m_pProfile->fuelPurchased				+= m_pLevelData->fuelPurchased;
		m_pProfile->perfectFares				+= m_pLevelData->perfectFares;
		m_pProfile->totalCustomersTransported	+= m_pLevelData->customersTransported;
		m_pProfile->totalCustomersLost			+= m_pLevelData->customersLost;
		m_pProfile->totalParcelsTransported		+= m_pLevelData->parcelsTransported;
		m_pProfile->totalParcelsLost			+= m_pLevelData->parcelsLost;
		m_pProfile->taxisLost					+= m_pLevelData->taxisLost;
			
		if( moneyToAdd > 0 )
			m_pProfile->extraLifeTotal += moneyToAdd;
			
		// hit extra life counter
		if( m_pProfile->extraLifeTotal > m_GameData.EXTRALIFE_REWARD )
		{
			m_pProfile->extraLifeTotal = 0;
			m_pProfile->currentLives += 1;
		}

		// clamp all values
		math::Clamp( &m_pProfile->currentLives, 0, 99 );
		math::Clamp( &m_pProfile->currentFuel, 0.0f, 100.0f );
		math::Clamp( &m_pProfile->currentMoney, -999999, 999999 );

		math::Clamp( &m_pProfile->totalMoneyEarned, -999999, 999999 );
		math::Clamp( &m_pProfile->fuelPurchased, -999999, 999999 );
		math::Clamp( &m_pProfile->perfectFares, 0, 99 );
		math::Clamp( &m_pProfile->totalCustomersTransported, 0, 9999 );
		math::Clamp( &m_pProfile->totalCustomersLost, 0, 9999 );
		math::Clamp( &m_pProfile->totalParcelsTransported, 0, 9999 );
		math::Clamp( &m_pProfile->totalParcelsLost, 0, 9999 );
		math::Clamp( &m_pProfile->taxisLost, 0, 9999 );

		m_Player->StopAllAudio();

		// if mid career, do the unlocks
		bool doUnlocks = false;

		// tow truck
		if( m_pProfile->towTruckShouldUnlock )
		{
			if( !m_pProfile->towTruckUnlocked )
			{
				ProfileManager::GetInstance()->AddVehicleToUnlocks( 2 );
				m_pProfile->towTruckUnlocked = true;

				// two truck award
				GameSystems::GetInstance()->AwardAchievement(1);

				doUnlocks = true;
			}
		}

		// unlock delivery van
		if( m_pProfile->totalParcelsTransported >= m_GameData.DELIVERY_VAN_UNLOCK_TOTALPARCELS )
		{
			if( !m_pProfile->deliveryVanUnlocked )
			{
				ProfileManager::GetInstance()->AddVehicleToUnlocks( 3 );
				m_pProfile->deliveryVanUnlocked = true;

				// delivery van award
				GameSystems::GetInstance()->AwardAchievement(2);

				doUnlocks = true;
			}
		}
						
		// unlock hot rod
		if( m_pProfile->fuelPurchased >= m_GameData.HOTROD_UNLOCK_TOTALFUEL )
		{	
			if( !m_pProfile->hotRodUnlocked )
			{
				ProfileManager::GetInstance()->AddVehicleToUnlocks( 4 );
				m_pProfile->hotRodUnlocked = true;

				// hot rod award
				GameSystems::GetInstance()->AwardAchievement(3);

				doUnlocks = true;
			}
		}

		if( doUnlocks )
		{
			// go to unlocks
			m_pStateManager->ChangeSecondaryState( new UnlockUI(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem, ProfileManager::CAREER_MODE) );	
		}
		else
		{
			// normal level complete
			m_pStateManager->ChangeSecondaryState( new LevelCompleteCareerUI(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );
		}

		m_LevelCompleteCareer = true;
		return;
	}
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
	else
	if( m_pState->GetId() == UI_UNLOCK )
	{
		// special branch to draw unlocks at the end of arcade without unloading the level
		ScriptDataHolder::DevScriptData devData = m_pScriptData->GetDevData();

		// default colour
		renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

		if( devData.allowDebugCam && gDebugCamera.IsEnabled() )
		{
			renderer::OpenGL::GetInstance()->SetNearFarClip( 0.5f, 10000.0f );
			renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

			renderer::OpenGL::GetInstance()->SetLookAt( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z, 
													gDebugCamera.GetTarget().X, gDebugCamera.GetTarget().Y, gDebugCamera.GetTarget().Z );
	
			snd::SoundManager::GetInstance()->SetListenerPosition( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z );
			snd::SoundManager::GetInstance()->SetListenerOrientation( (float)std::sin(math::DegToRad(-gDebugCamera.GetAngle())), 0.0f, (float)std::cos(math::DegToRad(-gDebugCamera.GetAngle())),
																		0.0f, 1.0f, 0.0f );
		}
		else
		{
			renderer::OpenGL::GetInstance()->SetNearFarClip( 0.01f, 500.0f );
			renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

			renderer::OpenGL::GetInstance()->SetLookAt( -2.9f, 1.6917025f, 4.1825247f, 300.0f, -232.30847f, -452.83453f );

			snd::SoundManager::GetInstance()->SetListenerPosition( 0.0f, 2.0f, 10.0f );

#ifdef BASE_PLATFORM_iOS
			snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(0.0f)), 0.0f, std::cos(math::DegToRad(0.0f)), 0.0f, 1.0f, 0.0f );
#else
			snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#endif // 
		}

		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		renderer::OpenGL::GetInstance()->EnableLighting();

		math::Vec4 lightPos( 0.0f, 1.0f, 5.0f, 1.0f );
		math::Vec4 lightDiff( 1.0f, 1.0f, 1.0f, 1.0f );
		math::Vec4 lightAmb( 1.0f, 1.0f, 1.0f, 1.0f );
		math::Vec4 lightSpec( 1.0f, 1.0f, 1.0f, 1.0f );
		renderer::OpenGL::GetInstance()->EnableLight( 0 );
		renderer::OpenGL::GetInstance()->SetLightPosition( 0, lightPos );
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, lightDiff );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, lightAmb );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, lightSpec );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 1.0f );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.0f );

		glPushMatrix();
			glTranslatef( 0.0f, 0.0f, 0.0f );
			glRotatef( m_Rotation, 0.0f, 1.0f, 0.0f );
			if( m_pVehicleModel != 0 )
				m_pVehicleModel->Draw();
		glPopMatrix();

		renderer::OpenGL::GetInstance()->DisableLighting();
		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	// normal game
	if( m_GameOver ||
		m_LevelStartWait || 
		m_LevelCompleteCareer ||
		m_pState->GetId() != UI_MAINGAME )
		return;

	// default colour
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	if( m_DevData.allowDebugCam && gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.5f, 10000.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z, 
												gDebugCamera.GetTarget().X, gDebugCamera.GetTarget().Y, gDebugCamera.GetTarget().Z );
	
		snd::SoundManager::GetInstance()->SetListenerPosition( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( (float)std::sin(math::DegToRad(-gDebugCamera.GetAngle())), 0.0f, (float)std::cos(math::DegToRad(-gDebugCamera.GetAngle())),
																	0.0f, 1.0f, 0.0f );
	}
	else
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( m_LevelData.levelNearClip, m_LevelData.levelFarClip );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( m_CamPos.X, 
												m_CamPos.Y, 
												m_CamPos.Z, 
												m_CamLookAt.X, 
												m_CamLookAt.Y, 
												m_CamLookAt.Z );

		snd::SoundManager::GetInstance()->SetListenerPosition( m_Player->GetBody()->GetPosition().x + m_LevelData.levelCameraOffsetX, 
																		m_Player->GetBody()->GetPosition().y + m_LevelData.levelCameraOffsetY, 
																		m_CamZoomOut );
#ifdef BASE_PLATFORM_iOS
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#else
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#endif // 
	}
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetFogState( &m_LevelFog );
	renderer::OpenGL::GetInstance()->EnableLighting();
	renderer::OpenGL::GetInstance()->EnableLight(0);
	
	renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

	if( m_LevelLight.showVehicleLight )
	{
		math::Vec4 lightPos( m_Player->GetPosition() );
		lightPos.X += m_PlayerNightLight.lightPos.X;
		lightPos.Y += m_PlayerNightLight.lightPos.Y;
		lightPos.Z += m_PlayerNightLight.lightPos.Z;
		lightPos.W = m_PlayerNightLight.lightPos.W;
		renderer::OpenGL::GetInstance()->SetLightPosition( 1, lightPos );	

		renderer::OpenGL::GetInstance()->EnableLight(1);
	}
	else
		renderer::OpenGL::GetInstance()->DisableLight(1);

	m_Level.DrawBGQuad();

	m_Level.DrawBackground();
	
	m_Level.DrawPorts();
	
	m_Level.DrawPhysicsObjects();

	m_Level.DrawMainLevel();
	
	// customers 
	m_Customers->Draw( (pUIState->IsPaused() || pUIState->IsTimeUp()) );

	// player
	m_Player->Draw( m_DevData.disableMeshDraw, (pUIState->IsPaused() || pUIState->IsTimeUp()) );

	m_Level.DrawForeground();
	m_Level.DrawWater();

	renderer::OpenGL::GetInstance()->DisableLighting();

	m_Level.DrawEmitters();
	m_Player->DrawEmitters();
	m_Player->DrawNightMode();

	bool fogState = m_LevelFog.bFogState;
	m_LevelFog.bFogState = false;
	renderer::OpenGL::GetInstance()->SetFogState( &m_LevelFog );
	m_LevelFog.bFogState = fogState;

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// physics objects (have to be updated and then drawn)
	if( m_DevData.enablePhysicsDraw )
	{
		bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
		bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

		if( lightState )
			renderer::OpenGL::GetInstance()->DisableLighting();
		if( textureState )
			renderer::OpenGL::GetInstance()->DisableTexturing();

		if( !m_WasPausedLastFrame )
			UpdatePhysics( m_LastDelta );

		if( lightState )
			renderer::OpenGL::GetInstance()->EnableLighting();
		if( textureState )
			renderer::OpenGL::GetInstance()->EnableTexturing();
	}

#ifdef _DEBUG
	#ifdef BASE_PLATFORM_WINDOWS
		m_Player->DrawDebug();
		m_Level.DrawDebug();
		m_Customers->DrawDebug();

		renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );
		glPushMatrix();
			glTranslatef( m_CameraCollision.vCenterPoint.X, m_CameraCollision.vCenterPoint.Y, m_CameraCollision.vCenterPoint.Z );
			renderer::DrawSphere( m_CameraCollision.fRadius );
		glPopMatrix();
	#endif // 
#endif

	// 2D draws
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
		renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
		renderer::OpenGL::GetInstance()->DisableLighting();

#ifdef _DEBUG		
		int offset = 10;
		int xPos = 345;
		int yPos = 300;
		if( core::app::IsLandscape() &&
		   renderer::OpenGL::GetInstance()->GetIsRotated() )
		{
			xPos = core::app::GetOrientationHeight() - 150;
			yPos = core::app::GetOrientationWidth() - 20;
		}
		else
		{
			xPos = core::app::GetOrientationWidth() - 150;
			yPos = core::app::GetOrientationHeight() - 20;
		}

		DBGPRINT( xPos, yPos-=offset, "FUEL (%d)", m_Player->GetFuelCount() );
		DBGPRINT( xPos, yPos-=offset, "DELTA (%.6f)", m_LastDelta );
		DBGPRINT( xPos, yPos-=offset, "Lin Vel (%.2f  %.2f)", m_Player->GetBody()->GetLinearVelocity().x, m_Player->GetBody()->GetLinearVelocity().y );
		DBGPRINT( xPos, yPos-=offset, "N (%.2f  %.2f)", PhysicsWorld::ContactNormal.x, PhysicsWorld::ContactNormal.y );	
		DBGPRINT( xPos, yPos-=offset, "(%.2f  %.2f  %.2f)", m_Player->GetUnprojection().X, m_Player->GetUnprojection().Y, m_Player->GetUnprojection().Z );
		DBGPRINT( xPos, yPos-=offset, "DAMAGE (%.2f)", m_Player->GetDamage() );
		DBGPRINT( xPos, yPos-=offset, "ACCEL (%.2f  %.2f  %.2f)", input::gInputState.Accelerometers[0], input::gInputState.Accelerometers[1], input::gInputState.Accelerometers[2] );
		DBGPRINT( xPos, yPos-=offset, "IN WATER %d", m_PlayerInWater );
		DBGPRINT( xPos, yPos-=offset, "WATER STATE %d", m_CameraWaterState );
		
		DBGPRINT( xPos, yPos-=offset, "TOTAL TOUCHES %d", input::gInputState.nTouchCount );
		for(int i=0; i < input::MAX_TOUCHES; ++i )
		{
			if( input::gInputState.TouchesData[i].bActive )
				DBGPRINT( xPos, yPos-=offset, "INPUT (%d) %d %d", input::gInputState.TouchesData[i].nSpecialId, input::gInputState.TouchesData[i].nTouchX, input::gInputState.TouchesData[i].nTouchY );
		}

		m_Customers->DrawDebug2D( xPos, yPos );
#endif // _DEBUG
	}
}

/////////////////////////////////////////////////////
/// Method: ReplayLevel
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::ReplayLevel()
{
	m_InputSystem.ResetAllInput();

	if( m_pVehicleModel != 0 )
	{
		res::RemoveModel( m_pVehicleModel );
		m_pVehicleModel = 0;
	}
	m_Rotation = 0.0f;

	m_LevelCompleteCareer = false;
	m_LevelCompleteArcade = false;
	m_GameOver = false;

	m_CameraWaterState = -1;
	m_PlayerInWater = false;
	m_TickTime = 0.0f;

	math::Vec3 playerStart( m_Level.GetPlayerStart().X, m_Level.GetPlayerStart().Y, 0.0f );
	m_Player->SetRespawnPosition(playerStart, m_Level.GetPlayerStartDirection());
	m_Player->Respawn();
	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
		m_Player->SetLevelCustomerTotal( m_LevelData.levelArcadeCustomers );
	else
		m_Player->SetLevelCustomerTotal( m_LevelData.levelCustomerTotal );
	m_Player->SetCustomersComplete(0);
	m_Player->ClearMoneyToAdd();

	m_PlayerInWater = m_Player->InWater();
	if( m_PlayerInWater )
	{
		m_LevelLight = m_WaterLight;
		m_LevelFog = m_WaterFog;
		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

		m_Player->SetNightMode(m_LevelLight.showVehicleLight);
	}
	else
	{
		m_LevelLight = m_LevelLightBackup;
		m_LevelFog = m_LevelFogBackup;

		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
		renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
		renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
		renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
		renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

		m_Player->SetNightMode(m_LevelLight.showVehicleLight);
	}
	
	m_Level.ClearPhysicsObjects();
	m_Level.ResetPhysicsObjects();

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
		m_Level.ResetStarPickup();

	m_Customers->Reset();

	m_CamZoomOut = m_LevelData.levelCameraZ;

	m_CamPos.X = playerStart.X + m_LevelData.levelCameraOffsetX;
	m_CamPos.Y = playerStart.Y + m_LevelData.levelCameraOffsetY;
	m_CamPos.Z = m_LevelData.levelCameraZ;

	m_CamLookAt.X = playerStart.X;
	m_CamLookAt.Y = playerStart.Y + m_LevelData.levelCamCenterY;
	m_CamLookAt.Z = m_LevelData.levelCamCenterZ;

	m_NumFramesX = 0;
	m_NumFramesY = 0;

	// clear basic level data
	ProfileManager::GetInstance()->ClearActiveLevelData();
	m_pProfile = 0;

	std::memset( m_pLevelData, 0, sizeof(ProfileManager::ActiveLevelData) );
	m_pLevelData->levelArcadeCustomers = m_LevelData.levelArcadeCustomers;
	m_pLevelData->levelArcadeFuel = m_LevelData.levelArcadeFuel;
	m_pLevelData->levelArcadeMoney = m_LevelData.levelArcadeMoney;

	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
	{
		m_GameTime = 0.0f;

		m_pProfile = ProfileManager::GetInstance()->GetProfile();
		DBG_ASSERT( m_pProfile != 0 );

		m_Player->SetLivesCount(m_pProfile->currentLives);
		m_Player->SetTotalMoney(m_pProfile->currentMoney);
		m_Player->SetFuelCount(static_cast<int>(m_pProfile->currentFuel));
	}
	else
	{
		m_GameTime = m_LevelData.levelArcadeTime;

		m_Player->SetLivesCount(m_GameData.DEFAULT_LIVES_COUNT);
		m_Player->SetTotalMoney(0);
		m_Player->SetFuelCount(100);
	}

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
}

/////////////////////////////////////////////////////
/// Method: UpdatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::UpdatePhysics( float deltaTime )
{

#ifdef BASE_PLATFORM_iOS	
    #ifdef BASE_PLATFORM_tvOS
        PhysicsWorld::GetWorld()->Step( core::FPS60, PHYSICS_ITERATIONS  );
    #else
		PhysicsWorld::GetWorld()->Step( core::FPS60, PHYSICS_ITERATIONS  );
    #endif
#else
		bool frameLock30 = m_DevData.frameLock30;
		bool frameLock60 = m_DevData.frameLock60;

		if( frameLock30 )
		{
#ifdef BASE_PLATFORM_WINDOWS	
			if( wglSwapIntervalEXT )
				wglSwapIntervalEXT( 2 );
#endif // 			
			PhysicsWorld::GetWorld()->Step( PHYSICS_TIMESTEP, PHYSICS_ITERATIONS );
		}
		else if( frameLock60 )
		{
#ifdef BASE_PLATFORM_WINDOWS	
			if( wglSwapIntervalEXT )
				wglSwapIntervalEXT( 1 );
#endif // 
			PhysicsWorld::GetWorld()->Step( deltaTime, PHYSICS_ITERATIONS );
		}
		else
		{
			PhysicsWorld::GetWorld()->Step( deltaTime, PHYSICS_ITERATIONS );
		}
#endif 

#ifdef _DEBUG
		PhysicsWorld::GetWorld()->Validate();
#endif // _DEBUG
}

/////////////////////////////////////////////////////
/// Method: ClassifySphereToAABB
/// Params: None
///
/////////////////////////////////////////////////////
int ClassifySphereToAABB( collision::Sphere s, collision::AABB b )
{
    int inside = 0;
    float sqDist = 0.0f;
    for (int i = 0; i < 3; i++) 
	{
		float v = s.vCenterPoint[i];
		if (v < b.vBoxMin[i])
            sqDist += (b.vBoxMin[i] - v) * (b.vBoxMin[i] - v);
		else if (v > b.vBoxMax[i])
            sqDist += (v - b.vBoxMax[i]) * (v - b.vBoxMax[i]);
        else if (v >= b.vBoxMin[i] + s.fRadius && v <= b.vBoxMax[i] - s.fRadius)
            inside++;
    }

    if (inside == 3) 
		return 0;//INSIDE;

	if (sqDist > s.fRadius * s.fRadius) 
		return 1;//OUTSIDE;

    return 2;//INTERSECTING;
}

/////////////////////////////////////////////////////
/// Method: CheckCameraAgainstWater
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::CheckCameraAgainstWater()
{
	std::vector<Level::WaterGrid>& waterList =  m_Level.GetWaterGridList();
	std::vector<Level::WaterGrid>::iterator it = waterList.begin();

	while( it != waterList.end() )
	{
		int state = ClassifySphereToAABB( m_CameraCollision, (*it).grid->GetAABB() );
		if( m_CameraWaterState != state )
		{
			//if( state == 0 )
			{
				if( (m_CamPos.Y <= (*it).grid->GetAABB().vCenter.Y) &&
						m_PlayerInWater )
					{
						m_LevelLight = m_WaterLight;
						m_LevelFog = m_WaterFog;
						renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
						renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
						renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
						renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

						m_Player->SetNightMode(m_LevelLight.showVehicleLight);
					}
					else
					{
						m_LevelLight = m_LevelLightBackup;
						m_LevelFog = m_LevelFogBackup;

						renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
						renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
						renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
						renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
						renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

						m_Player->SetNightMode(m_LevelLight.showVehicleLight);
					}
			}

			m_CameraWaterState = state;
		}

		// next
		it++;
	}
}
