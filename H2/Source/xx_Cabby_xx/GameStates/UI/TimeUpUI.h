

/*===================================================================
	File: TimeUpUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __TIMEUPUI_H__
#define __TIMEUPUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "Profiles/ProfileManager.h"
#include "GameStates/UI/UIFileLoader.h"

class TimeUpUI : public UIBaseState, public IBaseGameState
{
	public:
		TimeUpUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~TimeUpUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool ContinuePress()	{ return m_ContinuePress; }

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;
		ProfileManager::Profile* m_pProfile;

		bool m_ContinuePress;
};

#endif // __TIMEUPUI_H__

