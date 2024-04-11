
/*===================================================================
	File: IState.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __ISTATE_H__
#define __ISTATE_H__

// forward declare
#ifndef __STATEMANAGER_H__
	#include "StateManage/StateManager.h"
#endif // __STATEMANAGER_H__

class IState
{
	public:
		IState( StateManager& stateManager, int stateId=0 ) 
		{
			m_pStateManager = &stateManager;

			m_LastDeltaTime = 0.0f;
			m_pForwardState = 0;
			m_pBackState = 0;

			m_Id = stateId;
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

		int GetId()							{ return m_Id; }

	protected:
		float m_LastDeltaTime;
		StateManager* m_pStateManager;

		IState* m_pForwardState;
		IState* m_pBackState;

		int m_Id;
};

#endif // __ISTATE_H__