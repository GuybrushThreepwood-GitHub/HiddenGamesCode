
/*===================================================================
	File: GameSystems.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMESYSTEMS_H__
#define __GAMESYSTEMS_H__

#include "CollisionBase.h"
#include "ModelBase.h"
#include "SoundBase.h"
#include "SupportBase.h"

#include "H8Consts.h"
#include "InputSystem/InputSystem.h"

#include "ScriptAccess/ScriptDataHolder.h"
#include "Player/Player.h"
#include "Core/Core.h"
#include "EffectPool/EffectPool.h"
#include "Camera/GameCamera.h"
#include "Physics/PhysicsContact.h"

const int MAX_VARIABLE_NAME = 32;

class AchievementUI;
class GameUI;
class RayObject;

class GameSystems : public support::ScoresCallback
{
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

		// game camera
		void CreateGameCamera();
		void CreateGameCameraNoPlayer();
		void DestroyGameCamera();

		// preload resources
		void PreloadResources();
		void ClearPreloaded();

		// effects
		void CreateEffectPool( );
		void DestroyEffectPool();
		void DrawEffectPool();
		void UpdateEffectPool( float deltaTime );
		void ResetEffectPool();
		void SpawnExplosion( const math::Vec3& pos, const math::Vec3& col );
		void SpawnCircle( const math::Vec3& pos, const math::Vec3& col );
		void SpawnHearts();

		// game
		void SetCore( Core* core )		{ m_Core = core; }
		Core* GetCore()					{ return m_Core; }

		void SetGameUI( GameUI* ui )	{ m_GameUI = ui; }

		void LoadColourIndexList();
		void SetValidColoursList( std::vector<int> colourList );
		b2Color GetColourForIndex( int colourIndex, float multiply );
		math::Vec3 GetColourForIndex( int colourIndex );

		void AddToScore( int val );
		void ResetScore();
		unsigned long long int GetCurrentScore()			{ return m_CurrentScore; }

		void SetPersonalBest( unsigned long long int personalBest );
		unsigned long long int GetPersonalBest()		{ return m_PersonalBest; }

		void AddTextToConsole( const char* text, ...);

		// level
		void ResetLevelCounter()			{ m_Level = 0; }
		void IncrementLevelCounter()		{ m_Level++; }
		int GetLevelCounter()				{ return m_Level; }

		// cycle each 10 absorptions
		void ResetLevelUpCounter()			{ m_LevelUpgradeCounter = 0; }
		void IncrementLevelUpCounter()		{ m_LevelUpgradeCounter++; }
		int GetLevelUpCounter()				{ return m_LevelUpgradeCounter; }

		// wrap around count (number of times the level up has gone through all shields)
		void ResetWrapAroundCounter()		{ m_WrapAroundCounter = 0; }
		void IncrementWrapAroundCounter()	{ m_WrapAroundCounter++; }
		int GetWrapAroundCounter()			{ return m_WrapAroundCounter; }

		// audio
		void InitAudio();

		// debug
		void DrawDebug();

		// access
		Player& GetPlayer();
		GameCamera* GetGameCamera();
		const ScriptDataHolder::DevScriptData& GetDevData();
		const GameData& GetGameData();
		std::vector<int> GetValidColoursList()						{ return m_ValidGameColourList; }
		math::Vec3* GetColourLookupPtr()							{ return &m_ColourLookup[0]; }

		void SetBGMSource( ALuint bgmSource )					{ m_BGMSource = bgmSource; }
		void SetBGMPitch( float val )							{ m_BGMPitch = val; }
		void IncreaseBGMPitch()
		{
			if( m_BGMSource != snd::INVALID_SOUNDSOURCE )
			{
				m_BGMPitch += 0.001f;
				if( m_BGMPitch > 2.0f )
					m_BGMPitch = 2.0f;

				alSourcef( m_BGMSource, AL_PITCH, m_BGMPitch );
			}
		}

	private:
		InputSystem& m_InputSystem;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;
		GameData m_GameData;

		Player* m_Player;
		Core* m_Core;

		EffectPool* m_EffectPool;

		GameCamera* m_GameCamera;
		ContactListener m_ContactListener;

		AchievementUI* m_AchievementUI;
		GameUI* m_GameUI;

		bool m_JustBeatenPB;
		unsigned long long int m_CurrentScore;
		unsigned long long int m_PersonalBest;

		int m_LevelUpgradeCounter;
		int m_WrapAroundCounter;
		int m_Level;

		std::vector<int> m_ValidGameColourList;

		int m_TotalColours;
		math::Vec3 m_ColourLookup[MAX_COLOURS];

		ALuint m_BGMSource;
		float m_BGMPitch;

		static GameSystems* ms_Instance;
};

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

