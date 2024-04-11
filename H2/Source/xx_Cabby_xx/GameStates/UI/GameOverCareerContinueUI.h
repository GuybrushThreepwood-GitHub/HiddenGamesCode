

/*===================================================================
	File: GameOverCareerContinueUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMEOVERCAREERCONTINUEUI_H__
#define __GAMEOVERCAREERCONTINUEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class GameOverCareerContinueUI : public UIBaseState, public IBaseGameState
{
	public:
		GameOverCareerContinueUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~GameOverCareerContinueUI();

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

		ProfileManager::Profile* m_pProfile;
		GameData m_GameData;
		ProfileManager::ActiveLevelData* m_pLevelData;

		int m_NumberOfLivesRequested;
		int m_MaxNumberOfLives;
};

#endif // __GAMEOVERCAREERCONTINUEUI_H__

