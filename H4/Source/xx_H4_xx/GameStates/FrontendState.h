
/*===================================================================
	File: FrontendState.h
	Game:

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDSTATE_H__
#define __FRONTENDSTATE_H__

#include "GameStates/UI/UIFileLoader.h"

#include "GameSystems.h"

// forward declare
class IState;
class IBaseGameState;

class FrontendState : public IBaseGameState
{	
	private:
		enum FrontendStates
		{
			FrontendState_Main=0,
			FrontendState_GameSelect,
			FrontendState_NewGame,
			FrontendState_Rank,
			FrontendState_Options,
			FrontendState_About
		};

		// load states
		enum LoadState
		{
			LoadState_None=-1,
			LoadState_TransitionOut=0
		};

	public:
		FrontendState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~FrontendState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		void SetupState( FrontendStates state );
		bool UpdateLoadState();

	private:
		float m_LastDelta;
		bool m_InOptions;

		bool m_NewGame;
		bool m_ContinueGame;

		ScriptDataHolder* m_pScriptData;
		ScriptDataHolder::DevScriptData m_DevData;

		LoadState m_LoadTransitionState;
		bool m_LoadStarted;
		const char* m_LevelName;
		int m_TransitionAlpha;

		GameSystems::OptionsData m_CurrentOptions;
		GameSystems::BestRankData m_CurrentBestRank;
		GameSystems::GeneralData m_CurrentGeneralData;

		FrontendStates m_CurrentState;

		mdl::ModelHGM* m_FrontendMesh;
		mdl::ModelHGM* m_OptionsMesh;
		mdl::ModelHGM* m_RankMesh;

		UIFileLoader m_FrontendElements;
		UIFileLoader m_OptionsElements;
		UIFileLoader m_RankElements;

		bool m_SensitivityHeld;
		math::Vec3 m_SensBarStartPos;
		math::Vec3 m_SensBarMinRange;
		math::Vec3 m_SensBarMaxRange;

		int m_SensBarRange;
		int m_SensBarXPos;

		bool m_CreditsList;

		// text movement
		float m_MovementTimer;
		math::Vec3 m_CurrentMovement;
		int m_Alpha;
};

#endif // __FRONTENDSTATE_H__

