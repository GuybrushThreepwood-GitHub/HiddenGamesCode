
/*===================================================================
	File: MainState.h
	Game:

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINSTATE_H__
#define __MAINSTATE_H__

#include "Level/Level.h"
#include "Player/Player.h"
#include "Enemy/EnemyManagement.h"
#include "Decals/DecalSystem.h"
#include "ScriptedSequence/ScriptedSequencePlayer.h"

#include "Camera/Camera.h"
#include "Camera/GameCamera.h"

#include "MicroGame/MicroGame.h"
#include "GameStates/UI/MainStateUI.h"
#include "GameStates/UI/MainStateUITablet.h"

// forward declare
class IState;
class IBaseGameState;

class MainState : public IBaseGameState
{
	public:
		MainState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~MainState();

		void Setup();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		void UpdateB2DPhysics( float deltaTime );

		void UpdateLoadState();
		void DrawLoadState();

		void UpdateDeathState( float deltaTime );
		void DrawDeathState();

		bool UpdateMicroGameAndTransitions( float deltaTime );

	private:
		float m_LastDelta;
		float m_CurrentGameTime;

		bool m_StartSilence;
		float m_MusicTimer;

		MainStateUI* m_pUIState;

		mdl::ModelHGM* m_ContinueMesh;
		UIFileLoader m_ContinueElements;

		// load states
		enum LoadState
		{
			LoadState_None=-1,
			LoadState_TransitionIn=0,
			LoadState_TransitionOut
		};

		bool m_FirstLoad;
		float m_FirstLoadTime;
		float m_LoadTimer;
		LoadState m_LoadTransitionState;
		bool m_LoadStarted;
		const char* m_LevelName;

		int m_TransitionAlpha;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;
		ScriptDataHolder::CameraSetup m_CameraData;
		ScriptDataHolder::StageDefinition m_StageData;

		GameCamera* m_GameCamera;
		Player* m_Player;
		EnemyManagement* m_EnemyManagement;
		DecalSystem* m_DecalSystem;

		//ScriptedSequencePlayer* m_ScriptedSequencePlayer;

		// microgame
		bool m_MicroGameTransitionIn;
		bool m_MicroGameTransitionInShow;
		bool m_MicroGameTransitionToQuit;
		bool m_MicroGameTransitionToMainGame;
		bool m_IsMicroGameActive;
		MicroGame* m_MicroGame;

		// accelerometer control
		bool m_EnabledAccelerometer;
		float m_ShakeAllowTime;
	
		renderer::TGLFogState m_FogState;

		// death state
		bool m_DeathFadeComplete;
		math::Vec4Lite m_DeathFadeColour;
		bool m_ShowDeadState;
		bool m_SelectedToContinue;
		bool m_SelectedToQuit;

		// dead text movement
		float m_MovementTimer;
		math::Vec3 m_CurrentMovement;
		int m_MoveAlpha;
};

#endif // __MAINSTATE_H__

