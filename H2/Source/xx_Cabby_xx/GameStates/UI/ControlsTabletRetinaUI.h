

/*===================================================================
	File: ControlsTabletRetinaUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __CONTROLSTABLETRETINAUI_H__
#define __CONTROLSTABLETRETINAUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class ControlsTabletRetinaUI : public UIBaseState, public IBaseGameState
{
	public:
		ControlsTabletRetinaUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~ControlsTabletRetinaUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool PausePress()		{ return m_Pause; }
		bool UpPress()			{ return m_UpwardThrust; }
		bool DownPress()		{ return m_DownwardThrust; }
		bool LeftPress()		{ return m_Left; }
		bool RightPress()		{ return m_Right; }
		bool LandingGearPress() { return m_LandingGear; }

		void SetupControls( int controlType );

	protected:
		float m_LastDeltaTime;

		Player* m_Player;
	
		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		bool m_Pause;
		bool m_UpwardThrust;
		bool m_DownwardThrust;

		bool m_Left;
		bool m_Right;
		bool m_LandingGear;

		bool m_TiltControls;
};

#endif // __CONTROLSTABLETRETINAUI_H__

