
/*===================================================================
	File: MainGameState.cpp
	Game: H8

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
#include "PhysicsBase.h"
#include "ScriptBase.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"

#include "H8Consts.h"
#include "H8.h"

#include "Effects/Emitter.h"
#include "Resources/ModelResources.h"
#include "Resources/EmitterResources.h"
#include "Resources/StringResources.h"
#include "Resources/TextureResources.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/IBaseGameState.h"
#include "Player/Player.h"
#include "ScriptAccess/ScriptAccess.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/GameUI.h"
#include "GameStates/UI/BackgroundUI.h"

#include "GameStates/MainGameState.h"

#include "Profiles/ProfileManager.h"

namespace
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	input::Input debugInput;
	//bool drawGameUI = true;
}

void DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color);

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
	m_Player = 0;
	m_Core = 0;
	m_EnemyManager = 0;

	m_BG = 0;

	m_GameState = GameState_GameStartWait;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainGameState::~MainGameState()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Enter()
{
	int i=0;

	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();
	m_GameData = m_pScriptData->GetGameData();

	m_GameCamera = GameSystems::GetInstance()->GetGameCamera();

	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	GameSystems::GetInstance()->CreatePhysics();
	script::LuaCallFunction( "PostPhysicsSetup", 0, 0 );

	GameSystems::GetInstance()->LoadColourIndexList();

	GameSystems::GetInstance()->ResetScore();
	GameSystems::GetInstance()->SetPersonalBest( ProfileManager::GetInstance()->GetProfile()->personalBest );

	// shield/player
	m_Player = new Player( m_InputSystem );
	m_Player->Initialise();
	m_Player->Setup( m_DevData.shieldSetup );
	GameSystems::GetInstance()->SetPlayer( m_Player );

	m_Player->SetAnimState( Player::AnimState_Initial );

	// core
	m_Core = new Core;
	DBG_ASSERT( m_Core != 0 );
	m_Core->Setup( m_GameData.CORE_RADIUS );

	m_Core->SetPupilState( Core::PupilState_Normal );
	GameSystems::GetInstance()->SetCore( m_Core );

	// enemies
	m_EnemyManager = new EnemyManager;
	DBG_ASSERT( m_EnemyManager != 0 );
	m_EnemyManager->Initialise();
	m_EnemyManager->PauseAttack();

	// BG
	m_BG = new BackgroundUI;
	DBG_ASSERT( m_BG != 0 );
	m_BG->Enter();

	math::Vec3 upVector( 0.0f, 1.0f, 0.0f );
	math::Vec3 posVector( 0.0f, 0.0f, m_GameData.CAMERA_POS_TITLE_Z );
	m_GameCamera->SetUpVector( upVector );
	m_GameCamera->SetPosition( posVector );
	m_GameCamera->SetTarget( zeroVec );

	m_CountdownToCoreChange = math::RandomNumber( m_GameData.CORE_CHANGE_TIME_MIN, m_GameData.CORE_CHANGE_TIME_MAX );

	m_LevelUpAudio = AudioSystem::GetInstance()->AddAudioFile( 60 );

	m_RandomBeatTime = math::RandomNumber( 2.0f, 6.0f );
	
	for( i=0; i < RANDOM_BEATS; ++i )
		m_RandomBeatBufferIds[i] = AudioSystem::GetInstance()->AddAudioFile( 80+i );
	
	// sync PB
	//ProfileManager::GetInstance()->GetProfile()->personalBest = GameSystems::GetInstance()->GetPersonalBest();
	//support::Scores::GetInstance()->SendScore( GameSystems::GetInstance()->GetPersonalBest(), "uk.co.hiddengames.firewall.hiscores");
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Exit()
{
	m_GameCamera = 0;

	ProfileManager::GetInstance()->SaveProfile();

	GameSystems::GetInstance()->DestroyEffectPool();

	// turn off lights for possible complete state
	renderer::OpenGL::GetInstance()->DisableLight( 0 );
	renderer::OpenGL::GetInstance()->DisableLight( 1 );

	core::app::SetAccelerometerState(false);
	
	renderer::OpenGL::GetInstance()->SetMSAAState(false);

	if( m_BG != 0 )
	{
		m_BG->Exit();
		delete m_BG;
		m_BG = 0;
	}

	GameSystems::GetInstance()->SetCore( 0 );

	if( m_Core != 0 )
	{
		delete m_Core;
		m_Core = 0;
	}

	if( m_EnemyManager != 0 )
	{
		delete m_EnemyManager;
		m_EnemyManager = 0;
	}

	GameSystems::GetInstance()->SetPlayer( 0 );

	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}

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
	m_LastDelta = deltaTime;

	if( m_BG != 0 )
		m_BG->Update(deltaTime);

	m_RandomBeatTime -= deltaTime;
	if( m_RandomBeatTime < 0.0f )
	{
		m_RandomBeatTime = math::RandomNumber( 3.0f, 10.0f );
	
		int randomIndex = math::RandomNumber( 0, RANDOM_BEATS-1 );
		if( m_RandomBeatBufferIds[randomIndex] != snd::INVALID_SOUNDBUFFER )
		{
			float randomPitch = math::RandomNumber( -0.2f, 0.4f );
			AudioSystem::GetInstance()->PlayAudio( m_RandomBeatBufferIds[randomIndex], math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 1.0f+randomPitch, 0.1f );
		}	
	}

	GameUI* pUIState = static_cast<GameUI *>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );

	if( pUIState != 0 &&
		pUIState->GetId() == UI_GAME )
	{
		if( pUIState->IsPaused() )
		{
			m_Core->Update( deltaTime );
			return;
		}
	}

	if( m_DevData.allowDebugCam )
	{
		if( debugInput.IsKeyPressed( input::KEY_9, true ) )
		{
			if( gDebugCamera.IsEnabled() )
				gDebugCamera.Disable();	
			else
				gDebugCamera.Enable();
		}

		if( gDebugCamera.IsEnabled() )
			gDebugCamera.Update(deltaTime);
	}	

	/*if( debugInput.IsKeyPressed( input::KEY_LBRACKET, true, true ) )
	{
		m_Player->PreviousLevel();
	}
	else if( debugInput.IsKeyPressed( input::KEY_RBRACKET, true, true ) )
	{
		m_Player->NextLevel();
	}

	if( debugInput.IsKeyPressed( input::KEY_1, true, false ) )
	{
		m_Core->NextPupil();
	}
	else if( debugInput.IsKeyPressed( input::KEY_2, true, false ) )
	{
		m_EnemyManager->DestroyActiveEnemies(false);
	}*/

	// wait for the player
	if( m_Player->GetAnimState() != Player::AnimState_InGame )
	{
		if( !m_EnemyManager->IsPaused() )
			m_EnemyManager->PauseAttack();
	}
	else
	{
		if( m_EnemyManager->IsPaused() )
			m_EnemyManager->UnPauseAttack();
	}

	UpdatePhysics( deltaTime );

	if( m_Core->GetPetalCount() <= 0 )
	{
		ChangeGameState( MainGameState::GameState_End );
		pUIState->SetGameOver(true);

		ProfileManager::GetInstance()->GetProfile()->personalBest = GameSystems::GetInstance()->GetPersonalBest();
		ProfileManager::GetInstance()->SaveProfile();
	}

	// time to level up
	if( GameSystems::GetInstance()->GetLevelUpCounter() >= m_GameData.LEVELUP_COUNT )
	{
		if( m_Player->GetAnimState() == Player::AnimState_InGame )
		{
			m_EnemyManager->DestroyActiveEnemies(true);
			m_EnemyManager->PauseAttack();

			m_Player->SetLevelUp( true );
			m_Player->SetAnimState( Player::AnimState_Out );

			// play level up
			if( m_LevelUpAudio != snd::INVALID_SOUNDBUFFER )
			{
				AudioSystem::GetInstance()->PlayAudio( m_LevelUpAudio, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
			}
						
			GameSystems::GetInstance()->AddTextToConsole( "***********************************" );
			GameSystems::GetInstance()->AddTextToConsole( "Firmware upgrade started....." );
			GameSystems::GetInstance()->AddTextToConsole( ".........................." );
			GameSystems::GetInstance()->AddTextToConsole( "....upgrade complete" );
		}
		else
		if( m_Player->GetAnimState() == Player::AnimState_Out &&
			m_Player->IsAnimComplete() )
		{
			m_Player->NextLevel();

			GameSystems::GetInstance()->ResetLevelUpCounter();
			
			m_Core->SetPupilState( Core::PupilState_Heart, 0.0f );

			m_Player->SetAnimState( Player::AnimState_In );
			
			GameSystems::GetInstance()->IncrementLevelCounter();
			pUIState->SetGameLevel( GameSystems::GetInstance()->GetLevelCounter() );

			m_EnemyManager->UnPauseAttack();

			GameSystems::GetInstance()->AddTextToConsole( "Firewall back online..." );
			GameSystems::GetInstance()->AddTextToConsole( ".....ready" );

			GameSystems::GetInstance()->SpawnHearts();
		}	
	}

	m_Player->Update( deltaTime );

	m_Core->Update( deltaTime );

	m_EnemyManager->Update( deltaTime );
	
	m_GameCamera->Update(deltaTime);

	// draw level emitters
	GameSystems::GetInstance()->UpdateEffectPool(deltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Draw()
{
	IState* pState = m_pStateManager->GetSecondaryStateManager()->GetCurrentState();

	if( pState != 0 &&
		pState->GetId() != UI_GAME )
	{
		return;
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	if( m_BG != 0 )
		m_BG->Draw();

	if( m_DevData.allowDebugCam && gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.5f, 100000.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		renderer::OpenGL::GetInstance()->SetLookAt( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z, 
												gDebugCamera.GetTarget().X, gDebugCamera.GetTarget().Y, gDebugCamera.GetTarget().Z );
	
		snd::SoundManager::GetInstance()->SetListenerPosition( gDebugCamera.GetPosition().X, gDebugCamera.GetPosition().Y, gDebugCamera.GetPosition().Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( (float)std::sin(math::DegToRad(-gDebugCamera.GetAngle())), 0.0f, (float)std::cos(math::DegToRad(-gDebugCamera.GetAngle())),
																	0.0f, 1.0f, 0.0f );
	}
	else
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 1.0f, 100.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		m_GameCamera->SetupCamera();

#ifdef BASE_PLATFORM_iOS
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#else
		snd::SoundManager::GetInstance()->SetListenerOrientation( std::sin(math::DegToRad(180.0f)), 0.0f, std::cos(math::DegToRad(180.0f)), 0.0f, 1.0f, 0.0f );
#endif // 

		gDebugCamera.SetPosition(m_GameCamera->GetPosition());
		gDebugCamera.SetTarget(m_GameCamera->GetTarget());
	}

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// physics objects (have to be updated and then drawn)
	/*if( m_DevData.enablePhysicsDraw )
	{
		bool lightState = renderer::OpenGL::GetInstance()->GetLightingState();
		bool textureState = renderer::OpenGL::GetInstance()->GetTextureState();

		if( lightState )
			renderer::OpenGL::GetInstance()->DisableLighting();
		if( textureState )
			renderer::OpenGL::GetInstance()->DisableTexturing();

		for (b2Body* b = physics::PhysicsWorldB2D::GetWorld()->GetBodyList(); b; b = b->GetNext())
		{
			const b2Transform& xf = b->GetTransform();
			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				physics::PhysicsIdentifier* physId = 0;
				physId = reinterpret_cast<physics::PhysicsIdentifier*>( f->GetUserData() );
				b2Filter fd = f->GetFilterData();

				float multi = 1.0f;
				int colourIndex = -1;

				if( physId != 0 )
				{
					colourIndex = physId->GetNumeric1();

					if( fd.maskBits & ENEMY_CATEGORY ||
						fd.maskBits & NOTHING_CATEGORY )
						colourIndex = physId->GetNumeric2();
				}

				if( fd.maskBits & NOTHING_CATEGORY )
					multi = 0.2f;

				if (b->IsActive() == false)
				{
					if( physId != 0 )
						DrawShape(f, xf, GameSystems::GetInstance()->GetColourForIndex( colourIndex, multi ) );
					else
						DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.3f));
				}
				else if (b->GetType() == b2_staticBody)
				{
					if( physId != 0 )
						DrawShape(f, xf, GameSystems::GetInstance()->GetColourForIndex( colourIndex, multi ) );
					else
						DrawShape(f, xf, b2Color(0.5f, 0.9f, 0.5f));
				}
				else if (b->GetType() == b2_kinematicBody)
				{
					DrawShape(f, xf, b2Color(0.5f, 0.5f, 0.9f));
				}
				else if (b->IsAwake() == false)
				{
					if( physId != 0 )
						DrawShape(f, xf, GameSystems::GetInstance()->GetColourForIndex( colourIndex, multi ) );
					else
						DrawShape(f, xf, b2Color(0.6f, 0.6f, 0.6f));
				}
				else
				{
					if( physId != 0 )
						DrawShape(f, xf, GameSystems::GetInstance()->GetColourForIndex( colourIndex, multi ) );
					else
						DrawShape(f, xf, b2Color(0.9f, 0.7f, 0.7f));
				}
			}
		}

		if( lightState )
			renderer::OpenGL::GetInstance()->EnableLighting();
		if( textureState )
			renderer::OpenGL::GetInstance()->EnableTexturing();
	}*/
	
	// data/enemies
	m_EnemyManager->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// core
	m_Core->Draw();

	// player
	m_Player->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// draw level emitters
	GameSystems::GetInstance()->DrawEffectPool();

	#ifdef BASE_PLATFORM_WINDOWS
		m_Player->DrawDebug();
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

	static math::Vec2 accumVec(0.0f,0.0f );

	DBGPRINT( xPos, yPos-=offset, "DELTA (%.6f)", m_LastDelta );
	DBGPRINT( xPos, yPos-=offset, "ACCEL (%.2f  %.2f  %.2f)", input::gInputState.Accelerometers[0], input::gInputState.Accelerometers[1], input::gInputState.Accelerometers[2] );
	DBGPRINT( xPos, yPos-=offset, "SCORE (%llu)", GameSystems::GetInstance()->GetCurrentScore() );
	DBGPRINT( xPos, yPos-=offset, "LEVEL UP COUNTER (%d)", GameSystems::GetInstance()->GetLevelUpCounter() );
	DBGPRINT( xPos, yPos-=offset, "SHIELD WRAP (%d)", GameSystems::GetInstance()->GetWrapAroundCounter() );	

	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
	{
		accumVec = input::gInputState.TouchesData[input::FIRST_TOUCH].vAccumulatedVec;
	}
		
	DBGPRINT( xPos, yPos-=offset*2, "VEC (%.2f  %.2f )", accumVec.X, accumVec.Y );
	DBGPRINT( xPos, yPos-=offset*2, "LEN (%.2f)", accumVec.length() );
	math::Vec2 normAcc = math::Normalise( accumVec );
	DBGPRINT( xPos, yPos-=offset*2, "VEC NORM(%.2f  %.2f )", normAcc.X, normAcc.Y );
}

/////////////////////////////////////////////////////
/// Method: UpdatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::UpdatePhysics( float deltaTime )
{
	ClearContacts();

	physics::PhysicsWorldB2D::GetWorld()->Step( PHYSICS_TIMESTEP, PHYSICS_VEL_ITERATIONS, PHYSICS_POS_ITERATIONS );
}

/////////////////////////////////////////////////////
/// Method: CheckAchievements
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::CheckAchievements(float deltaTime)
{

}

/////////////////////////////////////////////////////
/// Method: Restart
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameState::Restart()
{
	GameSystems::GetInstance()->SetPersonalBest( ProfileManager::GetInstance()->GetProfile()->personalBest );

	m_EnemyManager->DestroyActiveEnemies(false);

	m_EnemyManager->PauseAttack();

	m_Player->SetLevelUp(false);
	m_Player->Reset();

	m_Core->Reset();

	GameSystems::GetInstance()->ResetLevelUpCounter();
	GameSystems::GetInstance()->ResetWrapAroundCounter();
	GameSystems::GetInstance()->ResetLevelUpCounter();
	GameSystems::GetInstance()->ResetScore();
}

/////////////////////////////////////////////////////
/// Method: ChangeGameState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void MainGameState::ChangeGameState( GameState state )
{
	if( m_GameState == GameState_GameStartWait )
	{
		// new game
		if( state == GameState_Game )
		{
			m_Core->Reset();

			Restart();

			GameUI* pUIState = static_cast<GameUI *>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );

			if( pUIState != 0 &&
				pUIState->GetId() == UI_GAME )
			{
				pUIState->ShowInstructions();
			}

			GameSystems::GetInstance()->AddTextToConsole( " " );
			GameSystems::GetInstance()->AddTextToConsole( "***********************************" );
			GameSystems::GetInstance()->AddTextToConsole( "Rebooting" );
			GameSystems::GetInstance()->AddTextToConsole( "Please wait..........." );
			GameSystems::GetInstance()->AddTextToConsole( "............................" );
			GameSystems::GetInstance()->AddTextToConsole( "..............................." );
			GameSystems::GetInstance()->AddTextToConsole( "Reboot complete" );
			GameSystems::GetInstance()->AddTextToConsole( "Boot sequence started" );
			GameSystems::GetInstance()->AddTextToConsole( "ROM OK!" );
			GameSystems::GetInstance()->AddTextToConsole( "RAM OK!" );
			GameSystems::GetInstance()->AddTextToConsole( "EEPROM OK!" );
			GameSystems::GetInstance()->AddTextToConsole( "Boot sequence complete" );
		}
	}
	else
	if( m_GameState == GameState_Game )
	{
		// reset
		if( state == GameState_GameStartWait )
		{
			m_Player->SetLevelUp(true);
			m_Player->SetAnimState( Player::AnimState_Out );

			m_EnemyManager->DestroyActiveEnemies(false);

			m_EnemyManager->PauseAttack();

			m_Core->Reset();

			GameSystems::GetInstance()->AddTextToConsole( "**************************" );
			GameSystems::GetInstance()->AddTextToConsole( "Reset" );
			GameSystems::GetInstance()->AddTextToConsole( "Please Wait..........." );
			GameSystems::GetInstance()->AddTextToConsole( ".................................." );
			GameSystems::GetInstance()->AddTextToConsole( ".................................." );
			GameSystems::GetInstance()->AddTextToConsole( "Reset Complete" );
		}
		else
		if( state == GameState_End )
		{
			m_Player->SetLevelUp(true);
			m_Player->SetAnimState( Player::AnimState_Out );

			m_EnemyManager->DestroyActiveEnemies(false);

			m_EnemyManager->PauseAttack();

			//m_Core->Reset();
		}
	}
	else
	if( m_GameState == GameState_End )
	{
		// new game
		if( state == GameState_Game )
		{
			m_Player->SetLevelUp(false);

			GameSystems::GetInstance()->AddTextToConsole( " " );
			GameSystems::GetInstance()->AddTextToConsole( "********************" );
			GameSystems::GetInstance()->AddTextToConsole( "Rebooting" );
			GameSystems::GetInstance()->AddTextToConsole( "Please Wait..........." );
			GameSystems::GetInstance()->AddTextToConsole( ".................................." );
			GameSystems::GetInstance()->AddTextToConsole( "Waiting for signal.................................." );
			GameSystems::GetInstance()->AddTextToConsole( "...memory clear" );
			GameSystems::GetInstance()->AddTextToConsole( "..............reboot complete" );
		}
		else
		if( state == GameState_GameStartWait )
		{
			m_Core->Reset();

			GameSystems::GetInstance()->SetBGMPitch( 1.0f );
		}
	}

	m_GameState = state;
}

/////////////////////////////////////////////////////
/// Method: DrawShape
/// Params: None
///
/////////////////////////////////////////////////////
void DrawShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color)
{
	switch (fixture->GetType())
	{
	case b2Shape::e_circle:
		{
			b2CircleShape* circle = (b2CircleShape*)fixture->GetShape();

			b2Vec2 center = b2Mul(xf, circle->m_p);
			float32 radius = circle->m_radius;
			b2Vec2 axis = b2Mul(xf.q, b2Vec2(1.0f, 0.0f));

			physics::PhysicsWorldB2D::GetDrawer()->DrawSolidCircle(center, radius, axis, color);
		}
		break;

	case b2Shape::e_edge:
		{
			b2EdgeShape* edge = (b2EdgeShape*)fixture->GetShape();
			b2Vec2 v1 = b2Mul(xf, edge->m_vertex1);
			b2Vec2 v2 = b2Mul(xf, edge->m_vertex2);
			physics::PhysicsWorldB2D::GetDrawer()->DrawSegment(v1, v2, color);
		}
		break;

	case b2Shape::e_chain:
		{
			b2ChainShape* chain = (b2ChainShape*)fixture->GetShape();
			int32 count = chain->m_count;
			const b2Vec2* vertices = chain->m_vertices;

			b2Vec2 v1 = b2Mul(xf, vertices[0]);
			for (int32 i = 1; i < count; ++i)
			{
				b2Vec2 v2 = b2Mul(xf, vertices[i]);
				physics::PhysicsWorldB2D::GetDrawer()->DrawSegment(v1, v2, color);
				physics::PhysicsWorldB2D::GetDrawer()->DrawCircle(v1, 0.05f, color);
				v1 = v2;
			}
		}
		break;

	case b2Shape::e_polygon:
		{
			b2PolygonShape* poly = (b2PolygonShape*)fixture->GetShape();
			int32 vertexCount = poly->m_count;
			b2Assert(vertexCount <= b2_maxPolygonVertices);
			b2Vec2 vertices[b2_maxPolygonVertices];

			for (int32 i = 0; i < vertexCount; ++i)
			{
				vertices[i] = b2Mul(xf, poly->m_vertices[i]);
			}

			physics::PhysicsWorldB2D::GetDrawer()->DrawSolidPolygon(vertices, vertexCount, color);
		}
		break;
            
    default:
        break;
	}
}
