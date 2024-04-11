
/*===================================================================
	File: SpecialEndingState.cpp
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

#include "Resources/ModelResources.h"

#include "GameStates/IBaseGameState.h"
#include "Physics/PhysicsWorld.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/Cabby/LightAccess.h"
#include "ScriptAccess/Cabby/FogAccess.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/SpecialEndingState.h"
#include "GameStates/UI/GameCompleteUI.h"

#include "Profiles/ProfileManager.h"

namespace
{
#ifdef BASE_PLATFORM_WINDOWS
	input::Input win32Input;
#endif // BASE_PLATFORM_WINDOWS

	input::Input debugInput;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
SpecialEndingState::SpecialEndingState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
, m_pScriptData(0)
{
	m_InputSystem.IgnoreInput(true);

	m_LastDelta = 0.0f;

	// load from script
	m_pScriptData = GetScriptDataHolder();

	m_pIState = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
SpecialEndingState::~SpecialEndingState()
{
	m_InputSystem.IgnoreInput(false);

	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}

	PhysicsWorld::SetPlayer( 0 );

	m_Level.Release();
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void SpecialEndingState::Enter()
{
	b2Vec2 areaMin( -100.0f, -100.0f );
	b2Vec2 areaMax(100.0f, 100.0f);
	b2Vec2 gravity(0.0f,0.0f);
	
	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();

	PhysicsWorld::Create( areaMin, areaMax, gravity, false );

	script::LoadScript( "assets/levels/special_ending/setup.lua" );
	script::LuaCallFunction( "Load_ending", 0, 0 );

	m_LevelData = m_pScriptData->GetLevelData();

	m_GameCamera = GameSystems::GetInstance()->GetGameCamera();
	m_GameData = m_pScriptData->GetGameData();

	m_LoadedLevel = false;
	m_CamMoveTimer = 0.0f;
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void SpecialEndingState::Exit()
{
	// clear the vehicle
	m_pScriptData->ReleaseVehicleList();

	PhysicsWorld::Destroy();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int SpecialEndingState::TransitionIn()
{

	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int SpecialEndingState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: None
///
/////////////////////////////////////////////////////
void SpecialEndingState::Update( float deltaTime )
{
	m_LastDelta = deltaTime;
	UpdatePhysics( deltaTime );

	m_GameCamera->Update( deltaTime );

	if( m_DevData.allowDebugCam )
	{
		if( debugInput.IsKeyPressed( input::KEY_9, true ) )
		{
			if( gDebugCamera.IsEnabled() )
				gDebugCamera.Disable();	
			else
				gDebugCamera.Enable();
		}

		gDebugCamera.Update(deltaTime);
	}	

	if( m_LoadedLevel )
		m_Level.Update( deltaTime );

	m_pIState = reinterpret_cast<IState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( m_pIState->GetId() == UI_GAMECOMPLETE )
	{
		GameCompleteUI* pUIState = static_cast<GameCompleteUI*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

		if( !m_LoadedLevel && 
			pUIState->FinishedFade() )
		{
			m_Level.Initialise();
			m_Level.LoadMeshData( m_LevelData.levelModel );
			m_Level.LoadBackgroundModel( m_LevelData.levelBackground );
			m_Level.LoadForegroundModel( m_LevelData.levelForeground );
			m_Level.LoadComplexPhysicsData( m_LevelData.levelPhysicsComplex, PhysicsWorld::GetWorld() );
			m_Level.LoadData( m_LevelData.levelData );

			m_Level.SetTimeOfDay( Level::LEVEL_DAY );
			m_LevelFog = GetFogData( m_LevelData.levelFogDay );
			m_LevelLight = GetLightData( m_LevelData.levelDayLight );
			m_Level.SetupBGQuad( m_LevelData.levelBGTextureDay, m_LevelData.levelBGDayUseFog, m_LevelData.levelBGDims, m_LevelData.levelBGPos ); 

			renderer::OpenGL::GetInstance()->EnableLight( 0 );

			renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );
			renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LevelLight.lightDiffuse );
			renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LevelLight.lightAmbient );
			renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LevelLight.lightSpecular );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, m_LevelLight.lightAttenuation.X );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, m_LevelLight.lightAttenuation.Y );
			renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, m_LevelLight.lightAttenuation.Z );

			int vehicleId = m_pScriptData->GetDefaultVehicle();
			int packId = vehicleId / m_GameData.MAX_ITEMS_PER_PACK;
			int vehicleOffsetId = vehicleId % m_GameData.MAX_ITEMS_PER_PACK;

			const char* funcCall = GetScriptDataHolder()->GetVehiclePackList()[packId]->pPackVehicleInfo[vehicleOffsetId].setupFunction;

			// call the vehicle setup
			script::LuaCallFunction( funcCall, 0, 0 );

			// in this state there should only be one vehicle
			ScriptDataHolder::VehicleScriptData* pVehicleData = m_pScriptData->GetVehicleList()[0];
			DBG_ASSERT( pVehicleData != 0 );

			m_Player = new Player( m_InputSystem, *pVehicleData, m_Level );
			DBG_ASSERT( m_Player != 0 );

			m_Player->Setup();

			math::Vec3 playerStart( m_Level.GetPlayerStart().X, m_Level.GetPlayerStart().Y, 0.0f );
			m_Player->SetRespawnPosition(playerStart, m_Level.GetPlayerStartDirection());
			m_Player->Respawn();

			if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
				m_Player->SetLevelCustomerTotal( m_LevelData.levelArcadeCustomers );
			else
				m_Player->SetLevelCustomerTotal( m_LevelData.levelCustomerTotal );

			PhysicsWorld::SetPlayer( m_Player );
			m_Player->SetRotationAngle( 45.0f );
			m_Player->SetInputState( false );

			// let the vehicle settle
			for( int i=0; i<60; ++i )
			{
				UpdatePhysics( deltaTime );
				m_Player->Update( deltaTime );
			}

			m_GameCamera->SetLerp( false );
			
			m_CamPos = math::Vec3(-27.32f, 9.60f, 14.54f);
			m_CamLookAt = math::Vec3(-35.02f, -86.39f, -535.39f );

			m_GameCamera->SetPosition( m_CamPos );
			m_GameCamera->SetTarget( m_CamLookAt );

			m_LoadedLevel = true;
			m_CamMoveTimer = 0.0f;
		}
	}

	if( m_LoadedLevel )
	{
		m_CamMoveTimer += deltaTime;

		if( m_CamMoveTimer >= 5.0f )
		{
			m_GameCamera->SetLerp( true );
			m_GameCamera->SetLerpSpeed( 0.25f );

			m_CamPos = math::Vec3(25.27f, 10.45f, 12.05f);
			m_CamLookAt = math::Vec3(28.57f, -7.54f, -537.92f );

			m_GameCamera->SetPosition( m_CamPos );
			m_GameCamera->SetTarget( m_CamLookAt );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void SpecialEndingState::Draw()
{
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
		renderer::OpenGL::GetInstance()->SetNearFarClip( m_LevelData.levelNearClip, m_LevelData.levelFarClip );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		math::Vec3 pos = m_GameCamera->GetPosition();
		math::Vec3 target = m_GameCamera->GetTarget();

		renderer::OpenGL::GetInstance()->SetLookAt( pos.X, pos.Y, pos.Z, 
													target.X, target.Y, target.Z );

		snd::SoundManager::GetInstance()->SetListenerPosition( 0.0f, 2.0f, 10.0f );
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_LoadedLevel )
	{
		renderer::OpenGL::GetInstance()->SetFogState( &m_LevelFog );
		renderer::OpenGL::GetInstance()->EnableLighting();
		renderer::OpenGL::GetInstance()->EnableLight(0);
	
		renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LevelLight.lightPos );

		m_Level.DrawBGQuad();

		m_Level.DrawBackground();
	
		m_Level.DrawPorts();
	
		m_Level.DrawPhysicsObjects();

		m_Level.DrawMainLevel();
	
		m_Player->Draw( );

		m_Level.DrawForeground();

		m_Level.DrawWater();

		renderer::OpenGL::GetInstance()->DisableLighting();

		m_Level.DrawEmitters();
	}

	renderer::OpenGL::GetInstance()->DisableLighting();
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: UpdatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void SpecialEndingState::UpdatePhysics( float deltaTime )
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
