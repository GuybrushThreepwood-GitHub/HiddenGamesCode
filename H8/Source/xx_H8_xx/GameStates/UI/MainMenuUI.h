
/*===================================================================
	File: MainMenuUI.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINMENUUI_H__
#define __MAINMENUUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class MainMenuUI : public UIBaseState, public IBaseGameState
{
	public:
		MainMenuUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~MainMenuUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool StartGame()			{ return m_StartGame; }

		void RefreshSelected();

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		float m_TapFlashTime;
		bool m_ShowTapFlash;

		bool m_StartGame;
};

#endif // __MAINMENUUI_H__

