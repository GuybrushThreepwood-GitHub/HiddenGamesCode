
/*===================================================================
	File: GameSystems.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMESYSTEMS_H__
#define __GAMESYSTEMS_H__

#include "CollisionBase.h"
#include "ModelBase.h"
#include "SupportBase.h"

#include "AppConsts.h"
#include "InputSystem/InputSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "Player/InventoryManagement.h"
#include "Enemy/EnemyManagement.h"
#include "NavigationMesh/NavigationMesh.h"
#include "Decals/DecalSystem.h"
#include "Camera/GameCamera.h"
#include "Physics/PhysicsContact.h"
#include "MicroGame/MicroGame.h"

#include "ScriptedSequence/ScriptedSequencePlayer.h"

const int MAX_VARIABLE_NAME = 32;

class AchievementUI;

class GameSystems : public support::ScoresCallback
{
	public:
		struct GeneralData
		{
			GeneralData()
			{
				numCompletes = 0;
				currentActiveGame = 0;
				fpsModeUnlocked = 0;

				costume1Unlocked = 1;
				costume2Unlocked = 0;
				costume3Unlocked = 0;
				costume4Unlocked = 0;
				costume5Unlocked = 0;
				costume6Unlocked = 0;
				costume7Unlocked = 0;
				costume8Unlocked = 0;
				costume9Unlocked = 0;
			}

			int numCompletes;
			int currentActiveGame;
			int fpsModeUnlocked;

			int costume1Unlocked;
			int costume2Unlocked;
			int costume3Unlocked;
			int costume4Unlocked;
			int costume5Unlocked;
			int costume6Unlocked;
			int costume7Unlocked;
			int costume8Unlocked;
			int costume9Unlocked;
		};

		struct OptionsData
		{
			OptionsData()
			{
				leftHanded = false;
				//analogueSensitivity = 5;
				controlType = 1;
				lowPowerMode = false;

				// main game
				//crosshair = false;
				camLerp = true;
				noiseFilter = 2;
				fpsMode = false;
			}

			// external
			bool leftHanded;
			//int analogueSensitivity;
			int controlType;
			bool lowPowerMode;

			// main game
			//bool crosshair;
			bool camLerp;
			int noiseFilter;
			bool fpsMode;
		};

		struct BestRankData
		{
			BestRankData()
			{
				timePlayed = 9999.0f;

				numKills = 0;
				numDocs = 0;
				numTokens = 0;
				numHeads = 0;

				rankIndex = -1;
			}

			float timePlayed;

			int numKills;
			int numDocs;
			int numTokens;
			int numHeads;

			int rankIndex;
		};

		struct CompleteData
		{
			GeneralData generalData;
			OptionsData optionsData;
			BestRankData bestRankData;
		};

		struct ItemData
		{
			ItemData()
			{
				std::memset( name, 0, sizeof(char)*MAX_VARIABLE_NAME );
				value = 0;
			}
			char name[MAX_VARIABLE_NAME];
			int value;
		};

		struct ItemDataBlock
		{
			ItemDataBlock()
			{
				itemDataSize = 0;
				itemCount = 0;
				pItems = 0;
			}

			int itemDataSize;

			int itemCount;
			ItemData* pItems; 
		};

		struct GameSaveData
		{
			GameSaveData()
			{
				timePlayed = 0.0f;
				numKills = 0;

				weaponIndex = -1;
				clothingIndex = 0;
				playerHealth = 100;
			
				pistolAmmo = 12;
				pistolAmmoTotal = 99;
			
				shotgunAmmo = 6;
				shotgunAmmoTotal = 99;

				inventoryTotalItems = 0;
				pInventoryList = 0;

				std::memset( &lastStageEntry, 0, sizeof(char)*MAX_VARIABLE_NAME );
				snprintf( lastStageEntry, MAX_VARIABLE_NAME, "stages/s01/setup.lua" );
				lastStageEntryDescriptionId = 1;
				lastStagePosRotId = 1000;
			}

			ItemDataBlock dataBlock;

			float timePlayed;
			int numKills;

			int weaponIndex;
			int clothingIndex;
			int playerHealth;
			
			int pistolAmmo;
			int pistolAmmoTotal;
			
			int shotgunAmmo;
			int shotgunAmmoTotal;

			int inventoryTotalItems;
			int* pInventoryList;

			// to reload the correct area
			char lastStageEntry[MAX_VARIABLE_NAME];
			int lastStageEntryDescriptionId;
			int lastStagePosRotId;
		};

	public:
		GameSystems( InputSystem& inputSystem );
		~GameSystems();

		static GameSystems *GetInstance( void ) 
		{
			DBG_ASSERT_MSG( (ms_Instance != 0), "GameSystem instance has not been created" );

			return( ms_Instance );
		}
		static bool IsInitialised( void ) 
		{
			return( ms_Instance != 0 );
		}

		// support::ScoresCallback
		virtual void LoggedIn();
		virtual int GetAchievementId( const char* achievementString );
		virtual const char* GetAchievementString( int achievementId );
		virtual void AchievementSubmitted( int achievementId, int errorId );

		void Update( float deltaTime );

		// options
		OptionsData& GetOptions()					{ return m_CompleteSaveData.optionsData; }
		void SetOptions( OptionsData& data )		{ m_CompleteSaveData.optionsData = data; }

		// general data
		GeneralData& GetGeneralData()				{ return m_CompleteSaveData.generalData; }
		void SetGeneralData( GeneralData& data )	{ m_CompleteSaveData.generalData = data; }

		// best rank data
		BestRankData& GetBestRankData()				{ return m_CompleteSaveData.bestRankData; }
		void SetBestRankData( BestRankData& data )	{ m_CompleteSaveData.bestRankData = data; }

//#ifdef DEVELOPER_SETUP
		GameSaveData& GetSaveData()					{ return m_GameSaveData; }
		void SetSaveData( GameSaveData& data )		{ m_GameSaveData = data; }
//#endif // DEVELOPER_SETUP

		// player
		void CreatePlayer();
		void DestroyPlayer();

        // scores
        void CreateScore();
        void DestroyScore();
		void AwardAchievement( int achievementId );
		void PrepareAchievementUI( int achievementId );
		void UpdateAchievementUI( float deltaTime );
		void DrawAchievementUI();

        // purchase
		void CreatePurchase();
		void DestroyPurchase();	

		// inventory
		void AddItem( int index );
		bool HasItem( int index );
		void RemoveItem( int index );

		// talk
		void Say( int index1, int index2, const char* callFunc );
		void GetSay( int* index1, int* index2, const char** callFunc );
		void ClearText();

		void SetTalkState( bool state );
		bool IsTalking();

		// physics
		void CreatePhysics();
		void DestroyPhysics();

		// level
		void CreateLevel( int playerSpawnId );
		void CallLevelOnCreate();
		void DestroyLevel();
		void DrawLevel( bool sortedMeshes );
		void DrawLevelSkybox();
		void DrawLevelEmitters();
		void UpdateLevel(float deltaTime);

		// game camera
		void CreateGameCamera();
		void DestroyGameCamera();

		// create enemy management
		void CreateEnemyManager();
		void DestroyEnemyManager();

		// create decal system
		void CreateDecalSystem();
		void DestroyDecalSystem();

		// level change
		void SetCorrectStage();
		void ContinueGame();
		void ChangeStage( const char* stageName, const char* stageSetupFunc, const char* levelName, int descriptionIndex, int playerSpawnId );
		void DestroyStage();
		void LoadNewStage();
		void SetChangeStageFlag( bool state )	{ m_StageChange = state; }
		bool GetChangeStageFlag()				{ return m_StageChange; }
		void SetLevelName( const char* name )	{ m_LevelName = name; }
		const char* GetLevelName()				{ return m_LevelName; }

		// microgame
		void LaunchMicroGame( int microGame, lua_State* pState );
		bool IsMicroGameActive()				{ return m_InMicroGame; }
		void EndMicroGame( int result );

		void SetEndGameState( bool state )		{ m_EndGameState = state; }
		bool GetEndGameState()					{ return m_EndGameState; }

		// scripted sequence
		void CreateScriptedSequencePlayer();
		void DestroyScriptedSequencePlayer();

		// object
		BaseObject* FindObject( int id );

		// debug
		void DrawDebug();

		// load/save
		void LoadOptions();
		void SaveOptions();

		void LoadBestRank();
		void SaveBestRank();

		void LoadGeneralData();
		void SaveGeneralData();

		void RegisterSaveVariables();
		void SetDataValue( const char* varName, int varValue );
		int GetDataValue( const char* varName );
		void ClearDataValues();

		void LoadGameData();
		void SaveGameData();

		// access
		Level& GetLevel();
		Player& GetPlayer();
		EnemyManagement& GetEnemyManager();
		InventoryManagement& GetInventory();
		DecalSystem& GetDecalSystem();

		GameCamera* GetGameCamera();
		MicroGame* GetMicroGame();

		ScriptedSequencePlayer* GetScriptedSequencePlayer();

	private:
		InputSystem& m_InputSystem;

		CompleteData m_CompleteSaveData;

		GameSaveData m_GameSaveData;
		ItemDataBlock m_RegisteredVariables;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;
		ScriptDataHolder::CameraSetup m_CameraData;
		ScriptDataHolder::StageDefinition m_StageData;
		GameData m_GameData;

		Level* m_Level;
		Player* m_Player;
		NavigationMesh* m_NavMesh;
		GameCamera* m_GameCamera;
		EnemyManagement* m_EnemyManagement;
		InventoryManagement* m_InventoryManagement;
		DecalSystem* m_DecalSystem;

		ScriptedSequencePlayer* m_ScriptedSequencePlayer;

		// talk
		bool m_IsTalking;
		int m_SayLine1;
		int m_SayLine2;
		const char* m_SayFuncCall;

		// microgame
		bool m_InMicroGame;
		MicroGame* m_MicroGame;
		const char* m_SuccessMicroGameFunc;

		ContactListener m_ContactListener;
		AchievementUI* m_AchievementUI;

		// stage load
		bool m_StageChange;
		const char* m_StageFile;
		const char* m_StageSetupFunc;
		const char* m_LevelName;
		int m_PlayerSpawnId;

		// end game
		bool m_EndGameState;

		static GameSystems* ms_Instance;
};

inline Level& GameSystems::GetLevel()
{
	return(*m_Level);
}

inline Player& GameSystems::GetPlayer()
{
	return(*m_Player);
}

inline EnemyManagement& GameSystems::GetEnemyManager()
{
	return(*m_EnemyManagement);
}

inline InventoryManagement& GameSystems::GetInventory()
{
	return(*m_InventoryManagement);
}

inline DecalSystem& GameSystems::GetDecalSystem()
{
	return(*m_DecalSystem);
}

inline GameCamera* GameSystems::GetGameCamera()
{
	return(m_GameCamera);
}

inline MicroGame* GameSystems::GetMicroGame()
{
	return(m_MicroGame);
}

inline ScriptedSequencePlayer* GameSystems::GetScriptedSequencePlayer()
{
	return(m_ScriptedSequencePlayer);
}

#endif // __GAMESYSTEMS_H__

