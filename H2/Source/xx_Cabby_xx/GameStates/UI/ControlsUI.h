

/*===================================================================
	File: ControlsUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __CONTROLSUI_H__
#define __CONTROLSUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/ControlsPhoneSDUI.h"
#include "GameStates/UI/ControlsPhoneRetinaUI.h"

#include "GameStates/UI/ControlsTabletSDUI.h"
#include "GameStates/UI/ControlsTabletRetinaUI.h"

#include "GameStates/UI/UIFileLoader.h"

class ControlsUI : public UIBaseState, public IBaseGameState
{
	public:
		ControlsUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~ControlsUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool PausePress();
		bool UpPress();
		bool DownPress();
		bool LeftPress();
		bool RightPress();
		bool LandingGearPress();

		void SetupControls( int controlType );

	protected:
		ControlsPhoneSDUI*	m_UIPhoneSD;
		ControlsPhoneRetinaUI*	m_UIPhoneRetina;

		ControlsTabletSDUI* m_UITabletSD;
		ControlsTabletRetinaUI* m_UITabletRetina;
};

#endif // __CONTROLSUI_H__

