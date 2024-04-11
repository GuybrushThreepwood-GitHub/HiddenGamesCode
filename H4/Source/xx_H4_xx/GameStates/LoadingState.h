
/*===================================================================
	File: LoadingState.h
	Game:

	(C)Hidden Games
=====================================================================*/

#ifndef __LOADINGSTATE_H__
#define __LOADINGSTATE_H__

#include "GameStates/UI/UIFileLoader.h"

#include "GameSystems.h"

// forward declare
class IState;
class IBaseGameState;

class LoadingState : public IBaseGameState
{	
	public:
		LoadingState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~LoadingState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		float m_LastDelta;

		UIFileLoader m_LoadingElements;

		bool m_DrawnOnce;

};

#endif // __LOADINGSTATE_H__

