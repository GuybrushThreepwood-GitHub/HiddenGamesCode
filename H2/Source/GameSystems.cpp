
/*===================================================================
	File: GameSystems.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "SupportBase.h"

#include "Cabby.h"
#include "CabbyConsts.h"

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

	// update some values before use
	math::Vec3 scaleFactor( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_GameData.ACHIEVEMENT_ICON_MAXX	*= scaleFactor.X;
	m_GameData.ACHIEVEMENT_ICON_SPEED	*= scaleFactor.X;

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

	m_GameData.SPEECHBUBBLE_XOFFSET_PHONE_SD		*= scaleFactor.X;
	m_GameData.SPEECHBUBBLE_YOFFSET_PHONE_SD		*= scaleFactor.Y;
	m_GameData.SPEECHBUBBLE_XOFFSET_PHONE_RETINA	*= scaleFactor.X;
	m_GameData.SPEECHBUBBLE_YOFFSET_PHONE_RETINA	*= scaleFactor.Y;
	m_GameData.SPEECHBUBBLE_XOFFSET_TABLET_SD		*= scaleFactor.X;
	m_GameData.SPEECHBUBBLE_YOFFSET_TABLET_SD		*= scaleFactor.Y;

	m_GameData.TOWTRUCK_FLYBY_SPEED		*= scaleFactor.X;

	m_GameData.ICON_HIGHEST_POS			*= scaleFactor.X;
	m_GameData.ICON_LOWEST_POS			*= scaleFactor.Y;
	m_GameData.ICON_EDGE_CLOSEST		*= scaleFactor.X;

	if( m_DevData.hiresCustomers )
	{
		m_GameData.CUSTOMER_MALE_MESH	=	m_GameData.CUSTOMER_MALE_MESH_HI;
		m_GameData.CUSTOMER_FEMALE_MESH	=	m_GameData.CUSTOMER_FEMALE_MESH_HI;
		m_GameData.CUSTOMER_PARCEL_MESH	=	m_GameData.CUSTOMER_PARCEL_MESH_HI;
	}
	else
	{
		m_GameData.CUSTOMER_MALE_MESH	=	m_GameData.CUSTOMER_MALE_MESH_LOW;
		m_GameData.CUSTOMER_FEMALE_MESH	=	m_GameData.CUSTOMER_FEMALE_MESH_LOW;
		m_GameData.CUSTOMER_PARCEL_MESH	=	m_GameData.CUSTOMER_PARCEL_MESH_LOW;
	}

	m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_PHONE_SD		*= scaleFactor.X;
	m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_PHONE_SD		*= scaleFactor.Y;
	m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_PHONE_RETINA	*= scaleFactor.X;
	m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_PHONE_RETINA	*= scaleFactor.Y;
	m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETX_TABLET_SD		*= scaleFactor.X;
	m_GameData.CUSTOMER_SWEAR_BUBBLE_OFFSETY_TABLET_SD		*= scaleFactor.Y;

	m_pScriptData->SetGameData(m_GameData);

	m_Level = 0;
	m_Player = 0;

	m_StageFile = 0;
	m_StageSetupFunc = 0;

	//m_GameCamera = 0;
	m_AchievementUI = 0;
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
/// Method: GetKiipAchievementId
/// Params: [in]achievementString
///
/////////////////////////////////////////////////////
int GameSystems::GetKiipAchievementId( const char* achievementId )
{
	return res::GetKiipAchievementIndex( achievementId );
}

/////////////////////////////////////////////////////
/// Method: GetAchievementString
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
const char* GameSystems::GetKiipAchievementString( int achievementId )
{
	return res::GetKiipAchievementString( achievementId );
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
		// if it's one of the unlocks...could make sure to unlock again
		/*switch( achievementId )
		{
			case 1: // tow truck
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->towTruckUnlocked = true;
				}
			}break;
			case 2: // delivery van
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->deliveryVanUnlocked = true;
				}
				
			}break;
			case 3: // hotrod
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->hotRodUnlocked = true;
				}
				
			}break;
			case 4: // bond lotus
			{
				
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->bondLotusUnlocked = true;
				}
			}break;
			case 5: // team a van
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->aTeamVanUnlocked = true;
				}
				
			}break;
			case 6: // ghoul hunter
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->ghoulHunterUnlocked = true;
				}
				
			}break;
			case 7: // spinner
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->spinnerUnlocked = true;
				}
				
			}break;
			case 8: // ledorean
			{
				if( ProfileManager::IsInitialised() )
				{
					ProfileManager::GetInstance()->GetProfile()->deloreanUnlocked = true;
				}
				
			}break;
				
			default:
				// ignore
				break;
		}
		
		if( ProfileManager::IsInitialised() )
		{
			ProfileManager::GetInstance()->SaveProfile();
		}*/
	
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
/*	b2Vec2 areaMin( -1000.0f, -1000.0f );
	b2Vec2 areaMax(1000.0f, 1000.0f);
	b2Vec2 gravity(0.0f,0.0f);

	physics::PhysicsWorldB2D::Create( areaMin, areaMax, gravity, m_DevData.enablePhysicsDraw );

	physics::PhysicsWorldB2D::GetWorld()->SetContactListener(&m_ContactListener);


	physics::PhysicsWorldODE::Create();*/
}

/////////////////////////////////////////////////////
/// Method: DestroyPhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPhysics()
{
//	physics::PhysicsWorldB2D::Destroy();
//	physics::PhysicsWorldODE::Destroy();
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
void GameSystems::AwardAchievement( int achievementId )
{
	if( support::Scores::IsInitialised() )
	{
		if( support::Scores::GetInstance()->IsNewAchievement( achievementId ) )
			PrepareAchievementUI( achievementId );

		support::Scores::GetInstance()->AwardAchievement( 100.0f, achievementId );
	}
}

/////////////////////////////////////////////////////
/// Method: AwardKiipAchievement
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::AwardKiipAchievement( int achievementId )
{
	if( support::Scores::IsInitialised() )
	{
		//if( support::Scores::GetInstance()->IsNewAchievement( achievementId ) )
		//	PrepareAchievementUI( achievementId );

		support::Scores::GetInstance()->AwardKiipAchievement( 100.0f, achievementId );
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareAchievementUI
/// Params: [in]achievementId
///
/////////////////////////////////////////////////////
void GameSystems::PrepareAchievementUI( int achievementId )
{
	if( m_AchievementUI != 0)
		m_AchievementUI->Show( achievementId );
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
/// Method: CreateGameCamera
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateGameCamera()
{
	DBG_ASSERT_MSG( (m_Player != 0), "Player needs to exist before the game camera can be created" );

	m_GameCamera = new GameCamera( *m_Player );
	DBG_ASSERT_MSG( (m_GameCamera != 0), "Could not create GameCamera class" );

}

/////////////////////////////////////////////////////
/// Method: CreateGameCameraNoPlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateGameCameraNoPlayer()
{
	m_GameCamera = new GameCamera();
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

