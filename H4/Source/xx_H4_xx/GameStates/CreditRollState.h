
/*===================================================================
	File: EndingState.h
	Game:

	(C)Hidden Games
=====================================================================*/

#ifndef __CREDITROLLSTATE_H__
#define __CREDITROLLSTATE_H__

#ifdef USE_CREDITROLLSTATE

#include "GameStates/UI/UIFileLoader.h"

#include "GameSystems.h"

// forward declare
class IState;
class IBaseGameState;

class CreditRollState : public IBaseGameState
{	
	private:
		// load states
		enum LoadState
		{
			LoadState_None=-1,
			LoadState_TransitionIn=0,
			LoadState_TransitionOut
		};

	public:
		CreditRollState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~CreditRollState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		bool ScreenTap();
		bool UpdateLoadState();

	private:
		float m_LastDelta;

		LoadState m_LoadTransitionState;
		int m_TransitionAlpha;

		mdl::ModelHGM* m_FrontendMesh;
		int m_TitleFadeAlpha;
		bool m_ShakeText;

		renderer::FreetypeFont* m_pFont12;
		renderer::FreetypeFont* m_pFont16;

		struct CreditLine
		{
			int fontIndex;

			math::Vec2 pos;
			ui::CUIStaticText creditText;
		};
		
		int m_ScreenTaps;

		int m_TotalStrings;
		CreditLine* m_pCredits;

		float m_RollY;

		// text movement
		float m_MovementTimer;
		math::Vec3 m_CurrentMovement;
		int m_Alpha;
};

#endif // USE_CREDITROLLSTATE

#endif // __CREDITROLLSTATE_H__

