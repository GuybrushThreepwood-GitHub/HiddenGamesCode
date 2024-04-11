
/*===================================================================
	File: GameSystems.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"

#include "H4.h"
#include "AppConsts.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"
#include "Effects/Emitter.h"

#include "ScriptAccess/ScriptAccess.h"
#include "ScriptAccess/Sound/SoundAccess.h"
#include "ScriptAccess/H4/Room/RoomAccess.h"
#include "ScriptAccess/H4/Inventory/InventoryAccess.h"

#include "GameObjects/Script/ScriptBaseObject.h"

#include "Resources/StringResources.h"
#include "Resources/AchievementList.h"

#include "MicroGame/MicroGameID.h"
#include "MicroGame/MicroGameKeypad.h"
#include "MicroGame/MicroGameSliderHold.h"
#include "MicroGame/MicroGameLiftButtons.h"
#include "MicroGame/MicroGameWiring.h"
#include "MicroGame/MicroGameDials.h"
#include "MicroGame/MicroGamePickupItem.h"
#include "MicroGame/MicroGameCardSwipe.h"
#include "MicroGame/MicroGameThumbScan.h"
#include "MicroGame/MicroGameKeylock.h"
#include "MicroGame/MicroGameCriminalDocs.h"
#include "MicroGame/MicroGameWaterTightDoor.h"
#include "MicroGame/MicroGameMap.h"
#include "MicroGame/MicroGameInventory.h"
#include "MicroGame/MicroGameUseItem.h"

#include "GameStates/UI/AchievementUI.h"

#include "GameSystems.h"

GameSystems* GameSystems::ms_Instance = 0;

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
	m_CameraData = m_pScriptData->GetCameraData();
	m_GameData = m_pScriptData->GetGameData();

	// update some values before use
	math::Vec3 scaleFactor( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_GameData.ACHIEVEMENT_ICON_MAXX	*= scaleFactor.X;
	m_GameData.ACHIEVEMENT_ICON_SPEED	*= scaleFactor.X;

	m_pScriptData->SetGameData(m_GameData);

	m_Level = 0;
	m_Player = 0;
	m_NavMesh = 0;

	m_GameCamera = 0;
	m_EnemyManagement = 0;
	m_InventoryManagement = 0;
	m_DecalSystem = 0;

	m_IsTalking = false;
	m_SayLine1 = -1;
	m_SayLine2 = -1;
	m_SayFuncCall = 0;

	m_MicroGame = 0;
	m_InMicroGame = false;
	m_SuccessMicroGameFunc = 0;

	m_StageChange = false;
	m_StageFile = 0;
	m_StageSetupFunc = 0;
	m_LevelName = 0;
	m_PlayerSpawnId = -1;

	m_EndGameState = false;

	m_ScriptedSequencePlayer = 0;

	RegisterBaseObjectFunctions();
	RegisterRoomFunctions();
	RegisterSoundFunctions();
	RegisterInventoryFunctions();

	// default options
	m_CompleteSaveData.optionsData.leftHanded = false;
	m_CompleteSaveData.optionsData.controlType = 1;
	m_CompleteSaveData.optionsData.lowPowerMode = false;

	m_CompleteSaveData.optionsData.camLerp = true;
	m_CompleteSaveData.optionsData.noiseFilter = 2;
	m_CompleteSaveData.optionsData.fpsMode = false;

	m_AchievementUI = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameSystems::~GameSystems()
{
	delete m_MicroGame;

	// clear the registered variables
	delete[] m_RegisteredVariables.pItems;

	delete[] m_GameSaveData.dataBlock.pItems;

	if( m_GameSaveData.pInventoryList != 0 )
	{
		delete[] m_GameSaveData.pInventoryList;
		m_GameSaveData.pInventoryList = 0;
	}

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
	int i=0;

	m_Player = new Player( m_InputSystem );
	DBG_ASSERT_MSG( (m_Player != 0), "Could not create player" );

	m_Player->Setup();

	m_InventoryManagement = new InventoryManagement();
	DBG_ASSERT_MSG( (m_InventoryManagement != 0), "Could not create inventory" );

	LoadGeneralData();
	
	// inmate clothing (always there)
	m_InventoryManagement->AddItem(7000);

	// setup the other clothing if it was already unlocked
	GeneralData& generalData = GetGeneralData();
	if( generalData.costume2Unlocked == 1 )
		m_InventoryManagement->AddItem(7001);
	if( generalData.costume3Unlocked == 1 )
		m_InventoryManagement->AddItem(7002);
	if( generalData.costume4Unlocked == 1 )
		m_InventoryManagement->AddItem(7003);
	if( generalData.costume5Unlocked == 1 )
		m_InventoryManagement->AddItem(7004);
	if( generalData.costume6Unlocked == 1 )
		m_InventoryManagement->AddItem(7005);
	if( generalData.costume7Unlocked == 1 )
		m_InventoryManagement->AddItem(7006);
	if( generalData.costume8Unlocked == 1 )
		m_InventoryManagement->AddItem(7007);
	if( generalData.costume9Unlocked == 1 )
		m_InventoryManagement->AddItem(7008);

#ifdef DEVELOPER_SETUP
	script::LuaCallFunction( "DeveloperSave", 0, 0 );
#endif // DEVELOPER_SETUP

	// set up the players data from the save file
	m_Player->SetPistolBulletMagCount( m_GameSaveData.pistolAmmo );
	m_Player->SetPistolBulletTotal( m_GameSaveData.pistolAmmoTotal  );
	m_Player->SetShotgunBulletMagCount( m_GameSaveData.shotgunAmmo );
	m_Player->SetShotgunBulletTotal( m_GameSaveData.shotgunAmmoTotal );
	m_Player->SetHealth( m_GameSaveData.playerHealth );

	m_Player->SetWeaponIndex( m_GameSaveData.weaponIndex );
	m_Player->SetClothingIndex( m_GameSaveData.clothingIndex );

	// add saved inventory items
	for( i=0; i < m_GameSaveData.inventoryTotalItems; ++i )
	{
		m_InventoryManagement->AddItem( m_GameSaveData.pInventoryList[i] );
	}
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

	if( m_InventoryManagement != 0 )
	{
		delete m_InventoryManagement;
		m_InventoryManagement = 0;
	}
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
/// Method: AddItem
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::AddItem( int index )
{
	return m_InventoryManagement->AddItem( index );
}

/////////////////////////////////////////////////////
/// Method: HasItem
/// Params: None
///
/////////////////////////////////////////////////////
bool GameSystems::HasItem( int index )
{
	return m_InventoryManagement->HasItem( index );
}

/////////////////////////////////////////////////////
/// Method: RemoveItem
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::RemoveItem( int index )
{
	return m_InventoryManagement->RemoveItem( index );
}

/////////////////////////////////////////////////////
/// Method: RemoveItem
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::Say( int index1, int index2, const char* callFunc )
{
	m_SayLine1 = index1;
	m_SayLine2 = index2;

	m_SayFuncCall = callFunc;
}

/////////////////////////////////////////////////////
/// Method: RemoveItem
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::GetSay( int* index1, int* index2, const char** callFunc )
{
	//DBG_ASSERT( (index1 != 0) );
	//DBG_ASSERT( (index2 != 0) );

	if( index1 != 0 )
		*index1 = m_SayLine1;
	if( index2 != 0 )
	{
		*index2 = m_SayLine2;
		*callFunc = m_SayFuncCall;
	}
}

/////////////////////////////////////////////////////
/// Method: ClearText
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ClearText()
{
	m_SayLine1 = -1;
	m_SayLine2 = -1;
	m_SayFuncCall = 0;
}

/////////////////////////////////////////////////////
/// Method: SetTalkState
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SetTalkState( bool state )
{
	m_IsTalking = state;
}

/////////////////////////////////////////////////////
/// Method: IsTalking
/// Params: None
///
/////////////////////////////////////////////////////
bool GameSystems::IsTalking()
{
	return( m_IsTalking );
}

/////////////////////////////////////////////////////
/// Method: CreatePhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreatePhysics()
{
	b2Vec2 areaMin( -1000.0f, -1000.0f );
	b2Vec2 areaMax(1000.0f, 1000.0f);
	b2Vec2 gravity(0.0f,0.0f);

	physics::PhysicsWorldB2D::Create( areaMin, areaMax, gravity, m_DevData.enablePhysicsDraw );

	physics::PhysicsWorldB2D::GetWorld()->SetContactListener(&m_ContactListener);
}

/////////////////////////////////////////////////////
/// Method: DestroyPhysics
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyPhysics()
{
	physics::PhysicsWorldB2D::Destroy();
}

/////////////////////////////////////////////////////
/// Method: CreateLevel
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateLevel( int playerSpawnId )
{
	m_StageData = m_pScriptData->GetActiveStageData();

	if( m_NavMesh == 0 )
	{
		m_NavMesh = new NavigationMesh;
		DBG_ASSERT_MSG( (m_NavMesh != 0), "Could not create level navmesh" );
	}

	math::Vec3 pos( 0.0f, 0.0f, 0.0f );
	float angle = 0.0f;

	m_Level = new Level;
	DBG_ASSERT( (m_Level != 0) );

	m_Level->Initialise();
	m_Level->LoadMeshData( m_StageData.gameLayer, m_StageData.magFilter, m_StageData.minFilter );
	m_Level->LoadPhysicsB2DData( m_StageData.physicsFileB2D, pos, angle, physics::PhysicsWorldB2D::GetWorld() );
	if( m_StageData.navMeshFile )
		m_NavMesh->CreateFromFile( m_StageData.navMeshFile );
	m_Level->LoadTypes( m_StageData.typesFile, false, 0, pos, angle );
	if( m_StageData.skyboxIndex != -1 )
		m_Level->LoadSkybox( m_StageData.skyboxIndex );
	m_Level->SetExtendedRoomLimits( m_StageData.roomMaxEmpty, m_StageData.roomMaxAmmo, m_StageData.roomMaxHealth, m_StageData.roomMaxZombie );

	m_Level->SetPlayerSpawnId( playerSpawnId );

	if( m_StageData.roomSetupFunc )
	{
		int result = 0;
		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		lua_getglobal( script::LuaScripting::GetState(), m_StageData.roomSetupFunc );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "*ERROR* Calling function '%s' failed\n", m_StageData.roomSetupFunc );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

			DBG_ASSERT_MSG( 0, "*ERROR* Calling function '%s' failed", m_StageData.roomSetupFunc );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CallLevelOnCreate
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CallLevelOnCreate()
{
	m_Level->CallOnCreate();
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

	if( m_NavMesh != 0 )
	{
		delete m_NavMesh;
		m_NavMesh = 0;
	}
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
/// Method: CreateEnemyManager
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateEnemyManager()
{
	m_EnemyManagement = new EnemyManagement( *m_Player );
	DBG_ASSERT_MSG( (m_EnemyManagement != 0), "Could not create enemy manager class" );
}

/////////////////////////////////////////////////////
/// Method: DestroyEnemyManager
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyEnemyManager()
{
	if(m_EnemyManagement != 0)
	{
		delete m_EnemyManagement;
		m_EnemyManagement = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: CreateDecalSystem
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::CreateDecalSystem()
{
	m_DecalSystem = new DecalSystem();
	DBG_ASSERT_MSG( (m_DecalSystem != 0), "Could not create decal system class" );
}

/////////////////////////////////////////////////////
/// Method: DestroyDecalSystem
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyDecalSystem()
{
	if(m_DecalSystem != 0)
	{
		delete m_DecalSystem;
		m_DecalSystem = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: DrawLevel
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawLevel(bool sortedMeshes)
{
	if( sortedMeshes )
		m_Level->DrawSorted();
	else
		m_Level->DrawOpaque();
}

/////////////////////////////////////////////////////
/// Method: DrawLevelSkybox
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawLevelSkybox()
{
	if( m_StageData.skyboxIndex != -1 )
	{
		if( gDebugCamera.IsEnabled() )
		{
			math::Vec3 pos = gDebugCamera.GetPosition();

			renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
			glPushMatrix();
				glTranslatef( pos.X, pos.Y, pos.Z );
				m_Level->DrawSkybox();
			glPopMatrix();
			renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
		}
		else
		{
			math::Vec3 playerPos = m_GameCamera->GetPosition();

			renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
			glPushMatrix();
				glTranslatef( playerPos.X, playerPos.Y, playerPos.Z );
				m_Level->DrawSkybox();
			glPopMatrix();
			renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: DrawLevelEmitters
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DrawLevelEmitters()
{
	m_Level->DrawEmitters();
}

/////////////////////////////////////////////////////
/// Method: UpdateLevel
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void GameSystems::UpdateLevel(float deltaTime)
{
	m_Level->Update(deltaTime);
	m_Level->UpdateAudio( m_Player->GetBoundingSphere(), deltaTime );
}

/////////////////////////////////////////////////////
/// Method: FindObject
/// Params: None
///
/////////////////////////////////////////////////////
BaseObject* GameSystems::FindObject( int id )
{
	const std::vector<BaseObject *>& objList = m_Level->GetGameObjectList();
	std::vector<BaseObject *>::const_iterator it = objList.begin();

	while( it != objList.end() )
	{
		if( (*it)->GetId() == id )
		{
			return( (*it) );
		}
		it++;
	}

	return(0);
}

/////////////////////////////////////////////////////
/// Method: SetCorrectStage
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SetCorrectStage()
{
	m_StageFile = m_pScriptData->GetActiveStageData().stageName;
	m_StageSetupFunc = m_pScriptData->GetActiveStageData().stageSetupFunc;
	m_LevelName = res::GetScriptString(0);
	m_PlayerSpawnId = -1;
}

/////////////////////////////////////////////////////
/// Method: ContinueGame
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ContinueGame()
{
	ChangeStage( m_StageFile, m_StageSetupFunc, m_LevelName, m_GameSaveData.lastStageEntryDescriptionId, m_PlayerSpawnId );

	// full ammo/health for player
	m_GameSaveData.pistolAmmo = 12;
	m_GameSaveData.pistolAmmoTotal = 99;

	m_GameSaveData.shotgunAmmo = 6;
	m_GameSaveData.shotgunAmmoTotal = 99;

	m_GameSaveData.playerHealth = 100;

	m_Player->SetPistolBulletMagCount( m_GameSaveData.pistolAmmo );
	m_Player->SetPistolBulletTotal( m_GameSaveData.pistolAmmoTotal  );
	m_Player->SetShotgunBulletMagCount( m_GameSaveData.shotgunAmmo );
	m_Player->SetShotgunBulletTotal( m_GameSaveData.shotgunAmmoTotal );
	m_Player->SetHealth( m_GameSaveData.playerHealth );
}

/////////////////////////////////////////////////////
/// Method: ChangeStage
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ChangeStage( const char* stageName, const char* stageSetupFunc, const char* levelName, int descriptionIndex, int playerSpawnId )
{
	m_StageFile = stageName;
	m_StageSetupFunc = stageSetupFunc;
	m_LevelName = levelName;
	m_PlayerSpawnId = playerSpawnId;

	// update the save data
	if( stageName != 0 )
		snprintf( m_GameSaveData.lastStageEntry, MAX_VARIABLE_NAME, "%s", stageName );

	m_GameSaveData.lastStageEntryDescriptionId = descriptionIndex;
	m_GameSaveData.lastStagePosRotId = m_PlayerSpawnId;

	// save the game data
	if( m_DevData.enableSave )
		SaveGameData();

	m_StageChange = true;
}

/////////////////////////////////////////////////////
/// Method: DestroyStage
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyStage()
{
	GameSystems::GetInstance()->DestroyLevel();

	m_DecalSystem->ClearDecals();
	m_Player->ClearBullets();

	//m_ScriptedSequencePlayer->ClearLevelData();

	m_EnemyManagement->ClearLevelData();
	m_Player->ClearLevelData();
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

	GameSystems::GetInstance()->CreateLevel( m_PlayerSpawnId );
	
	m_Player->SetupLevelData( *m_Level, m_NavMesh );
	m_Player->SetDeadState( false );
	m_Player->SetTorchState( m_StageData.enableFlashlight );

	int clothingIndex = m_Player->GetClothingIndex();

	// set correct texture
	m_Player->SetClothingIndex(clothingIndex);

	m_EnemyManagement->SetupLevelData( *m_Level, m_NavMesh );
}

/////////////////////////////////////////////////////
/// Method: LaunchMicroGame
/// Params: [in]microGame
///
/////////////////////////////////////////////////////
void GameSystems::LaunchMicroGame( int microGame, lua_State* pState )
{
	DBG_ASSERT( ((m_MicroGame == 0) && (m_InMicroGame == false)) );

	switch( microGame )
	{
		case MICROGAME_KEYPAD:
		{
			// number and func
			int numeric = static_cast<int>( lua_tonumber( pState, 2 ) );
			const char* successFunc = lua_tostring( pState, 3 );

			m_MicroGame = new MicroGameKeypad( numeric );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate keypad microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_SLIDERHOLD:
		{
			// number and func
			int numeric1 = static_cast<int>( lua_tonumber( pState, 2 ) );
			int numeric2 = static_cast<int>( lua_tonumber( pState, 3 ) );
			int numeric3 = static_cast<int>( lua_tonumber( pState, 4 ) );

			const char* successFunc = lua_tostring( pState, 5 );

			m_MicroGame = new MicroGameSliderHold( numeric1, numeric2, numeric3 );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate sliderhold microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_LIFTBUTTONS:
		{
			int currentFloor = static_cast<int>( lua_tonumber( pState, 2 ) );

			bool button1 = static_cast<bool>( lua_toboolean( pState, 3 ) != 0 );
			const char* button1Label = lua_tostring( pState, 4 );
			const char* button1Func = lua_tostring( pState, 5 );

			bool button2 = static_cast<bool>( lua_toboolean( pState, 6 ) != 0 );
			const char* button2Label = lua_tostring( pState, 7 );
			const char* button2Func = lua_tostring( pState, 8 );

			bool button3 = static_cast<bool>( lua_toboolean( pState, 9 ) != 0 );
			const char* button3Label = lua_tostring( pState, 10 );
			const char* button3Func = lua_tostring( pState, 11 );


			m_MicroGame = new MicroGameLiftButtons( currentFloor, button1, button1Label, button1Func, button2, button2Label, button2Func, button3, button3Label, button3Func );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate lift buttons microgame memory" );

			m_SuccessMicroGameFunc = 0;

			m_InMicroGame = true;
		}break;

		case MICROGAME_WIRING:
		{
			int wire1 = static_cast<int>( lua_tointeger( pState, 2 ) );
			int wire2 = static_cast<int>( lua_tointeger( pState, 3 ) );

			const char* successFunc = lua_tostring( pState, 4 );

			m_MicroGame = new MicroGameWiring( wire1, wire2 );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate wiring microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_DIALS:
		{
			int dial1 = static_cast<int>( lua_tointeger( pState, 2 ) );
			int dial2 = static_cast<int>( lua_tointeger( pState, 3 ) );
			int dial3 = static_cast<int>( lua_tointeger( pState, 4 ) );

			const char* successFunc = lua_tostring( pState, 5 );

			m_MicroGame = new MicroGameDials( dial1, dial2, dial3 );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate dials microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_PICKUPITEM:
		{
			math::Vec3 camPos, camLookAt;

			camPos.X = static_cast<float>( lua_tonumber( pState, 2 ) );
			camPos.Y = static_cast<float>( lua_tonumber( pState, 3 ) );
			camPos.Z = static_cast<float>( lua_tonumber( pState, 4 ) );

			camLookAt.X = static_cast<float>( lua_tonumber( pState, 5 ) );
			camLookAt.Y = static_cast<float>( lua_tonumber( pState, 6 ) );
			camLookAt.Z = static_cast<float>( lua_tonumber( pState, 7 ) );

			float maxYRot = static_cast<float>( lua_tonumber( pState, 8 ) );
			float maxXRot = static_cast<float>( lua_tonumber( pState, 9 ) );

			float minZoom = static_cast<float>( lua_tonumber( pState, 10 ) );
			float maxZoom = static_cast<float>( lua_tonumber( pState, 11 ) );

			int itemId = static_cast<int>( lua_tointeger( pState, 12 ) );
			int uiModelId = static_cast<int>( lua_tointeger( pState, 13 ) );

			int description1Index = static_cast<int>( lua_tointeger( pState, 14 ) );
			int description2Index = static_cast<int>( lua_tointeger( pState, 15 ) );

			const char* successFunc = lua_tostring( pState, 16 );

			m_MicroGame = new MicroGamePickupItem( camPos, camLookAt, maxYRot, maxXRot, minZoom, maxZoom, itemId, uiModelId, description1Index, description2Index );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate pickup item microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_CARDSWIPE:
		{
			int cardIndex = static_cast<int>( lua_tonumber( pState, 2 ) );
			const char* successFunc = lua_tostring( pState, 3 );

			m_MicroGame = new MicroGameCardSwipe( cardIndex );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate card swipe microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_THUMBSCAN:
		{
			const char* successFunc = lua_tostring( pState, 2 );

			m_MicroGame = new MicroGameThumbScan( );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate thumb scan microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_KEYLOCK:
		{
			int keyIndex = static_cast<int>( lua_tonumber( pState, 2 ) );
			const char* successFunc = lua_tostring( pState, 3 );

			m_MicroGame = new MicroGameKeylock( keyIndex );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate keylock microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_CRIMINALDOCS:
		{
			int whichDoc = static_cast<int>( lua_tonumber( pState, 2 ) );
			const char* successFunc = 0;

			m_MicroGame = new MicroGameCriminalDocs( whichDoc );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate criminal docs microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;
		}break;

		case MICROGAME_WATERTIGHTDOOR:
		{
			const char* successFunc = lua_tostring( pState, 2 );

			m_MicroGame = new MicroGameWaterTightDoor( );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate water tight door microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;

		}break;

		case MICROGAME_MAP:
		{
			const char* successFunc = 0;

			int mapIndex = static_cast<int>( lua_tonumber( pState, 2 ) );
			float playerPosX = static_cast<float>( lua_tonumber( pState, 3 ) );
			float playerPosY = static_cast<float>( lua_tonumber( pState, 4 ) );
			float playerRot = static_cast<float>( lua_tonumber( pState, 5 ) );

			m_MicroGame = new MicroGameMap( mapIndex, playerPosX, playerPosY, playerRot );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate map microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;

		}break;

		case MICROGAME_INVENTORY:
		{
			const char* successFunc = 0;

			m_MicroGame = new MicroGameInventory( );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate inventory microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;

		}break;

		case MICROGAME_USEITEM:
		{
			int correctSlot = static_cast<int>( lua_tonumber( pState, 2 ) );
			const char* successFunc = lua_tostring( pState, 3 );
			const char* giveupFunc = lua_tostring( pState, 4 );

			m_MicroGame = new MicroGameUseItem( correctSlot, giveupFunc );
			DBG_ASSERT_MSG( (m_MicroGame != 0), "*ERROR* Could not allocate useitem microgame memory" );

			m_SuccessMicroGameFunc = successFunc;

			m_InMicroGame = true;

		}break;

		default:
			DBG_ASSERT_MSG( 0, "*ERROR* Invalid microgame id" );
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: EndMicroGame
/// Params: [in]result
///
/////////////////////////////////////////////////////
void GameSystems::EndMicroGame( int result )
{
	DBG_ASSERT( ((m_MicroGame != 0) && (m_InMicroGame == true)) );

	bool returnCode = m_MicroGame->ReturnCode();

	delete m_MicroGame;
	m_MicroGame = 0;

	m_InMicroGame = false;

	if( !core::IsEmptyString(m_SuccessMicroGameFunc) )
	{
		if( returnCode )
		{
			int result = 0;
			int errorFuncIndex;
			errorFuncIndex = script::GetErrorFuncIndex();

			const char* microGameSuccess = m_SuccessMicroGameFunc;
			m_SuccessMicroGameFunc = 0;

			lua_getglobal( script::LuaScripting::GetState(), microGameSuccess );

			result = lua_pcall( script::LuaScripting::GetState(), 0, 0, errorFuncIndex );

			// LUA_ERRRUN --- a runtime error. 
			// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
			// LUA_ERRERR --- error while running the error handler function. 

			if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
			{
				DBGLOG( "*ERROR* Calling function '%s' failed\n", microGameSuccess );
				DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );

				DBG_ASSERT_MSG( 0, "*ERROR* Calling function '%s' failed", microGameSuccess );
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: CreateScriptedSequencePlayer
/// Params: None
///
/////////////////////////////////////////////////////		
void GameSystems::CreateScriptedSequencePlayer()
{
	m_ScriptedSequencePlayer = new ScriptedSequencePlayer();
	DBG_ASSERT_MSG( (m_ScriptedSequencePlayer != 0), "Could not create scripted sequence player class" );
}

/////////////////////////////////////////////////////
/// Method: DestroyScriptedSequencePlayer
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::DestroyScriptedSequencePlayer()
{
	if(m_ScriptedSequencePlayer != 0)
	{
		delete m_ScriptedSequencePlayer;
		m_ScriptedSequencePlayer = 0;
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
/// Method: LoadOptions
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoadOptions()
{
	file::TFileHandle fileHandle;
	
	char optionsPath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &optionsPath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( optionsPath, core::MAX_PATH+core::MAX_PATH, "%s/options.sav", core::app::GetSavePath() );

	if( file::FileExists( optionsPath ) )
	{
		if( file::FileOpen( optionsPath, file::FILETYPE_BINARY_READ, &fileHandle ) )
		{
			file::FileRead( &m_CompleteSaveData.optionsData, sizeof(OptionsData), &fileHandle );

			// removed other control type
			m_CompleteSaveData.optionsData.controlType = 1;

			file::FileClose( &fileHandle );
		}
	}
	
	// set the power mode flag early
	core::app::SetLowPowerMode( m_CompleteSaveData.optionsData.lowPowerMode );
}

/////////////////////////////////////////////////////
/// Method: SaveOptions
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SaveOptions()
{
	file::TFileHandle fileHandle;
	
	char optionsPath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &optionsPath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( optionsPath, core::MAX_PATH+core::MAX_PATH, "%s/options.sav", core::app::GetSavePath() );

	if( file::FileOpen( optionsPath, file::FILETYPE_BINARY_WRITE, &fileHandle ) )
	{
		// removed other control type
		m_CompleteSaveData.optionsData.controlType = 1;

		file::FileWrite( &m_CompleteSaveData.optionsData, sizeof(OptionsData), &fileHandle );

		file::FileClose( &fileHandle );
	}
}

/////////////////////////////////////////////////////
/// Method: LoadBestRank
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoadBestRank()
{
	file::TFileHandle fileHandle;
	
	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/bestrank.sav", core::app::GetSavePath() );

	if( file::FileExists( savePath ) )
	{
		if( file::FileOpen( savePath, file::FILETYPE_BINARY_READ, &fileHandle ) )
		{
			file::FileRead( &m_CompleteSaveData.bestRankData, sizeof(BestRankData), &fileHandle );

			file::FileClose( &fileHandle );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SaveBestRank
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SaveBestRank()
{
	file::TFileHandle fileHandle;
	
	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/bestrank.sav", core::app::GetSavePath() );

	if( file::FileOpen( savePath, file::FILETYPE_BINARY_WRITE, &fileHandle ) )
	{
		file::FileWrite( &m_CompleteSaveData.bestRankData, sizeof(BestRankData), &fileHandle );

		file::FileClose( &fileHandle );
	}
}

/////////////////////////////////////////////////////
/// Method: LoadGeneralData
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::LoadGeneralData()
{
	file::TFileHandle fileHandle;
	
	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/general.sav", core::app::GetSavePath() );

	if( file::FileExists( savePath ) )
	{
		if( file::FileOpen( savePath, file::FILETYPE_BINARY_READ, &fileHandle ) )
		{
			file::FileRead( &m_CompleteSaveData.generalData, sizeof(GeneralData), &fileHandle );

			file::FileClose( &fileHandle );
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SaveGeneralData
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SaveGeneralData()
{
	file::TFileHandle fileHandle;
	
	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/general.sav", core::app::GetSavePath() );

	if( file::FileOpen( savePath, file::FILETYPE_BINARY_WRITE, &fileHandle ) )
	{
		file::FileWrite( &m_CompleteSaveData.generalData, sizeof(GeneralData), &fileHandle );

		file::FileClose( &fileHandle );
	}
}

/////////////////////////////////////////////////////
/// Method: RegisterSaveVariables
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::RegisterSaveVariables()
{
	int itemOffset = 0;
	std::memset( &m_RegisteredVariables, 0, sizeof(ItemDataBlock) );


#ifdef _DEBUG
	if( script::LuaFunctionCheck( "RegisterSaveData" ) == 0 )
#endif // _DEBUG
	{
		int result = 0;

		int errorFuncIndex;
		errorFuncIndex = script::GetErrorFuncIndex();

		// get the function
		lua_getglobal( script::LuaScripting::GetState(), "RegisterSaveData" );

		result = lua_pcall( script::LuaScripting::GetState(), 0, 1, errorFuncIndex );

		// LUA_ERRRUN --- a runtime error. 
		// LUA_ERRMEM --- memory allocation error. For such errors, Lua does not call the error handler function. 
		// LUA_ERRERR --- error while running the error handler function. 

		if( result == LUA_ERRRUN || result == LUA_ERRMEM || result == LUA_ERRERR )
		{
			DBGLOG( "RESOURCES: *ERROR* Calling function '%s' failed\n", "RegisterAnimatedResources" );
			DBGLOG( "\tLUA_TRACEBACK: %s\n", lua_tostring( script::LuaScripting::GetState(), -1 ) );
			
			DBG_ASSERT_MSG( 0, "RESOURCES: *ERROR* Calling function '%s' failed", "RegisterAnimatedResources" );
			return;
		}

		// should be a table of tables
		if( lua_istable( script::LuaScripting::GetState(), -1 ) )
		{
			int i=0;
			int n = luaL_len( script::LuaScripting::GetState(), -1 );

			// allocate space for the variables
			m_RegisteredVariables.itemCount = n;
			m_RegisteredVariables.pItems = new ItemData[n];

			// go through all the tables in this table
			for( i = 1; i <= n; ++i )
			{
				lua_rawgeti( script::LuaScripting::GetState(), -1, i );
				if( lua_istable( script::LuaScripting::GetState(), -1 ) )
				{
					int innerCount = luaL_len( script::LuaScripting::GetState(), -1 );
					if( innerCount == 2 )
					{
						// variable name
						lua_rawgeti( script::LuaScripting::GetState(), -1, 1 );
						const char* varName = lua_tostring( script::LuaScripting::GetState(), -1 );

						lua_pop( script::LuaScripting::GetState(), 1 );

						// default value
						lua_rawgeti( script::LuaScripting::GetState(), -1, 2 );
						int varValue = static_cast<int>( lua_tonumber( script::LuaScripting::GetState(), -1 ) );

						lua_pop( script::LuaScripting::GetState(), 1 );

						snprintf( m_RegisteredVariables.pItems[itemOffset].name, MAX_VARIABLE_NAME, "%s", varName );
						m_RegisteredVariables.pItems[itemOffset].value = varValue;

						itemOffset++;
					}
				}
				lua_pop( script::LuaScripting::GetState(), 1 );	
			}
			lua_pop( script::LuaScripting::GetState(), 1 );
		}
	}

	// store the final size for comparison
	m_RegisteredVariables.itemDataSize = (m_RegisteredVariables.itemCount * sizeof(ItemData));
}

/////////////////////////////////////////////////////
/// Method: SetDataValue
/// Params: [in]varName, [in]varValue
///
/////////////////////////////////////////////////////			
void GameSystems::SetDataValue( const char* varName, int varValue )
{
	int i=0;
	for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
	{
		if( std::strcmp( varName, m_GameSaveData.dataBlock.pItems[i].name ) == 0 )
		{
			m_GameSaveData.dataBlock.pItems[i].value = varValue;
			return;
		}
	}

	DBG_ASSERT_MSG( 0, "ERROR: save data variable '%s' not found", varName );
}
		
/////////////////////////////////////////////////////
/// Method: GetDataValue
/// Params: [in]varName
///
/////////////////////////////////////////////////////	
int GameSystems::GetDataValue( const char* varName )
{
	int i=0;
	for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
	{
		if( std::strcmp( varName, m_GameSaveData.dataBlock.pItems[i].name ) == 0 )
			return( m_GameSaveData.dataBlock.pItems[i].value );
	}

	DBG_ASSERT_MSG( 0, "ERROR: save data variable '%s' not found", varName );
	return(0);
}

/////////////////////////////////////////////////////
/// Method: ClearDataValues
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::ClearDataValues()
{
	int i=0;
	for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
		m_GameSaveData.dataBlock.pItems[i].value = 0;
}

/////////////////////////////////////////////////////
/// Method: LoadGameData
/// Params: None
///
/////////////////////////////////////////////////////		
void GameSystems::LoadGameData()
{
	int i=0;
	file::TFileHandle fileHandle;
	
	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/game.sav", core::app::GetSavePath() );

	if( file::FileExists( savePath ) )
	{
		if( file::FileOpen( savePath, file::FILETYPE_BINARY_READ, &fileHandle ) )
		{
			file::FileReadInt( &m_GameSaveData.dataBlock.itemDataSize, sizeof(int), 1, &fileHandle );

			// mismatch sizes
			if( m_GameSaveData.dataBlock.itemDataSize != m_RegisteredVariables.itemDataSize )
			{
				DBGLOG( "*WARNING* Save file was found, but had mismatched data size\n" );

				if( m_GameSaveData.dataBlock.pItems != 0 )
				{
					delete[] m_GameSaveData.dataBlock.pItems;
					m_GameSaveData.dataBlock.pItems = 0;
				}

				// copy the registered data into the game save
				m_GameSaveData.dataBlock.itemDataSize = m_RegisteredVariables.itemDataSize;
				m_GameSaveData.dataBlock.itemCount = m_RegisteredVariables.itemCount;

				m_GameSaveData.dataBlock.pItems = new ItemData[m_RegisteredVariables.itemCount];

				for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
				{
					snprintf( m_GameSaveData.dataBlock.pItems[i].name, MAX_VARIABLE_NAME, "%s", m_RegisteredVariables.pItems[i].name );
					m_GameSaveData.dataBlock.pItems[i].value = m_RegisteredVariables.pItems[i].value;
				}

				file::FileClose( &fileHandle );
			}
			else
			{
				// read in registered variables
				file::FileReadInt( &m_GameSaveData.dataBlock.itemCount, sizeof(int), 1, &fileHandle );
				if( m_GameSaveData.dataBlock.pItems != 0 )
				{
					delete[] m_GameSaveData.dataBlock.pItems;
					m_GameSaveData.dataBlock.pItems = 0;
				}
				m_GameSaveData.dataBlock.pItems = new ItemData[m_GameSaveData.dataBlock.itemCount];

				for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
				{
					file::FileReadChar( &m_GameSaveData.dataBlock.pItems[i].name, sizeof(char), MAX_VARIABLE_NAME, &fileHandle );
					file::FileReadInt( &m_GameSaveData.dataBlock.pItems[i].value, sizeof(int), 1, &fileHandle );
				}

				// load normal game data
				file::FileReadFloat( &m_GameSaveData.timePlayed,		sizeof(float), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.numKills,			sizeof(int), 1, &fileHandle );

				file::FileReadInt( &m_GameSaveData.weaponIndex,			sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.clothingIndex,		sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.playerHealth,		sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.pistolAmmo,			sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.pistolAmmoTotal,		sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.shotgunAmmo,			sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.shotgunAmmoTotal,	sizeof(int), 1, &fileHandle );

				// load the inventory
				file::FileReadInt( &m_GameSaveData.inventoryTotalItems, sizeof(int), 1, &fileHandle );

				if( m_GameSaveData.pInventoryList != 0 )
				{
					delete[] m_GameSaveData.pInventoryList;
					m_GameSaveData.pInventoryList = 0;
				}

				m_GameSaveData.pInventoryList = new int[m_GameSaveData.inventoryTotalItems];

				for( i=0; i < m_GameSaveData.inventoryTotalItems; ++i )
				{
					int itemId = 0;
					file::FileReadInt( &itemId,	sizeof(int), 1, &fileHandle );

					m_GameSaveData.pInventoryList[i] = itemId;
				}

				file::FileReadChar( &m_GameSaveData.lastStageEntry,		sizeof(char), MAX_VARIABLE_NAME, &fileHandle );
				file::FileReadInt( &m_GameSaveData.lastStageEntryDescriptionId, sizeof(int), 1, &fileHandle );
				file::FileReadInt( &m_GameSaveData.lastStagePosRotId,	sizeof(int), 1, &fileHandle );

				file::FileClose( &fileHandle );
			}
		}
	}
	else
	{
		// just setup the save unique save data

		if( m_GameSaveData.dataBlock.pItems != 0 )
		{
			delete[] m_GameSaveData.dataBlock.pItems;
			m_GameSaveData.dataBlock.pItems = 0;
		}

		// copy the registered data into the game save
		m_GameSaveData.dataBlock.itemDataSize = m_RegisteredVariables.itemDataSize;
		m_GameSaveData.dataBlock.itemCount = m_RegisteredVariables.itemCount;

		m_GameSaveData.dataBlock.pItems = new ItemData[m_RegisteredVariables.itemCount];

		for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
		{
			snprintf( m_GameSaveData.dataBlock.pItems[i].name, MAX_VARIABLE_NAME, "%s", m_RegisteredVariables.pItems[i].name );
			m_GameSaveData.dataBlock.pItems[i].value = m_RegisteredVariables.pItems[i].value;
		}
	}

	// set up
	if( m_DevData.enableSave )
	{
		for( unsigned int i=0; i < m_pScriptData->GetStageSetupsList().size(); ++i )
		{
			if( std::strcmp( m_pScriptData->GetStageSetupsList()[i]->stageFile, m_GameSaveData.lastStageEntry ) == 0 )
			{
				m_StageSetupFunc = m_pScriptData->GetStageSetupsList()[i]->stageSetupFunc;
			}
		}

		m_StageFile = m_GameSaveData.lastStageEntry;
		m_LevelName = res::GetScriptString( m_GameSaveData.lastStageEntryDescriptionId );
		m_PlayerSpawnId = m_GameSaveData.lastStagePosRotId;
	}
}

/////////////////////////////////////////////////////
/// Method: SaveGameData
/// Params: None
///
/////////////////////////////////////////////////////
void GameSystems::SaveGameData()
{
	int i=0;
	file::TFileHandle fileHandle;
	std::vector<int> inventoryItemList;
	
	if( m_InventoryManagement )
		inventoryItemList = m_InventoryManagement->GetItemList();

	char savePath[core::MAX_PATH+core::MAX_PATH];
	std::memset( &savePath, 0, sizeof(char)*core::MAX_PATH+core::MAX_PATH );
	snprintf( savePath, core::MAX_PATH+core::MAX_PATH, "%s/game.sav", core::app::GetSavePath() );

	if( file::FileOpen( savePath, file::FILETYPE_BINARY_WRITE, &fileHandle ) )
	{
		file::FileWriteInt( &m_GameSaveData.dataBlock.itemDataSize, sizeof(int), 1, &fileHandle );

		// save out registered variables
		file::FileWriteInt( &m_GameSaveData.dataBlock.itemCount, sizeof(int), 1, &fileHandle );
		for( i=0; i < m_GameSaveData.dataBlock.itemCount; ++i )
		{
			file::FileWriteChar( &m_GameSaveData.dataBlock.pItems[i].name, sizeof(char), MAX_VARIABLE_NAME, &fileHandle );
			file::FileWriteInt( &m_GameSaveData.dataBlock.pItems[i].value, sizeof(int), 1, &fileHandle );
		}

		// save out normal game data
		if( m_Player )
		{
			m_GameSaveData.pistolAmmo		= m_Player->GetPistolBulletMagCount( );
			m_GameSaveData.pistolAmmoTotal	= m_Player->GetPistolBulletTotal( );
			m_GameSaveData.shotgunAmmo		= m_Player->GetShotgunBulletMagCount( );
			m_GameSaveData.shotgunAmmoTotal = m_Player->GetShotgunBulletTotal( );
			m_GameSaveData.playerHealth		= m_Player->GetHealth( );
			m_GameSaveData.weaponIndex		= m_Player->GetWeaponIndex( );
			m_GameSaveData.clothingIndex	= m_Player->GetClothingIndex( );
		}

		file::FileWriteFloat( &m_GameSaveData.timePlayed,		sizeof(float), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.numKills,		sizeof(int), 1, &fileHandle );

		file::FileWriteInt( &m_GameSaveData.weaponIndex,		sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.clothingIndex,		sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.playerHealth,		sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.pistolAmmo,			sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.pistolAmmoTotal,	sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.shotgunAmmo,		sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.shotgunAmmoTotal,	sizeof(int), 1, &fileHandle );

		// save the inventory
		m_GameSaveData.inventoryTotalItems = static_cast<int>( inventoryItemList.size() );

		file::FileWriteInt( &m_GameSaveData.inventoryTotalItems, sizeof(int), 1, &fileHandle );
		for( i=0; i < m_GameSaveData.inventoryTotalItems; ++i )
		{
			int itemId = inventoryItemList[i];
			file::FileWriteInt( &itemId,	sizeof(int), 1, &fileHandle );
		}

		file::FileWriteChar( &m_GameSaveData.lastStageEntry,	sizeof(char), MAX_VARIABLE_NAME, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.lastStageEntryDescriptionId, sizeof(int), 1, &fileHandle );
		file::FileWriteInt( &m_GameSaveData.lastStagePosRotId,	sizeof(int), 1, &fileHandle );

		file::FileClose( &fileHandle );
	}
}
