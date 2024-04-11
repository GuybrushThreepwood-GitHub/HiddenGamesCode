
/*===================================================================
	File: TitleScreenState.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __TITLESCREENSTATE_H__
#define __TITLESCREENSTATE_H__

#include "ScriptAccess/ScriptDataHolder.h"

// forward declare
class IState;
class IBaseGameState;

class TitleScreenState : public IBaseGameState
{
	public:
		TitleScreenState( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~TitleScreenState();

		virtual void Enter();
		virtual void Exit();
		virtual int TransitionIn();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

	private:
		float m_LastDelta;
};

#endif // __TITLESCREENSTATE_H__

