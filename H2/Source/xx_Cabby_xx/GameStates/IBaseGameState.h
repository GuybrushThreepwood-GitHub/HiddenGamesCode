
#ifndef __IBASEGAMESTATE_H__
#define __IBASEGAMESTATE_H__

#ifndef __ISTATE_H__
#include "StateManage/IState.h"
#endif // __ISTATE_H__

#ifndef __STATEMANAGER_H__
#include "StateManage/StateManager.h"
#endif // __STATEMANAGER_H__

#ifndef __INPUTSYSTEM_H__
#include "InputSystem/InputSystem.h"
#endif // __INPUTSYSTEM_H__

class IBaseGameState : public IState
{
	public:
		IBaseGameState( StateManager& stateManager, InputSystem& inputSystem, int stateId=0 )
			: IState( stateManager, stateId )
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
