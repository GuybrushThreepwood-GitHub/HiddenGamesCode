
/*===================================================================
	File: StateManager.h
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifndef __STATEMANAGER_H__
#define __STATEMANAGER_H__

// forward declare
class IState;

class StateManager
{
	public:
		StateManager();
		~StateManager();

		void SetSecondaryManager( StateManager* pSecondaryManager );

		void Update( float deltaTime );
		void Draw();

		void ChangeState( IState* newState );
		void ChangeSecondaryState( IState* newState );

		IState* GetCurrentState() { return m_pCurrentState; }
		StateManager* GetSecondaryStateManager();

	private:
		IState* m_pCurrentState;
		StateManager* m_pSecondaryManager;

		bool m_WaitingOnTransitionOut;
		bool m_WaitingOnTransitionIn;
		IState* m_pStoredTransitionState;
};

#endif // __STATEMANAGER_H__

