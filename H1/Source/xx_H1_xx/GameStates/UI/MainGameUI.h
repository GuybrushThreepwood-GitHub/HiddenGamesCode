

/*===================================================================
	File: MainGameUI.h
	Game: AirCadets

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
class LevelCompleteUI;

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

		// HUD
		void SetCountdown( int number );
		void SetGameTime( float timer );
		void SetGamePoints( const char* points );
		void SetSpeedLever( float val );
		void SetCountdownState( bool state );

		// PAUSE
		void SetShowTokenTime( bool state );
		void SetTokenCountdown( float time );
		void SetTokenCounts( int taku, int mei, int earl, int fio, int mito, int uki );
		void SetTotalFlightTime( float time );

		void DoLevelComplete();

		bool IsPaused()							{ return m_bShowPauseOverlay; }
		bool IsLevelComplete()					{ return m_bShowLevelComplete; }

	protected:
		float m_LastDeltaTime;
		Player* m_Player;
		ProfileManager::Profile* m_pProfile;

		ControlsUI* m_pControlsUI;
		IconsUI* m_pIconsUI;
		HudUI* m_pHUDUI;
		PauseUI* m_pPauseUI;
		LevelCompleteUI* m_pLevelCompleteUI;

// pause overlay
		bool m_bShowPauseOverlay;
		bool m_bShowLevelComplete;

		// respawn
		bool m_RespawnFadeToBlack;
		bool m_RespawnFadeToGame;
		int m_RespawnFade;
};

#endif // __MAINGAMEUI_H__

