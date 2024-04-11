

/*===================================================================
	File: IconsUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __ICONSUI_H__
#define __ICONSUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/IconsPhoneSDUI.h"
#include "GameStates/UI/IconsPhoneRetinaUI.h"

#include "GameStates/UI/IconsTabletSDUI.h"

#include "GameStates/UI/UIFileLoader.h"

class IconsUI : public UIBaseState, public IBaseGameState
{
	public:
		IconsUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~IconsUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	protected:
		IconsPhoneSDUI*	m_UIPhoneSD;
		IconsPhoneRetinaUI*	m_UIPhoneRetina;

		IconsTabletSDUI* m_UITabletSD;
};

#endif // __ICONSUI_H__

