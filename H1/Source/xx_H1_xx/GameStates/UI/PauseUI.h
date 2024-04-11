

/*===================================================================
	File: PauseUI.h
	Game: AirCadets

	(C)Hidden Games
=====================================================================*/

#ifndef __PAUSEUI_H__
#define __PAUSEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "Profiles/ProfileManager.h"
#include "GameStates/UI/UIFileLoader.h"

class PauseUI : public UIBaseState, public IBaseGameState
{
	public:
		PauseUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~PauseUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		bool QuitPress()		{ return m_QuitPress; }
		bool RestartPress()		{ return m_RestartPress; }
		bool ContinuePress()	{ return m_ContinuePress; }
		bool ControlChange()	{ return m_ControlChange; }

		void SetShowTokenTime( bool state )					{ m_ShowTokenTime = state; }

		void SetTokenCountdown( float time );
		void SetTokenCounts( int taku, int mei, int earl, int fio, int mito, int uki );
		void SetTotalFlightTime( float time );

        void RefreshSelected();
	protected:
		float m_LastDeltaTime;

		mdl::ModelHGM* m_UIMesh;
		UIFileLoader m_Elements;
		ProfileManager::Profile* m_pProfile;

		bool m_QuitPress;
		bool m_RestartPress;
		bool m_ContinuePress;
		bool m_ControlChange;

		bool m_ShowTokenTime;
		float m_TokenCountdown;

		int m_TakuCount;
		int m_MeiCount;
		int m_EarlCount;
		int m_FioCount;
		int m_MitoCount;
		int m_UkiCount;
};

#endif // __PAUSEUI_H__

