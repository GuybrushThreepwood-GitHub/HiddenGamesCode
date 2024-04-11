
/*===================================================================
	File: PauseMenuUI.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __PAUSEMENUUI_H__
#define __PAUSEMENUUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class PauseMenuUI : public UIBaseState, public IBaseGameState
{
	public:
		PauseMenuUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~PauseMenuUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool Resume()				{ return m_Resume; }
		bool Restart()				{ return m_Restart; }
		bool Quit()					{ return m_Quit; }

		void RefreshSelected();
	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		bool m_Resume;
		bool m_Restart;
		bool m_Quit;
};

#endif // __PAUSEMENUUI_H__

