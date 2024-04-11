

/*===================================================================
	File: GameOverArcadeUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMEOVERARCADEUI_H__
#define __GAMEOVERARCADEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class GameOverArcadeUI : public UIBaseState, public IBaseGameState
{
	public:
		GameOverArcadeUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~GameOverArcadeUI();

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
};

#endif // __GAMEOVERARCADEUI_H__

