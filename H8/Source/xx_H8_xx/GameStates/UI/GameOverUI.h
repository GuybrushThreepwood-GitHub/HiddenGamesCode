
/*===================================================================
	File: GameOverUI.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMEOVERUI_H__
#define __GAMEOVERUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class GameOverUI : public UIBaseState, public IBaseGameState
{
	public:
		GameOverUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~GameOverUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void SetGameTime( float time );
		void SetGameScore( unsigned long long int score );
		void SetGameLevel( int level );

		bool Quit()					{ return m_Quit; }
		bool Restart()				{ return m_Restart; }

		void RefreshSelected();
	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		bool m_Quit;
		bool m_Restart;
};

#endif // __GAMEOVERUI_H__

