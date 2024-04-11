

/*===================================================================
	File: LevelCompleteUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVELCOMPLETEUI_H__
#define __LEVELCOMPLETEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class LevelCompleteUI : public UIBaseState, public IBaseGameState
{
	public:
		LevelCompleteUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~LevelCompleteUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void Setup();

		bool ResumeGame()			{ return m_ResumeGame; }
		bool ReplayMode()			{ return m_ReplayMode; }

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		ProfileManager::Profile* m_pProfile;

		GameData m_GameData;

		bool m_ResumeGame;
		bool m_ReplayMode;
};

#endif // __LEVELCOMPLETEUI_H__

