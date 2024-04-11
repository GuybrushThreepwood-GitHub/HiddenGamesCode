
#ifndef __IBASEGAMESTATE_H__
#define __IBASEGAMESTATE_H__

#ifndef __ISTATE_H__
#include "State/IState.h"
#endif // __ISTATE_H__

#include "StateManage/StateManager.h"
#include "InputSystem/InputSystem.h"

class IBaseGameState : public IState
{
	public:
		IBaseGameState( StateManager& stateManager, InputSystem& inputSystem )
			: IState( stateManager )
			, m_InputSystem(inputSystem)
		{
			input::InitialiseInput();
		}

		virtual ~IBaseGameState()
		{}

	protected:
		InputSystem& m_InputSystem;
};

#endif // __IBASEGAMESTATE_H__
