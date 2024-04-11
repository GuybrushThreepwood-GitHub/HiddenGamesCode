

/*===================================================================
	File: ControlsTabletSDUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __CONTROLSTABLETSDUI_H__
#define __CONTROLSTABLETSDUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class ControlsTabletSDUI : public UIBaseState, public IBaseGameState
{
	public:
		ControlsTabletSDUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~ControlsTabletSDUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool DivePress()		{ return m_Dive; }
		bool LiftPress()		{ return m_Lift; }
		bool LeftPress()		{ return m_Left; }
		bool RightPress()		{ return m_Right; }
		bool ShootPress()		{ return m_Shoot; }
		const math::Vec2&		AnalogueValues();

		void SetupControls( bool tilt );
		void SetSpeedLever( float percent );

	protected:
		float m_LastDeltaTime;

		Player* m_Player;
		GameData m_GameData;
	
		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		math::Vec3 m_LeverStartPos;
		math::Vec2 m_LeverMaxMin;
		float m_LeverRange;

		float m_AnalogueRadius;
		math::Vec3 m_AnalogueCenter;
		int m_AnalogueTouchIndex;

		math::Vec2 m_AnalogueVales;

		bool m_Dive;
		bool m_Lift;

		bool m_Left;
		bool m_Right;
		bool m_Shoot;

		bool m_TiltControls;

		bool m_PlacedAnalogue;
};

#endif // __CONTROLSTABLETSDUI_H__

