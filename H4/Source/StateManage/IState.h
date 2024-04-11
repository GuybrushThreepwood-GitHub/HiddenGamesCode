
/*===================================================================
	File: IState.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __ISTATE_H__
#define __ISTATE_H__

// forward declare
#include "StateManage/StateManager.h"

class IState
{
	public:
		IState( StateManager& stateManager ) 
		{
			m_pStateManager = &stateManager;

			m_LastDeltaTime = 0.0f;
			m_pForwardState = 0;
			m_pBackState = 0;
		}
		virtual ~IState() 
		{
			m_pStateManager = 0;

			m_LastDeltaTime = 0.0f;
			m_pForwardState = 0;
			m_pBackState = 0;
		}

		virtual void Enter() = 0;
		virtual void Exit() = 0;
		virtual void PrepareTransitionIn(){};
		virtual int TransitionIn() = 0;
		virtual void PrepareTransitionOut(){};
		virtual int TransitionOut() = 0;
		
		virtual void Update( float deltaTime ) = 0;
		virtual void Draw() = 0;

		void ChangeState( IState* newState )
		{
			if( m_pStateManager )
				m_pStateManager->ChangeState( newState );
		}
		StateManager* GetStateManager()
		{
			return( m_pStateManager );
		}

	protected:
		float m_LastDeltaTime;
		StateManager* m_pStateManager;

		IState* m_pForwardState;
		IState* m_pBackState;
};

#endif // __ISTATE_H__