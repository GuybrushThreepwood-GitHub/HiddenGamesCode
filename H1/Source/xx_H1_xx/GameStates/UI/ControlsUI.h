

/*===================================================================
	File: ControlsUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __CONTROLSUI_H__
#define __CONTROLSUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"
#include "GameStates/UI/ControlsTabletSDUI.h"
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

		bool DivePress();
		bool LiftPress();
		bool LeftPress();
		bool RightPress();
		bool ShootPress();

		const math::Vec2& AnalogueValues();

		void SetupControls( bool tilt );
		void SetSpeedLever( float val );

	protected:
		ControlsTabletSDUI* m_UITabletSD;
};

#endif // __CONTROLSUI_H__

