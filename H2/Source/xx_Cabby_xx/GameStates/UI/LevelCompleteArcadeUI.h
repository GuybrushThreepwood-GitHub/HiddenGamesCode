

/*===================================================================
	File: LevelCompleteArcadeUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __LEVELCOMPLETEARCADEUI_H__
#define __LEVELCOMPLETEARCADEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class LevelCompleteArcadeUI : public UIBaseState, public IBaseGameState
{
	public:
		LevelCompleteArcadeUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~LevelCompleteArcadeUI();

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

		ProfileManager::eGameMode m_GameMode;
		GameData m_GameData;
};

#endif // __LEVELCOMPLETEARCADEUI_H__

