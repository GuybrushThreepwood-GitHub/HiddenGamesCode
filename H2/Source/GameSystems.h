
/*===================================================================
	File: GameSystems.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMESYSTEMS_H__
#define __GAMESYSTEMS_H__

#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "SupportBase.h"

#include "CabbyConsts.h"
#include "InputSystem/InputSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "Customers/CustomerManagement.h"

#include "Camera/GameCamera.h"


const int MAX_VARIABLE_NAME = 32;

class AchievementUI;

class GameSystems : public support::ScoresCallback
{
	public:
		enum ESaveAction
		{
			ESaveAction_Clear=0,
			ESaveAction_Save
		};

		struct SkyFogData
		{
			int skyboxIndex;
			mdl::ModelHGM* skyModel;
			renderer::TGLFogState fogState;
		};

	public:
		GameSystems( InputSystem& inputSystem );
		virtual ~GameSystems();

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
		virtual int GetKiipAchievementId( const char* achievementString );
		virtual const char* GetKiipAchievementString( int achievementId );
		virtual void AchievementSubmitted( int achievementId, int errorId );

		void Update( float deltaTime );

		// player
		void CreatePlayer();
		void DestroyPlayer();

		// physics
		void CreatePhysics();
		void DestroyPhysics();
    
        // scores
        void CreateScore();
        void DestroyScore();
		void AwardAchievement( int achievementId );
		void AwardKiipAchievement( int achievementId );
		void PrepareAchievementUI( int achievementId );
		void UpdateAchievementUI( float deltaTime );
		void DrawAchievementUI();

        // purchase
		void CreatePurchase();
		void DestroyPurchase();		
	
		// level
		void CreateLevel();
		void DestroyLevel();
		void LoadNewStage();

		// game camera
		void CreateGameCamera();
		void CreateGameCameraNoPlayer();
		void DestroyGameCamera();

		void SetCustomerManager( CustomerManagement* manager )	{ m_CustomerManager = manager; }
		CustomerManagement* GetCustomerManager()				{ return m_CustomerManager; }

		// stage
		void SetCorrectStage();

		// debug
		void DrawDebug();

		// access
		Level& GetLevel();
		Player& GetPlayer();
		GameCamera* GetGameCamera();
		const ScriptDataHolder::DevScriptData& GetDevData();
		const GameData& GetGameData();

		void ClearPoints()												{ m_TotalPoints = 0; }
		void AddPoints( int points )									{ m_TotalPoints += points; }
		void RemovePoints( int points )									{ m_TotalPoints -= points; if( m_TotalPoints<0 )m_TotalPoints=0;}
		int GetScore()													{ return m_TotalPoints; }

	private:
		InputSystem& m_InputSystem;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;
		//ScriptDataHolder::CameraSetup m_CameraData;
		//ScriptDataHolder::StageDefinition m_StageData;
		GameData m_GameData;

		// current stage
		const char* m_StageFile;
		const char* m_StageSetupFunc;

		Level* m_Level;
		Player* m_Player;
		CustomerManagement* m_CustomerManager;

		GameCamera* m_GameCamera;

		//ContactListener m_ContactListener;
		AchievementUI* m_AchievementUI;

		int m_TotalPoints;

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

inline GameCamera* GameSystems::GetGameCamera()
{
	return(m_GameCamera);
}

inline const ScriptDataHolder::DevScriptData& GameSystems::GetDevData()
{
	return(m_DevData);
}

inline const GameData& GameSystems::GetGameData()
{
	return(m_GameData);
}

#endif // __GAMESYSTEMS_H__

