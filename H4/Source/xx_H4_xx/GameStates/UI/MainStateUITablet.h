

/*===================================================================
	File: MainStateUITablet.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINSTATEUITABLET_H__
#define __MAINSTATEUITABLET_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/MainStateUI.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class MainStateUITablet : public MainStateUI
{
	public:
		MainStateUITablet( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~MainStateUITablet();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		virtual void SetLevelName( const char* name );
		virtual void ClearSay();

		virtual bool IsPaused()								{ return m_Paused; }
		virtual bool HasQuit()								{ return m_Quit; }
		virtual void SetDeathContinueFlag( bool state )		{ m_ContinueFromDeath = state; }

	protected:
		virtual void UpdateLeftGameUI( float deltaTime );
		virtual void UpdateRightGameUI( float deltaTime );
		virtual void UpdatePauseUI( float deltaTime );

		virtual void UpdateType1( float deltaTime );

		virtual void UpdateType4( float deltaTime );

		virtual void UpdateOptions();
};

#endif // __MAINSTATEUITABLET_H__

