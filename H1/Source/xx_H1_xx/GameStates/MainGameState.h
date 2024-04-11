
/*===================================================================
	File: MainGameState.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINGAMESTATE_H__
#define __MAINGAMESTATE_H__

#include "Level/Level.h"
#include "Player/Player.h"

#include "ScriptAccess/ScriptDataHolder.h"

#include "Profiles/ProfileManager.h"

// forward declare
class IState;
class IBaseGameState;

class MainGameState : public IBaseGameState
{
	public:
		MainGameState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~MainGameState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();
	
		void ContinueLevel();
		void RestartLevel();
		void ReplayLevel();

		void UIStartLevel()	{ m_LevelStartWait = false; }

	private:
		void CheckAchievements(float deltaTime);

	private:
		float m_LastDelta;

		IState* m_pState;

		ScriptDataHolder* m_pScriptData;
		float m_GameTime;

		ScriptDataHolder::DevScriptData m_DevData;	
		ScriptDataHolder::PhysicsSetup m_PhysicsData;
		ScriptDataHolder::LevelScriptData m_LevelData;
		GameData m_GameData;

		ScriptDataHolder::LevelLightBlock m_LevelLight;
		ScriptDataHolder::LevelLightBlock m_LevelLightBackup;
		ScriptDataHolder::LevelLightBlock m_PlayerNightLight;

		renderer::TGLFogState m_LevelFog;
		renderer::TGLFogState m_LevelFogBackup;

		ProfileManager::ActiveLevelData* m_pLevelData;

		int m_LevelIndex;
		int m_PlaneIndex;
		GameSystems::GameMode m_GameMode;
		int m_TimeOfDay;

		GameCamera* m_GameCamera;
		math::Vec3 m_CamPos;
		math::Vec3 m_CamLookAt;

		Player* m_Player;
		Level m_Level;

		bool m_LevelStartWait;
		bool m_LevelCompleteMode;

		bool m_WasPausedLastFrame;

		ALuint m_TimerTick;
		ALuint m_TimerEnd;
		float m_TickTime; 

		bool m_InitCountdown;
		float m_CountdownTime; 
		
		bool m_TokenTimerStarted;
		float m_TokenCollectCountdownTimer; 

		bool m_DeadFirstFrame;
		float m_PlayerDeadTimer;

		bool m_CalledRespawnFade;

		// weather effects
		bool m_IsSnowing;
		bool m_IsRaining;

		efx::Emitter* m_FlyingEffect;
		efx::Emitter* m_SnowEffect;
		efx::Emitter* m_RainEffect;

		bool m_BoundingAreaEnabled;
		int m_BoundingAlpha;
		mdl::ModelHGM* m_BoundingAreaEffect;

		ALuint m_RainSrcId;
		ALuint m_RainBufferId;

		// achievement timers
		float m_AchAircraftTimer;
		float m_AchLevelPlayedTimer;
		float m_AchWeatherTimer;
		float m_AchTimeOfDayTimer;
		float m_AchFreeFlightTimer;

		int m_LastCountObjectsDestroyed;

		float m_FlightTime;
};

#endif // __MAINGAMESTATE_H__

