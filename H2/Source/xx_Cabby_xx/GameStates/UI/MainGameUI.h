

/*===================================================================
	File: MainGameUI.h
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#ifndef __MAINGAMEUI_H__
#define __MAINGAMEUI_H__

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "Profiles/ProfileManager.h"
#include "GameStates/UI/UIFileLoader.h"

class ControlsUI;
class IconsUI;
class HudUI;
class PauseUI;
class TimeUpUI;

class MainGameUI : public UIBaseState, public IBaseGameState
{
	public:
		MainGameUI( StateManager& stateManager, InputSystem& inputSystem);
		virtual ~MainGameUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void DoRespawnFadeToBlack();
		bool IsRespawnFadeToBlackComplete();
		bool IsRespawnFadeToGameComplete();
		void ResetRespawn();

		void DoRescueFadeToBlack();
		bool IsRescueFadeToGameComplete();
		bool HasFlyByFinished();

		//void SetLivesCount( int count );
		//void SetCashCount( int count );
		void SetGameTime( float timer );
		
		void DoTimeUp();

		bool IsPaused()							{ return m_bShowPauseOverlay; }
		bool IsTimeUp()							{ return m_bShowTimeUp; }

	protected:
		float m_LastDeltaTime;
		Player* m_Player;
		ProfileManager::Profile* m_pProfile;

		ControlsUI* m_pControlsUI;
		IconsUI* m_pIconsUI;
		HudUI* m_pHUDUI;
		PauseUI* m_pPauseUI;
		TimeUpUI* m_pTimeUpUI;

// pause overlay
		bool m_bShowPauseOverlay;
		bool m_bShowTimeUp;

		// respawn
		bool m_RespawnFadeToBlack;
		bool m_RespawnFadeToGame;
		int m_RespawnFade;

		bool m_FinishedTowTruckFlyBy;
		bool m_FirstCostDraw;
		bool m_RescueFadeToBlack;
		bool m_RescueFadeToGame;
		int m_RescueFade;
};

#endif // __MAINGAMEUI_H__

