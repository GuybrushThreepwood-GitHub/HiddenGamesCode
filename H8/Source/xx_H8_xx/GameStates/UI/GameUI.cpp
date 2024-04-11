
/*===================================================================
	File: GameUI.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "H8Consts.h"
#include "H8.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/GameUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/MainMenuUI.h"
#include "GameStates/UI/PauseMenuUI.h"
#include "GameStates/UI/GameOverUI.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/MainGameState.h"

#include "GameStates/UI/GeneratedFiles/hud.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameUI::GameUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_GAME )
{
	m_MainMenu = new MainMenuUI( stateManager, inputSystem );
	DBG_ASSERT( m_MainMenu != 0 );

	m_PauseMenu = new PauseMenuUI( stateManager, inputSystem );
	DBG_ASSERT( m_PauseMenu != 0 );

	m_GameOver = new GameOverUI( stateManager, inputSystem );
	DBG_ASSERT( m_GameOver != 0 );

	m_InMainMenu = true;
	m_InGame = false;
	m_IsPaused = false;
	m_IsGameOver = false;

	m_GameTime = 0.0f;
	m_GameLevel = 0;
	m_DownloadTime = 0.0f;

	m_HudMesh = 0;

	GameSystems::GetInstance()->SetGameUI( this );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameUI::~GameUI()
{
	GameSystems::GetInstance()->SetGameUI( 0 );

	if( m_MainMenu != 0 )
	{
		delete m_MainMenu;
		m_MainMenu = 0;
	}

	if( m_PauseMenu != 0 )
	{
		delete m_PauseMenu;
		m_PauseMenu = 0;
	}

	if( m_GameOver != 0 )
	{
		delete m_GameOver;
		m_GameOver = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::Enter()
{
	m_GameData = GetScriptDataHolder()->GetGameData();

	if( m_MainMenu != 0 )
		m_MainMenu->Enter();

	if( m_PauseMenu != 0 )
		m_PauseMenu->Enter();

	if( m_GameOver != 0 )
		m_GameOver->Enter();

	m_HudMesh = res::LoadModel( 10004 );
	DBG_ASSERT( m_HudMesh != 0 );

	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_HudElements.Load( "assets/ui/hud.hui", srcDims, m_HudMesh );

	m_DownloadBarPos = m_HudElements.GetElementPositionNonAABB(TD_PERCENTAGE);
	m_DownloadBarDims = math::Vec2( m_GameData.DOWNLOADBAR_MAX_WIDTH, m_GameData.DOWNLOADBAR_MAX_HEIGHT );
		
	m_ConsoleUpdateTimer = 0.0f;

	m_InstructionsFadeOutTime = m_GameData.INSTRUCTIONS_FADE_TIME;
	m_InstructionsFadeOut = 255;

	m_InstructionColour = m_HudElements.GetElementTextColour(TS_INSTRUCTIONS);

	if (core::app::IstvOS())
	{
		m_HudElements.ChangeElementDrawState(TS_PAUSE, false);
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::Exit()
{
	if( m_HudMesh != 0 )
	{
		res::RemoveModel( m_HudMesh );
		m_HudMesh = 0;
	}

	if( m_MainMenu != 0 )
		m_MainMenu->Exit();

	if( m_PauseMenu != 0 )
		m_PauseMenu->Exit();

	if( m_GameOver != 0 )
		m_GameOver->Exit();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int GameUI::TransitionIn()
{
	if( !m_FinishedTransitionIn )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition -= 15;
		if( m_FadeTransition <= 0 )
		{
			m_FadeTransition = 0;
			m_FinishedTransitionIn = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}

	// transition finished, make sure to do a clean draw
	Draw();
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int GameUI::TransitionOut()
{
	if( !m_FinishedTransitionOut )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition += 15;
		if( m_FadeTransition > 255 )
		{
			m_FadeTransition = 255;
			m_FinishedTransitionOut = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void GameUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	m_ConsoleUpdateTimer -= deltaTime;
	if( m_ConsoleUpdateTimer < 0.0f )
	{
		m_ConsoleUpdateTimer = 0.0f;
		UpdateConsole();
	}

	if( m_MainMenu != 0 &&
		m_InMainMenu )
	{
		m_MainMenu->Update(deltaTime);

		if( m_MainMenu->StartGame() )
		{
			input::ClearAllTouches();

			m_InMainMenu = false;
			m_InGame = true;

			m_GameTime = 0.0f;
			m_GameLevel = 0;
			m_DownloadTime = 0.0f;

			SetGameLevel( m_GameLevel );
			SetGameTime( m_GameTime );

			GameSystems::GetInstance()->ResetLevelCounter();

			ClearConsole();

			MainGameState* pGameState = reinterpret_cast<MainGameState*>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );
			if( pGameState != 0 )
			{
				pGameState->ChangeGameState( MainGameState::GameState_Game );
			}

			math::Vec3 posVector( 0.0f, 0.0f, m_GameData.CAMERA_POS_GAME_Z );
			GameSystems::GetInstance()->GetGameCamera()->SetPosition( posVector );
		}
	}

	if( m_InGame &&
		!m_IsGameOver &&
		!m_IsPaused )
	{
		// new
		if (m_InputSystem.GetUIUp())
		{
			m_HudElements.SelectPreviousRow();
		}
		else
		if (m_InputSystem.GetUIDown())
		{
			m_HudElements.SelectNextRow();
		}
		else
		if (m_InputSystem.GetUILeft())
		{
			m_HudElements.SelectPreviousColumn();
		}
		else
		if (m_InputSystem.GetUIRight())
		{
			m_HudElements.SelectNextColumn();
		}

		if (m_InputSystem.GetUISelect())
			m_HudElements.SelectionPress(true);
		else
			m_HudElements.SelectionPress(false);

		m_HudElements.Update( TOUCH_SIZE_GAME, 2, deltaTime );

		if( m_InstructionsFadeOutTime > 0.0f )
		{
			m_InstructionsFadeOutTime -= deltaTime;
			if( m_InstructionsFadeOutTime < 0.0f )
			{
				m_InstructionsFadeOutTime = 0.0f;
				HideInstructions();
			}
			else
			{
				float div = 255.0f / m_GameData.INSTRUCTIONS_FADE_TIME;

				m_InstructionsFadeOut = static_cast<int>(div*m_InstructionsFadeOutTime); 
				if( m_InstructionsFadeOut < 0 )
					m_InstructionsFadeOut = 0;

				math::Vec4Lite col = m_HudElements.GetElementTextColour(TS_INSTRUCTIONS);
				col.A = m_InstructionsFadeOut;
				m_HudElements.ChangeElementTextColour( TS_INSTRUCTIONS, col );
			}
		}

		if( !m_IsPaused &&
			GameSystems::GetInstance()->GetPlayer().GetAnimState() == Player::AnimState_InGame )
		{
			m_GameTime += deltaTime;
			SetGameTime( m_GameTime );

			m_DownloadTime += deltaTime;
			if( m_DownloadTime > m_GameData.DOWNLOADBAR_TIME )
			{
				m_DownloadTime = 0.0f;
				GameSystems::GetInstance()->AddToScore( m_GameData.DOWNLOADBAR_POINT_BONUS );

				GameSystems::GetInstance()->AddTextToConsole( "Patch download complete....." );
				GameSystems::GetInstance()->AddTextToConsole( "bonus %d", m_GameData.DOWNLOADBAR_POINT_BONUS );
				GameSystems::GetInstance()->AddTextToConsole( "....searching for next file......" );
				GameSystems::GetInstance()->AddTextToConsole( "..file found..." );
				GameSystems::GetInstance()->AddTextToConsole( "download started" );
			}
		}

		if( m_HudElements.CheckElementForSingleTouch(TS_PAUSE) || 
			m_InputSystem.GetPause() || 
			core::app::IsInBackground() ||
           m_InputSystem.GetMenu() )
		{
			m_IsPaused = true;
			input::ClearAllTouches();
			m_PauseMenu->RefreshSelected();
            core::app::SetHandleMenuButton(true);
		}
		else
		{
			m_InputSystem.SetLeft(false);
			m_InputSystem.SetRight(false);
			m_InputSystem.SetUp(false);
			m_InputSystem.SetDown(false);

			if( input::gInputState.TouchesData[input::FIRST_TOUCH].bPress ||
				input::gInputState.TouchesData[input::FIRST_TOUCH].bHeld )
			{
				if( input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta > 2.0f )
				{
					m_InputSystem.SetLeft(true);
					m_InputSystem.SetRight(false);
				}
				else if(input::gInputState.TouchesData[input::FIRST_TOUCH].nXDelta < -2.0f )
				{
					m_InputSystem.SetLeft(false);
					m_InputSystem.SetRight(true);
				}

				/*if( input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta > 2.0f )
				{				
					m_InputSystem.SetUp(true);
					m_InputSystem.SetDown(false);				
				}
				else if(input::gInputState.TouchesData[input::FIRST_TOUCH].nYDelta < -2.0f )
				{
					m_InputSystem.SetUp(false);
					m_InputSystem.SetDown(true);
				}*/
			}
		}
	}

	if( m_PauseMenu != 0 )
	{
		if( m_IsPaused )
		{
			m_PauseMenu->Update(deltaTime);

			if( m_PauseMenu->Resume() )
            {
				m_IsPaused = false;
                
                core::app::SetHandleMenuButton(true);
            }
			else
			if( m_PauseMenu->Restart() )
			{
				m_IsPaused = false;
				m_InMainMenu = false;
				m_InGame = true;

				m_GameTime = 0.0f;
				m_GameLevel = 0;
				m_DownloadTime = 0.0f;

				SetGameLevel( m_GameLevel );
				SetGameTime( m_GameTime );

				GameSystems::GetInstance()->ResetLevelCounter();

				// clear console
				ClearConsole();

				MainGameState* pGameState = reinterpret_cast<MainGameState*>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );
				if( pGameState != 0 )
					pGameState->Restart();

				ShowInstructions();
                
                core::app::SetHandleMenuButton(true);
			}
			else
			if( m_PauseMenu->Quit() ||
               m_InputSystem.GetMenu() )
			{
				m_IsPaused = false;
				m_InMainMenu = true;
				m_InGame = false;

				m_GameTime = 0.0f;
				m_GameLevel = 0;
				m_DownloadTime = 0.0f;

				SetGameLevel( m_GameLevel );
				SetGameTime( m_GameTime );

				GameSystems::GetInstance()->ResetLevelCounter();

				m_MainMenu->RefreshSelected();

				// clear console
				ClearConsole();

				MainGameState* pGameState = reinterpret_cast<MainGameState*>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );
				if( pGameState != 0 )
					pGameState->ChangeGameState( MainGameState::GameState_GameStartWait );

				math::Vec3 posVector( 0.0f, 0.0f, m_GameData.CAMERA_POS_TITLE_Z );
				GameSystems::GetInstance()->GetGameCamera()->SetPosition( posVector );
                
                core::app::SetHandleMenuButton(false);
			}
		}
	}

	if( m_GameOver != 0 )
	{
		if( m_IsGameOver )
		{
			m_GameOver->Update(deltaTime);

			if( m_GameOver->Restart() )
			{
				m_IsPaused = false;
				m_InMainMenu = false;
				m_InGame = true;
				m_IsGameOver = false;

				m_GameTime = 0.0f;
				m_GameLevel = 0;
				m_DownloadTime = 0.0f;

				SetGameLevel( m_GameLevel );
				SetGameTime( m_GameTime );

				GameSystems::GetInstance()->ResetLevelCounter();

				// clear console
				ClearConsole();

				MainGameState* pGameState = reinterpret_cast<MainGameState*>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );
				if( pGameState != 0 )
				{
					pGameState->Restart();
					pGameState->ChangeGameState( MainGameState::GameState_Game );
				}

				ShowInstructions();
                
                core::app::SetHandleMenuButton(true);
			}
			else
			if( m_GameOver->Quit() ||
               m_InputSystem.GetMenu() )
			{
				m_IsPaused = false;
				m_InMainMenu = true;
				m_InGame = false;
				m_IsGameOver = false;

				m_GameTime = 0.0f;
				m_GameLevel = 0;
				m_DownloadTime = 0.0f;

				SetGameLevel( m_GameLevel );
				SetGameTime( m_GameTime );

				GameSystems::GetInstance()->ResetLevelCounter();

                m_MainMenu->RefreshSelected();
                
				// clear console
				ClearConsole();

				MainGameState* pGameState = reinterpret_cast<MainGameState*>( m_pStateManager->GetSecondaryStateManager()->GetCurrentState() );
				if( pGameState != 0 )
					pGameState->ChangeGameState( MainGameState::GameState_GameStartWait );

				math::Vec3 posVector( 0.0f, 0.0f, m_GameData.CAMERA_POS_TITLE_Z );
				GameSystems::GetInstance()->GetGameCamera()->SetPosition( posVector );
                
                core::app::SetHandleMenuButton(false);
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::Draw()
{
	if( m_InMainMenu )
	{
		if( m_MainMenu != 0 )
			m_MainMenu->Draw();
	}

	if( m_IsGameOver )
	{
		if( m_GameOver != 0 )
			m_GameOver->Draw();
	}

	if( m_InGame && 
		!m_IsGameOver &&
		!m_IsPaused )
	{
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		renderer::OpenGL::GetInstance()->DepthMode(false, GL_ALWAYS);

		if( m_HudMesh != 0 )
			m_HudMesh->Draw();

		m_HudElements.Draw();

		renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
		DrawDownloadBar( m_DownloadBarPos, m_DownloadBarDims, m_DownloadTime, m_GameData.DOWNLOADBAR_TIME, m_HudElements, TD_PERCENTAGE );
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	}

	if( m_IsPaused )
	{
		if( m_PauseMenu != 0 )
			m_PauseMenu->Draw();
	}

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: SetGameOver
/// Params: [in]state
///
/////////////////////////////////////////////////////
void GameUI::SetGameOver( bool state )				
{
	if( m_IsGameOver == state )
		return;
	
	m_IsGameOver = state; 

	if( m_IsGameOver )
	{
		if( m_GameOver != 0 )
		{
			m_GameOver->RefreshSelected();

			m_GameOver->SetGameLevel( GameSystems::GetInstance()->GetLevelCounter() );
			m_GameOver->SetGameTime( m_GameTime );
			m_GameOver->SetGameScore( GameSystems::GetInstance()->GetCurrentScore() );
			
			// save and sync
			ProfileManager::GetInstance()->GetProfile()->personalBest = GameSystems::GetInstance()->GetPersonalBest();
			ProfileManager::GetInstance()->SaveProfile();
			
			// current score
			support::Scores::GetInstance()->SendScore( GameSystems::GetInstance()->GetCurrentScore(), "uk.co.hiddengames.firewall.hiscores");
				
		}
	}
}

/////////////////////////////////////////////////////
/// Method: SetCurrentScore
/// Params: [in]score
///
/////////////////////////////////////////////////////
void GameUI::SetCurrentScore( unsigned long long int score )
{
	SetScore( m_HudElements, TD_CURRENTSCORE, score );
}

/////////////////////////////////////////////////////
/// Method: SetCurrentScore
/// Params: [in]score
///
/////////////////////////////////////////////////////
void GameUI::SetPersonalBest( unsigned long long int score )
{
	SetScore( m_HudElements, TD_BESTSCORE, score );
}

/////////////////////////////////////////////////////
/// Method: SetGameLevel
/// Params: [in]level
///
/////////////////////////////////////////////////////
void GameUI::SetGameLevel( int level )
{
	m_HudElements.ChangeElementText( TD_CURRENTLEVEL, "%s %d", res::GetScriptString(206), level );
}

/////////////////////////////////////////////////////
/// Method: SetGameTime
/// Params: [in]time
///
/////////////////////////////////////////////////////
void GameUI::SetGameTime( float time )
{
	SetTime( m_HudElements, TD_TIME, time );
	const char* elementText = m_HudElements.GetElementText(TD_TIME);

	m_HudElements.ChangeElementText( TD_TIME, "%s %s", res::GetScriptString(207), elementText );
}

/////////////////////////////////////////////////////
/// Method: AddTextToConsole
/// Params: [in]text
///
/////////////////////////////////////////////////////
void GameUI::AddTextToConsole( const char* text )
{
	// store it
	m_StoredConsoleStrings.push( std::string(text) );
}

/////////////////////////////////////////////////////
/// Method: UpdateConsole
/// Params: [in]text
///
/////////////////////////////////////////////////////
void GameUI::UpdateConsole()
{
	int j=0;

	if( m_IsPaused || 
		m_IsGameOver ||
		!m_InGame )
		return;

	if( !m_StoredConsoleStrings.empty() )
	{
		std::string newOutput = m_StoredConsoleStrings.front();

		// swap from top 
		for( j=4; j >= 1; --j )
		{
			if( m_HudElements.GetElementText( TD_CONSOLE1+(j-1) ) != 0 )
				m_HudElements.ChangeElementText( TD_CONSOLE1+j, m_HudElements.GetElementText( TD_CONSOLE1+(j-1) ) );
		}

		// top line
		m_HudElements.ChangeElementText( TD_CONSOLE1, newOutput.c_str() );

		ALuint bufferId = AudioSystem::GetInstance()->AddAudioFile( 101 );

		if( bufferId != snd::INVALID_SOUNDBUFFER )
		{
			AudioSystem::GetInstance()->PlayAudio( bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 2.0f, 0.2f );
		}

		// remove
		m_StoredConsoleStrings.pop();
	}
	else
	{
		// swap from top 
		for( j=4; j >= 1; --j )
		{
			if( m_HudElements.GetElementText( TD_CONSOLE1+(j-1) ) !=0 )
			{
				m_HudElements.ChangeElementText( TD_CONSOLE1+j, m_HudElements.GetElementText( TD_CONSOLE1+(j-1) ) );
			}
		}

		if( strcmp( m_HudElements.GetElementText( TD_CONSOLE5 ), "" ) != 0 )
		{
			ALuint bufferId = AudioSystem::GetInstance()->AddAudioFile( 101 );

			if( bufferId != snd::INVALID_SOUNDBUFFER )
			{
				AudioSystem::GetInstance()->PlayAudio( bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE, 2.0f, 0.2f );
			}
		}

		// top line
		m_HudElements.ChangeElementText( TD_CONSOLE1, "" );

	/*if( numToAdd > 8 )
		{
			int inserts = math::RandomNumber(1, 3);

			for( i=0; i < inserts; i++ )
				GameSystems::GetInstance()->AddTextToConsole( res::GetScriptString( math::RandomNumber(1000, 1030) ) );
		}*/
	}
	
	m_ConsoleUpdateTimer = 0.35f;
}

/////////////////////////////////////////////////////
/// Method: ClearConsole
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::ClearConsole()
{
	int i=0;

	while( !m_StoredConsoleStrings.empty() )
	{
		// remove
		m_StoredConsoleStrings.pop();
	}

	for( i=0; i < 5; i++ )
		m_HudElements.ChangeElementText( TD_CONSOLE1+i, "" );
}

/////////////////////////////////////////////////////
/// Method: ShowInstructions
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::ShowInstructions()
{
	m_InstructionColour.A = m_InstructionsFadeOut;
	m_InstructionsFadeOutTime = m_GameData.INSTRUCTIONS_FADE_TIME;

	m_HudElements.ChangeElementTextColour( TS_INSTRUCTIONS, m_InstructionColour );
	m_HudElements.ChangeElementDrawState( TS_INSTRUCTIONS, true );
}
		
/////////////////////////////////////////////////////
/// Method: HideInstructions
/// Params: None
///
/////////////////////////////////////////////////////
void GameUI::HideInstructions()
{
	m_InstructionsFadeOutTime = 0.0f;
	m_InstructionsFadeOut = 255;
	m_InstructionColour.A = 255;
	m_HudElements.ChangeElementTextColour( TS_INSTRUCTIONS, m_InstructionColour );
	m_HudElements.ChangeElementDrawState( TS_INSTRUCTIONS, false );
}
