
/*===================================================================
	File: GameUI.h
	Game: H8

	(C)Hidden Games
=====================================================================*/

#ifndef __GAMEUI_H__
#define __GAMEUI_H__

#include <queue>

// forward declare
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/IBaseGameState.h"

#include "GameStates/UI/UIFileLoader.h"

// forward declare
class MainMenuUI;
class PauseMenuUI;
class GameOverUI;

class GameUI : public UIBaseState, public IBaseGameState
{
	public:
		GameUI( StateManager& stateManager, InputSystem& inputSystem );
		virtual ~GameUI();

		virtual void Enter();
		virtual void Exit();
		virtual void PrepareTransitionIn();
		virtual int TransitionIn();
		virtual void PrepareTransitionOut();
		virtual int TransitionOut();

		virtual void Update( float deltaTime );
		virtual void Draw();

		void SetCurrentScore( unsigned long long int score );
		void SetPersonalBest( unsigned long long int score );
		void SetGameLevel( int level );
		void SetGameTime( float time );

		bool IsPaused()								{ return m_IsPaused; }
		void SetGameOver( bool state );

		void ShowInstructions();
		void HideInstructions();

		void AddTextToConsole( const char* text );

	private:
		void UpdateConsole();
		void ClearConsole();

	protected:
		float m_LastDeltaTime;

		GameData m_GameData;

		MainMenuUI* m_MainMenu;
		PauseMenuUI* m_PauseMenu;
		GameOverUI* m_GameOver;
		
		mdl::ModelHGM* m_HudMesh;
		UIFileLoader m_HudElements;

		bool m_InMainMenu;
		bool m_InGame;
		bool m_IsPaused;
		bool m_IsGameOver;

		float m_GameTime;
		int m_GameLevel;

		float m_DownloadTime;

		math::Vec4Lite m_InstructionColour;
		float m_InstructionsFadeOutTime;
		int m_InstructionsFadeOut;

		math::Vec3 m_DownloadBarPos;
		math::Vec2 m_DownloadBarDims;

		float m_ConsoleUpdateTimer;
		std::queue<std::string> m_StoredConsoleStrings;
};

#endif // __GAMEUI_H__

