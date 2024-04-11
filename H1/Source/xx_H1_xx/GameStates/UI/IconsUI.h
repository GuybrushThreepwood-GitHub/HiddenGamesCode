

/*===================================================================
	File: IconsUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __ICONSUI_H__
#define __ICONSUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"
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
		IconsTabletSDUI* m_UITabletSD;
};

#endif // __ICONSUI_H__

