
/*===================================================================
	File: MainGameState.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINGAMESTATE_H__
#define __MAINGAMESTATE_H__

#include "Level/Level.h"
#include "Player/Player.h"
#include "Customers/CustomerManagement.h"

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
	
		void ReplayLevel();
		void UIStartLevel()	{ m_LevelStartWait = false; }

	private:
		void UpdatePhysics( float deltaTime );
		void CheckCameraAgainstWater();

	private:
		float m_LastDelta;

		IState* m_pState;

		ScriptDataHolder* m_pScriptData;
		float m_GameTime;

		ScriptDataHolder::DevScriptData m_DevData;	
		ScriptDataHolder::LevelScriptData m_LevelData;
		GameData m_GameData;

		ScriptDataHolder::LevelLightBlock m_LevelLight;
		ScriptDataHolder::LevelLightBlock m_LevelLightBackup;
		ScriptDataHolder::LevelLightBlock m_PlayerNightLight;
		ScriptDataHolder::LevelLightBlock m_WaterLight;

		renderer::TGLFogState m_LevelFog;
		renderer::TGLFogState m_LevelFogBackup;
		renderer::TGLFogState m_WaterFog;

		ProfileManager::Profile* m_pProfile;
		ProfileManager::ActiveLevelData* m_pLevelData;

		float m_CamZoomOut;
		math::Vec3 m_CamPos;
		math::Vec3 m_CamLookAt;
		int m_NumFramesX;
		int m_NumFramesY;

		Player* m_Player;
		Level m_Level;
		CustomerManagement* m_Customers;

		bool m_LevelStartWait;
		bool m_LevelCompleteCareer;
		bool m_LevelCompleteArcade;
		bool m_GameOver;

		bool m_WasPausedLastFrame;
		bool m_bFullZoomIn;
		bool m_bFullZoomOut;

		bool m_PlayerInWater;
		bool m_DeadFirstFrame;
		float m_PlayerDeadTimer;

		bool m_RescueFirstFrame;
		float m_PlayerRescueTimer;

		ALuint m_ExtraLifeId;
		ALuint m_TimerTick;
		ALuint m_TimerEnd;

		float m_TickTime; 
		bool m_CalledRespawnFade;

		bool m_CalledRescueFade;

		// unlock data
		mdl::ModelHGM* m_pVehicleModel;
		float m_Rotation;
		int m_UnlockVehicleId;

		// camera to water collision
		int m_CameraWaterState;
		collision::Sphere m_CameraCollision;

};

#endif // __MAINGAMESTATE_H__

