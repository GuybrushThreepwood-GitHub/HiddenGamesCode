

/*===================================================================
	File: HudUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __HUDUI_H__
#define __HUDUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

class HudUI : public UIBaseState, public IBaseGameState
{
	public:
		HudUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~HudUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool PausePress()							{ return m_Pause; }

		void SetCountdown( int number );
		void SetGameTime( float timer );
		void SetGamePoints( const char* points);

		void DisableScore();
		void DisableTime();
		void SetCountdownState( bool state );

	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;

		GameData m_GameData;
		Player* m_Player;
		ScriptDataHolder::LevelScriptData m_LevelData;

		bool m_Pause;
};

#endif // __HUDUI_H__

