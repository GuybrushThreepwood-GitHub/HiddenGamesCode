

/*===================================================================
	File: LevelCompleteCareerUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVELCOMPLETECAREERUI_H__
#define __LEVELCOMPLETECAREERUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class LevelCompleteCareerUI : public UIBaseState, public IBaseGameState
{
	public:
		LevelCompleteCareerUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~LevelCompleteCareerUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		bool m_QuitEnabled;
		ProfileManager::Profile* m_pProfile;

		ProfileManager::eGameMode m_GameMode;
		GameData m_GameData;
};

#endif // __LEVELCOMPLETECAREERUI_H__

