
/*===================================================================
	File: GameSystems.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMESYSTEMS_H__
#define __GAMESYSTEMS_H__

#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "SupportBase.h"

#include "H1Consts.h"
#include "InputSystem/InputSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "Level/Level.h"
#include "Player/Player.h"
#include "Physics/RayObject.h"
#include "EffectPool/EffectPool.h"
#include "Camera/GameCamera.h"


const int MAX_VARIABLE_NAME = 32;

class AchievementUI;
class RayObject;

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

		enum GameMode
		{
			GAMEMODE_FREEFLIGHT=0,
			GAMEMODE_TARGET,
			GAMEMODE_RINGRUN
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
		virtual void AchievementSubmitted( int achievementId, int errorId );

		void Update( float deltaTime );

		// player
		void CreatePlayer();
		void DestroyPlayer();
		void SetPlayer( Player* player )		{ m_Player = player; }

		// physics
		void CreatePhysics();
		void DestroyPhysics();
		RayObject* GetRayObject()	{ return m_RayObject; }

        // scores
        void CreateScore();
        void DestroyScore();
		void AwardAchievement( int achievementId, int textId  );
		void AwardAchievement( int achievementId, const char* text );
		void PrepareAchievementUI( int achievementId, int textId  );
		void PrepareAchievementUI( int achievementId, const char* text );
		void UpdateAchievementUI( float deltaTime );
		void DrawAchievementUI();
		bool HasAchievement( int achievementId );

        // purchase
		void CreatePurchase();
		void DestroyPurchase();		
	
		// level
		void CreateLevel();
		void DestroyLevel();
		void LoadNewStage();
		void DrawLevel( bool sortedMeshes );
		void DrawLevelEmitters();
		void UpdateLevel(float deltaTime);
		void SetLevel( Level* activeLevel )			{ m_Level = activeLevel; }

		// game camera
		void CreateGameCamera();
		void CreateGameCameraNoPlayer();
		void DestroyGameCamera();

		// preload resources
		void PreloadResources();
		void ClearPreloaded();

		// stage
		void SetCorrectStage();

		void CreateEffectPool( int maxExplosions, int maxFires );
		void DestroyEffectPool();
		void DrawEffectPool();
		void UpdateEffectPool( float deltaTime );
		void ResetEffectPool();
		void SpawnExplosion( const math::Vec3& pos );
		void SpawnFire( const math::Vec3& pos );
		void SpawnStars( const math::Vec3& pos );

		// audio
		void InitAudio();
		void PlayObjectDestroyedAudio();
		void PlayTargetDestroyedAudio();
		void PlayRingPassAudio( float pitchExtra );
		void PlayLevelCompleteAudio();

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
		
		void ResetTargetsDestroyed()									{ m_TargetsDestroyed = 0; }
		int GetTargetsDestroyed()										{ return m_TargetsDestroyed; }
		void SetTargetsDestroyed( int count )							{ m_TargetsDestroyed = count; }

		void ResetObjectsDestroyed()									{ m_ObjectsDestroyed = 0; }
		int GetObjectsDestroyed()										{ return m_ObjectsDestroyed; }
		void SetObjectsDestroyed( int count )							{ m_ObjectsDestroyed = count; }

		void ResetLaps()												{ m_LapsComplete = 0; }
		int GetLapsCompleted()											{ return m_LapsComplete; }
		void SetLapsComplete( int count )								{ m_LapsComplete = count; }

		void ResetGatesComplete()										{ m_GateComplete = 0; }
		int GetGatesComplete()											{ return m_GateComplete; }
		void SetGatesComplete( int count )								{ m_GateComplete = count; }

		int GetGatesTotal()												{ return m_TotalGates; }
		void SetGatesTotal( int count )									{ m_TotalGates = count; }

		void SetGameMode( GameMode mode )								{ m_GameMode = mode; }
		void SetGameMode( GameMode mode, int index, int extraData )		{ m_GameMode = mode; m_GameModeIndex = index; m_GameModeExtraData = extraData; }
		void SetGameModeIndex( int index )								{ m_GameModeIndex = index; }
		void SetGameModeExtraData( int data )							{ m_GameModeExtraData = data; }
		GameMode GetGameMode()											{ return m_GameMode; }
		int GetGameModeIndex()											{ return m_GameModeIndex; }
		int GetGameModeExtraData()										{ return m_GameModeExtraData; }

	private:
		InputSystem& m_InputSystem;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		GameMode m_GameMode;
		int m_GameModeIndex;
		int m_GameModeExtraData;

		// current stage
		const char* m_StageFile;
		const char* m_StageSetupFunc;

		Level* m_Level;
		Player* m_Player;
		RayObject* m_RayObject;
		EffectPool* m_EffectPool;

		GameCamera* m_GameCamera;

		AchievementUI* m_AchievementUI;

		int m_TotalPoints;
		int m_TargetsDestroyed;

		int m_LapsComplete;
		int m_GateComplete;
		int m_TotalGates;

		// 
		int m_ObjectsDestroyed;

		ALuint m_ObjectDestroyBufferId;
		ALuint m_TargetDestroyBufferId;
		ALuint m_RingPassBufferId;

		ALuint m_LevelCompleteBufferId;

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

