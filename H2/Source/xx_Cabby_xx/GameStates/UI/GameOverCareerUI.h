

/*===================================================================
	File: GameOverCareerUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMEOVERCAREERUI_H__
#define __GAMEOVERCAREERUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class GameOverCareerUI : public UIBaseState, public IBaseGameState
{
	public:
		GameOverCareerUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~GameOverCareerUI();

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

		GameData m_GameData;
		ProfileManager::Profile* m_pProfile;
};

#endif // __GAMEOVERCAREERUI_H__

