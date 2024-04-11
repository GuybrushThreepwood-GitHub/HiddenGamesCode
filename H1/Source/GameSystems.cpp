
/*===================================================================
	File: GameSystems.cpp
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "SupportBase.h"

#include "H1.h"
#include "H1Consts.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"
#include "Effects/Emitter.h"

#include "ScriptAccess/ScriptAccess.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/AchievementList.h"

#include "Profiles/ProfileManager.h"

#include "GameStates/UI/AchievementUI.h"

#include "GameSystems.h"

GameSystems* GameSystems::ms_Instance = 0;

namespace
{
	math::Vec3 zeroVec(0.0f,0.0f,0.0f);
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameSystems::GameSystems( InputSystem& inputSystem )
	: m_InputSystem(inputSystem)
{
	DBG_ASSERT_MSG( (ms_Instance == 0), "GameSystems instance already created" );

	ms_Instance = this;

	m_pScriptData = 0;

	m_pScriptData = GetScriptDataHolder();
	m_DevData = m_pScriptData->GetDevData();
	//m_CameraData = m_pScriptData->GetCameraData();
	m_GameData = m_pScriptData->GetGameData();

	m_GameMode = GAMEMODE_RINGRUN;
	m_GameModeIndex = 0;
	m_GameModeExtraData = -1;

	// update some values before use
	math::Vec3 scaleFactor( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_GameData.ACHIEVEMENT_ICON_MAXX	*= scaleFactor.X;
	m_GameData.ACHIEVEMENT_ICON_SPEED	*= scaleFactor.X;

	//m_GameData.ANALOGUE_DEAD_ZONE		*= scaleFactor.X;
	
	// adbar assumes landscape
/*	m_GameData.ADBAR_PHONE_POSX		*= scaleFactor.Y;
	m_GameData.ADBAR_PHONE_POSY		*= scaleFactor.X;
	m_GameData.ADBAR_PHONEHD_POSX	*= scaleFactor.Y;
	m_GameData.ADBAR_PHONEHD_POSY	*= scaleFactor.X;
	m_GameData.ADBAR_TABLET_POSX	*= scaleFactor.Y;
	m_GameData.ADBAR_TABLET_POSY	*= scaleFactor.X;
	m_GameData.ADBAR_TABLETHD_POSX	*= scaleFactor.Y;
	m_GameData.ADBAR_TABLETHD_POSY	*= scaleFactor.X;
*/
	m_pScriptData->SetGameData(m_GameData);

	m_Level = 0;
	m_Player = 0;

	m_StageFile = 0;
	m_StageSetupFunc = 0;

	//m_GameCamera = 0;
	m_AchievementUI = 0;
	m_RayObject = 0;

	m_TargetsDestroyed = 0;
	m_LapsComplete = 0;

	m_GateComplete = 0;
	m_TotalGates = 0;

	m_ObjectDestroyBufferId = snd::INVALID_SOUNDBUFFER;
	m_TargetDestroyBufferId = snd::INVALID_SOUNDBUFFER;
	m_RingPassBufferId = snd::INVALID_SOUNDBUFFER;

	m_LevelCompleteBufferId = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameSystems::~GameSystems()
{
	if( m_Level != 0 )
	{
		delete m_Level;
		m_Level = 0;
	}

	ms_Instance = 0;
}

/////////////////////////////////////////////////////
/// Method: LoggedIn
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoggedIn()
{
	// get latest achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: GetAchievementId
/// Params: [in]achievementString
///
/////////////////////////////////////////////////////
int GameSystems::GetAchievementId( const char* achievementId )
{
	return res::GetAchievementIndex( achievementId );
}

/////////////////////////////////////////////////////
/// Method: GetAchievementString
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
const char* GameSystems::GetAchievementString( int achievementId )
{
	return res::GetAchievementString( achievementId );
}

/////////////////////////////////////////////////////
/// Method: AchievementSubmitted
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AchievementSubmitted( int achievementId, int errorId )
{
	if( errorId == 0 )
	{
	
	}
	else
	// called when an achievement was successful/failed
	if( errorId != 0 )
	{
		DBGLOG( "%s failed to be submitted\n", res::GetAchievementString(achievementId) );
	}
};

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void GameSystems::Update( float deltaTime )
{
	UpdateAchievementUI( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: CreatePlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePlayer()
{
	//m_Player = new Player( m_InputSystem );
	//DBG_ASSERT_MSG( (m_Player != 0), "Could not create player" );
}

/////////////////////////////////////////////////////
/// Method: DestroyPlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPlayer()
{
	if( m_Player != 0 )
	{
		delete m_Player;
		m_Player = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePhysics()
{
	physics::PhysicsWorldODE::Create();

	//dSpaceID spaceId = dHashSpaceCreate(0);
	//dHashSpaceSetLevels( spaceId, 4, 32 );
	//physics::PhysicsWorldODE::SetSpace( spaceId );

	m_RayObject = new RayObject( physics::PhysicsWorldODE::GetSpace(), 1.0f );
}

/////////////////////////////////////////////////////
/// Method: DestroyPhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPhysics()
{
	if( m_RayObject != 0 )
	{
		delete m_RayObject;
		m_RayObject = 0;
	}
	
	physics::PhysicsWorldODE::Destroy();
}

/////////////////////////////////////////////////////
/// Method: CreateScore
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateScore()
{
    support::Scores::Create(this);

	m_AchievementUI = new AchievementUI;
	DBG_ASSERT( m_AchievementUI != 0 );

}

/////////////////////////////////////////////////////
/// Method: DestroyScore
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyScore()
{
    support::Scores::Destroy();

	if( m_AchievementUI != 0)
	{
		delete m_AchievementUI;
		m_AchievementUI = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreatePurchases
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePurchase()
{
    support::Purchase::Create();
}

/////////////////////////////////////////////////////
/// Method: DestroyPurchases
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPurchase()
{
    support::Purchase::Destroy();
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AwardAchievement( int achievementId, int textId )
{
	if( support::Scores::IsInitialised() )
	{
		if( achievementId != -1 )
		{
			if( support::Scores::GetInstance()->IsNewAchievement( achievementId ) )
				PrepareAchievementUI( achievementId, textId );
			else
			{
				if( textId != -1 )
					PrepareAchievementUI( -1, textId );
			}	
			
			support::Scores::GetInstance()->AwardAchievement( 100.0f, achievementId );
		}
		else
		{
			if( textId != -1 )
			{
				PrepareAchievementUI( achievementId, textId );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: AwardAchievement
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AwardAchievement( int achievementId, const char* text )
{
	if( support::Scores::IsInitialised() )
	{
		if( achievementId != -1 )
		{
			if( support::Scores::GetInstance()->IsNewAchievement( achievementId ) )
				PrepareAchievementUI( achievementId, text );
			else
			{
				if( text != 0 )
					PrepareAchievementUI( -1, text );
			}	

			support::Scores::GetInstance()->AwardAchievement( 100.0f, achievementId );
		}
		else
		{
			if( text != 0 )
			{
				PrepareAchievementUI( achievementId, text );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareAchievementUI
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::PrepareAchievementUI( int achievementId, int textId  )
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Show( achievementId, textId );
}

/////////////////////////////////////////////////////
/// Method: PrepareAchievementUI
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::PrepareAchievementUI( int achievementId, const char* text  )
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Show( achievementId, text );
}

/////////////////////////////////////////////////////
/// Method: UpdateAchievementUI
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::UpdateAchievementUI( float deltaTime )
{
	if( m_AchievementUI != 0 )
		m_AchievementUI->Update( deltaTime );
}

/////////////////////////////////////////////////////
/// Method: DrawAchievementUI
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawAchievementUI()
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Draw();
}

/////////////////////////////////////////////////////
/// Method: HasAchievement
/// Params: None
///
/////////////////////////////////////////////////////
bool GameSystems::HasAchievement( int achievementId )
{
	if( support::Scores::IsInitialised() )
	{
		if( achievementId != -1 )
		{
			return !support::Scores::GetInstance()->IsNewAchievement( achievementId );
		}
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: CreateLevel
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateLevel()
{
	math::Vec3 pos( 0.0f, 0.0f, 0.0f );

	m_Level = new Level;
	DBG_ASSERT( (m_Level != 0) );

	//m_StageData = m_pScriptData->GetActiveStageData();

	m_Level->Initialise();
	//m_Level->LoadPhysicsB2DData( m_StageData.physicsFileB2D, pos, angle, physics::PhysicsWorldB2D::GetWorld() );
	//m_Level->LoadTypes( m_StageData.typesFile, pos, angle );
	//m_Level->PreloadPhysicsPieces();
}

/////////////////////////////////////////////////////
/// Method: DestroyLevel
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyLevel()
{
	if( m_Level != 0 )
	{
		delete m_Level;
		m_Level = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: LoadNewStage
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoadNewStage()
{
	// load the script
	script::LoadScript( m_StageFile );
	script::LuaCallFunction( m_StageSetupFunc, 0, 0 );

	GameSystems::GetInstance()->CreateLevel();
	
	/*m_Player->SetupLevelData( *m_Level );*/
}

/////////////////////////////////////////////////////
/// Method: DrawLevel
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawLevel(bool sortedMeshes)
{
//	if( sortedMeshes )
//		m_Level->DrawSorted();
//	else
//		m_Level->DrawOpaque();
}

/////////////////////////////////////////////////////
/// Method: DrawLevelEmitters
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawLevelEmitters()
{
	//m_Level->DrawEmitters();
}

/////////////////////////////////////////////////////
/// Method: UpdateLevel
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void GameSystems::UpdateLevel(float deltaTime)
{
	//m_Level->Update(deltaTime);
	//m_Level->UpdateAudio( m_Player->GetBoundingSphere(), deltaTime );
}

/////////////////////////////////////////////////////
/// Method: CreateGameCamera
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateGameCamera()
{
	DBG_ASSERT_MSG( (m_Player != 0), "Player needs to exist before the game camera can be created" );

	m_GameCamera = new GameCamera( m_InputSystem, *m_Player );
	DBG_ASSERT_MSG( (m_GameCamera != 0), "Could not create GameCamera class" );

}

/////////////////////////////////////////////////////
/// Method: CreateGameCameraNoPlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateGameCameraNoPlayer()
{
	m_GameCamera = new GameCamera(m_InputSystem);
	DBG_ASSERT_MSG( (m_GameCamera != 0), "Could not create GameCamera class" );

}

/////////////////////////////////////////////////////
/// Method: DestroyGameCamera
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyGameCamera()
{
	if(m_GameCamera != 0)
	{
		delete m_GameCamera;
		m_GameCamera = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PreloadResources
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::PreloadResources()
{

}
		
/////////////////////////////////////////////////////
/// Method: ClearPreloaded
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ClearPreloaded()
{

}

/////////////////////////////////////////////////////
/// Method: DrawDebug
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawDebug()
{
	m_Level->DrawDebug();
}

/////////////////////////////////////////////////////
/// Method: SetCorrectStage
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SetCorrectStage()
{
	//m_StageFile = m_pScriptData->GetActiveStageData().stageName;
	//m_StageSetupFunc = m_pScriptData->GetActiveStageData().stageSetupFunc;
}

/////////////////////////////////////////////////////
/// Method: CreateEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateEffectPool( int maxExplosions, int maxFires )
{
	m_EffectPool = new EffectPool( maxExplosions, maxFires );
	DBG_ASSERT_MSG( (m_EffectPool != 0), "Could not create EffectPool class" );
}
		
/////////////////////////////////////////////////////
/// Method: DestroyEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyEffectPool()
{
	if(m_EffectPool != 0)
	{
		delete m_EffectPool;
		m_EffectPool = 0;
	}
}
		
/////////////////////////////////////////////////////
/// Method: DrawEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawEffectPool()
{
	if(m_EffectPool != 0)
		m_EffectPool->Draw();
}

/////////////////////////////////////////////////////
/// Method: UpdateEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::UpdateEffectPool( float deltaTime )
{
	if(m_EffectPool != 0)
		m_EffectPool->Update(deltaTime);
}

/////////////////////////////////////////////////////
/// Method: ResetEffectPool
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ResetEffectPool( )
{
	if(m_EffectPool != 0)
		m_EffectPool->Reset();
}

/////////////////////////////////////////////////////
/// Method: SpawnExplosion
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void GameSystems::SpawnExplosion( const math::Vec3& pos )
{
	if(m_EffectPool != 0)
		m_EffectPool->SpawnExplosion(pos);
}
		
/////////////////////////////////////////////////////
/// Method: SpawnFire
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void GameSystems::SpawnFire( const math::Vec3& pos )
{
	if(m_EffectPool != 0)
		m_EffectPool->SpawnFire(pos);
}

/////////////////////////////////////////////////////
/// Method: SpawnFire
/// Params: [in]pos
///
/////////////////////////////////////////////////////
void GameSystems::SpawnStars( const math::Vec3& pos )
{
	if(m_EffectPool != 0)
		m_EffectPool->SpawnStars(pos);
}

/////////////////////////////////////////////////////
/// Method: InitAudio
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::InitAudio()
{
	if( m_ObjectDestroyBufferId == snd::INVALID_SOUNDBUFFER )
	{
		m_ObjectDestroyBufferId = AudioSystem::GetInstance()->AddAudioFile( 503/*object_destroy.wav*/ );
	}

	if( m_TargetDestroyBufferId == snd::INVALID_SOUNDBUFFER )
	{
		m_TargetDestroyBufferId = AudioSystem::GetInstance()->AddAudioFile( 504/*target_destroy.wav*/ );
	}

	if( m_RingPassBufferId == snd::INVALID_SOUNDBUFFER )
	{
		m_RingPassBufferId = AudioSystem::GetInstance()->AddAudioFile( 505/*ring_pass.wav*/ );
	}

	if( m_LevelCompleteBufferId == snd::INVALID_SOUNDBUFFER )
	{
		m_LevelCompleteBufferId = AudioSystem::GetInstance()->AddAudioFile( 108/*level_complete.wav*/ );
	}
}

/////////////////////////////////////////////////////
/// Method: PlayObjectDestroyedAudio
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::PlayObjectDestroyedAudio()
{
	if( m_ObjectDestroyBufferId != snd::INVALID_SOUNDBUFFER )
		AudioSystem::GetInstance()->PlayAudio( m_ObjectDestroyBufferId, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 0.5f );
}

/////////////////////////////////////////////////////
/// Method: PlayTargetDestroyedAudio
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::PlayTargetDestroyedAudio()
{
	if( m_TargetDestroyBufferId != snd::INVALID_SOUNDBUFFER )
		AudioSystem::GetInstance()->PlayAudio( m_TargetDestroyBufferId, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 0.5f );
}

/////////////////////////////////////////////////////
/// Method: PlayRingPassAudio
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::PlayRingPassAudio( float pitchExtra )
{
	if( m_RingPassBufferId != snd::INVALID_SOUNDBUFFER )
		AudioSystem::GetInstance()->PlayAudio( m_RingPassBufferId, zeroVec, AL_TRUE, AL_FALSE, 1.0f+pitchExtra, 0.5f );
}

/////////////////////////////////////////////////////
/// Method: PlayLevelCompleteAudio
/// Params: 
///
/////////////////////////////////////////////////////
void GameSystems::PlayLevelCompleteAudio()
{
	if( m_LevelCompleteBufferId != snd::INVALID_SOUNDBUFFER )
		AudioSystem::GetInstance()->PlayAudio( m_LevelCompleteBufferId, zeroVec, AL_TRUE, AL_FALSE, 1.0f, 0.7f );
}
