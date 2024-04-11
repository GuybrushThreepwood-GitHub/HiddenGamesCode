

/*===================================================================
	File: PauseUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __PAUSEUI_H__
#define __PAUSEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "Profiles/ProfileManager.h"
#include "GameStates/UI/UIFileLoader.h"

class PauseUI : public UIBaseState, public IBaseGameState
{
	public:
		PauseUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~PauseUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool QuitPress()		{ return m_QuitPress; }
		bool ContinuePress()	{ return m_ContinuePress; }
		bool ControlChange()	{ return m_ControlChange; }

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;
		ProfileManager::Profile* m_pProfile;

		bool m_QuitPress;
		bool m_ContinuePress;
		bool m_ControlChange;
};

#endif // __PAUSEUI_H__

