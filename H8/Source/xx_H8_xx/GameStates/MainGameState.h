
/*===================================================================
	File: MainGameState.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINGAMESTATE_H__
#define __MAINGAMESTATE_H__

#include "Player/Player.h"
#include "Enemy/EnemyManager.h"
#include "Core/Core.h"

#include "ScriptAccess/ScriptDataHolder.h"

#include "Profiles/ProfileManager.h"

// forward declare
class IState;
class IBaseGameState;

class BackgroundUI;

const int RANDOM_BEATS = 6;

class MainGameState : public IBaseGameState
{
	public:
		enum GameState
		{
			GameState_GameStartWait=0,
			GameState_Game,
			GameState_End
		};

	public:
		MainGameState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~MainGameState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void Restart();
		void ChangeGameState( GameState state );

	private:
		void LoadColourIndexList();
		void UpdatePhysics( float deltaTime );
		void CheckAchievements(float deltaTime);

	private:
		float m_LastDelta;

		IState* m_pState;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;	

		GameData m_GameData;

		GameCamera* m_GameCamera;
		math::Vec3 m_CamPos;
		math::Vec3 m_CamLookAt;

		GameState m_GameState;

		Player* m_Player;
		Core* m_Core;
		EnemyManager* m_EnemyManager;

		BackgroundUI* m_BG;

		float m_CountdownToCoreChange;

		ALuint m_LevelUpAudio;

		float m_RandomBeatTime;
		ALuint m_RandomBeatBufferIds[RANDOM_BEATS];
};

#endif // __MAINGAMESTATE_H__

