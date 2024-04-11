

/*===================================================================
	File: FrontendPromptsUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __FRONTENDPROMPTSUI_H__
#define __FRONTENDPROMPTSUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"
#include "Profiles/ProfileManager.h"

class FrontendPromptsUI : public UIBaseState, public IBaseGameState
{
	public:
		FrontendPromptsUI( StateManager& stateManager, InputSystem& inputSystem, bool fullClear );
		virtual ~FrontendPromptsUI();

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

		bool m_FullClear;
		ProfileManager::Profile* m_pProfile;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;
};

#endif // __FRONTENDPROMPTSUI_H__

