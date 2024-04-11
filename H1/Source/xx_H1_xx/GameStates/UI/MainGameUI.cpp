
/*===================================================================
	File: HudUI.cpp
	Game: AirCadets

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

#include "H1Consts.h"
#include "H1.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/UI/ControlsUI.h"
#include "GameStates/UI/HudUI.h"
#include "GameStates/UI/IconsUI.h"
#include "GameStates/UI/PauseUI.h"
#include "GameStates/UI/LevelCompleteUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendMainMenuUI.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/MainGameState.h"

#include "GameStates/UI/GeneratedFiles/hud.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud.hui.h"

namespace
{
	input::Input debugInput;
	bool drawGameControlsUI = true;
	bool drawGameUI = true;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MainGameUI::MainGameUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_MAINGAME )
{
	m_Player = 0;//PhysicsWorld::GetPlayer();

	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	
	m_pControlsUI = new ControlsUI( stateManager, inputSystem );
	DBG_ASSERT( m_pControlsUI != 0 );

	m_pIconsUI = 0;
	if( GameSystems::GetInstance()->GetGameMode() == GameSystems::GAMEMODE_TARGET )
	{
		m_pIconsUI = new IconsUI( stateManager, inputSystem );
		DBG_ASSERT( m_pIconsUI != 0 );
	}

	m_pHUDUI = new HudUI( stateManager, inputSystem );
	DBG_ASSERT( m_pHUDUI != 0 );

	m_pPauseUI = new PauseUI( stateManager, inputSystem );
	DBG_ASSERT( m_pPauseUI != 0 );

	m_pLevelCompleteUI = new LevelCompleteUI( stateManager, inputSystem );
	DBG_ASSERT( m_pLevelCompleteUI != 0 );

	m_bShowPauseOverlay = false;
	m_bShowLevelComplete = false;

	m_RespawnFadeToBlack = false;
	m_RespawnFadeToGame = false;
	m_RespawnFade = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainGameUI::~MainGameUI()
{
	if( m_pControlsUI != 0 )
		delete m_pControlsUI;

	if( m_pIconsUI != 0 )
		delete m_pIconsUI;

	if( m_pHUDUI != 0 )
		delete m_pHUDUI;

	if( m_pPauseUI != 0 )
		delete m_pPauseUI;

	if( m_pLevelCompleteUI != 0 )
		delete m_pLevelCompleteUI;
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::Enter()
{
	if( m_pControlsUI != 0 )
	{
		m_pControlsUI->Enter();
		m_pControlsUI->SetupControls( true );
	}

	if( m_pIconsUI != 0 )
		m_pIconsUI->Enter();

	if( m_pHUDUI != 0 )
		m_pHUDUI->Enter();

	if( m_pPauseUI != 0 )
		m_pPauseUI->Enter();

	if( m_pLevelCompleteUI != 0 )
		m_pLevelCompleteUI->Enter();
	
	if( !m_pProfile->adsRemoved )
	{
		core::app::SetAdBarState( true );
		#ifdef BASE_PLATFORM_WINDOWS
			core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
		#endif // BASE_PLATFORM_WINDOWS
	}

	if (core::app::IstvOS())
		drawGameControlsUI = false;
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::Exit()
{
	if( m_pControlsUI != 0 )
		m_pControlsUI->Exit();

	if( m_pIconsUI != 0 )
		m_pIconsUI->Exit();

	if( m_pHUDUI != 0 )
		m_pHUDUI->Exit();

	if( m_pPauseUI != 0 )
		m_pPauseUI->Exit();

	if( m_pLevelCompleteUI != 0 )
		m_pLevelCompleteUI->Exit();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int MainGameUI::TransitionIn()
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
int MainGameUI::TransitionOut()
{
	if( !m_FinishedTransitionOut )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition += 15;
		if( m_FadeTransition >= 255 )
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
void MainGameUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	if( debugInput.IsKeyPressed( input::KEY_F1, true, true ) )
	{
		drawGameUI = !drawGameUI;
	}

	if( m_bShowPauseOverlay )
	{
		if( m_pPauseUI != 0 )
			m_pPauseUI->Update( deltaTime );

		m_InputSystem.SetReset(false);

		if( m_pPauseUI->QuitPress() )
		{
			m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
			ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_MainMenu) );
			return;			
		}
		else
		if( m_pPauseUI->ContinuePress() )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_bShowPauseOverlay = false;
		}
		else
		if( m_pPauseUI->RestartPress() )
		{	
			AudioSystem::GetInstance()->PlayUIAudio();

			MainGameState* pMainState = 0;

			m_bShowPauseOverlay = false;
			
			pMainState = static_cast<MainGameState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
			pMainState->ReplayLevel();
		}
	}
	else if( m_bShowLevelComplete )
	{
		if( m_pLevelCompleteUI != 0 )
			m_pLevelCompleteUI->Update( deltaTime );

		if( m_pLevelCompleteUI->ResumeGame() )	
		{
			m_bShowLevelComplete = false;

			if( m_pHUDUI != 0 )
			{
				m_pHUDUI->DisableScore();
				m_pHUDUI->DisableTime();
			}
		}
		else
		if( m_pLevelCompleteUI->ReplayMode() )	
		{
			m_bShowLevelComplete = false;
		}

		/*if( m_pLevelCompleteUI->ContinuePress() )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			
			m_bShowLevelComplete = false;

			ChangeState( new LevelCompleteArcadeUI(*m_pStateManager, m_InputSystem) );
			return;
		}*/
	}
	else
	{
		if( m_pControlsUI != 0 )
		{
            m_pControlsUI->Update(deltaTime);
			
            if (drawGameControlsUI)
			{
            
				if (m_pControlsUI->LeftPress())
					m_InputSystem.SetLeft(true);
				else
					m_InputSystem.SetLeft(false);

				if (m_pControlsUI->RightPress())
					m_InputSystem.SetRight(true);
				else
					m_InputSystem.SetRight(false);

				if (m_pControlsUI->DivePress())
					m_InputSystem.SetDive(true);
				else
					m_InputSystem.SetDive(false);

				if (m_pControlsUI->LiftPress())
					m_InputSystem.SetLift(true);
				else
					m_InputSystem.SetLift(false);

				if (m_pControlsUI->ShootPress())
					m_InputSystem.SetShoot(true);
				else
					m_InputSystem.SetShoot(false);

				m_InputSystem.SetAnalogueValues(m_pControlsUI->AnalogueValues());
			}
		}

		if( m_pIconsUI != 0 )
			m_pIconsUI->Update( deltaTime );

		if( m_pHUDUI != 0 )
		{
			m_pHUDUI->Update( deltaTime );

			if( !m_bShowPauseOverlay &&
				!m_RespawnFadeToBlack &&
				!m_RespawnFadeToGame )
			{
				if( m_pHUDUI->PausePress() ||
					m_InputSystem.GetPause() ||
					core::app::IsInBackground() ||
					m_InputSystem.GetMenu())
				{
					//AudioSystem::GetInstance()->PlayUIAudio();

                    input::ClearAllTouches();
                    m_pPauseUI->RefreshSelected();
                    
					m_InputSystem.SetReset(true);
					m_bShowPauseOverlay = true;
                    
					// sync achievements
					if( support::Scores::IsInitialised() )
						support::Scores::GetInstance()->SyncAchievements();
				}
				else
					m_InputSystem.SetReset(false);
			}
			else 
				m_InputSystem.SetReset(false);
		}
	}

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// draw everything
	if( drawGameUI )
	{
		//if (drawGameControlsUI)
		{
			if (m_pControlsUI != 0)
				m_pControlsUI->Draw();
		}
	}

	if( m_pIconsUI != 0 )
		m_pIconsUI->Draw();
		
	if( m_pHUDUI != 0 )
		m_pHUDUI->Draw();

	// PAUSE OVERLAY
	if( m_bShowPauseOverlay )
	{
		if( drawGameUI )
		{
			if( m_pPauseUI != 0 )
				m_pPauseUI->Draw();
		}
	}

	// LEVEL COMPLETE OVERLAY
	if( m_bShowLevelComplete )
	{
		if( drawGameUI )
		{
			if( m_pLevelCompleteUI != 0 )
				m_pLevelCompleteUI->Draw();
		}
	}

	if( m_RespawnFadeToGame )
	{
		// draw fade
		m_RespawnFade -= 15;
		if( m_RespawnFade <= 0 )
		{
			m_RespawnFade = 0;
			m_RespawnFadeToGame = false;
		}

		DrawFullscreenQuad( 0.0f, m_RespawnFade );
	}

	if( m_RespawnFadeToBlack )
	{
		// draw fade
		m_RespawnFade += 15;
		if( m_RespawnFade >= 255 )
		{
			m_RespawnFade = 255;
			m_RespawnFadeToBlack = false;
			m_RespawnFadeToGame = true;
		}

		DrawFullscreenQuad( 0.0f, m_RespawnFade );
	}
}

/////////////////////////////////////////////////////
/// Method: DoRespawnFadeToBlack
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::DoRespawnFadeToBlack()
{
	m_RespawnFadeToBlack = true;
	m_RespawnFade = 0;
}

/////////////////////////////////////////////////////
/// Method: IsRespawnFadeToBlackComplete
/// Params: None
///
/////////////////////////////////////////////////////
bool MainGameUI::IsRespawnFadeToBlackComplete()
{
	return( !m_RespawnFadeToBlack );
}
	
/////////////////////////////////////////////////////
/// Method: IsRespawnFadeToGameComplete
/// Params: None
///
/////////////////////////////////////////////////////
bool MainGameUI::IsRespawnFadeToGameComplete()
{
	return( !m_RespawnFadeToGame );
}
		
/////////////////////////////////////////////////////
/// Method: ResetRespawn
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::ResetRespawn()
{
	m_RespawnFadeToBlack = false;
	m_RespawnFadeToGame = false;
	m_RespawnFade = 0;
}

/////////////////////////////////////////////////////
/// Method: SetCountdown
/// Params: [in]number
///
/////////////////////////////////////////////////////
void MainGameUI::SetCountdown( int number )
{
	if( m_pHUDUI != 0 )
		m_pHUDUI->SetCountdown( number );
}

/////////////////////////////////////////////////////
/// Method: SetGameTime
/// Params: [in]timer
///
/////////////////////////////////////////////////////
void MainGameUI::SetGameTime( float timer )
{
	if( m_pHUDUI != 0 )
		m_pHUDUI->SetGameTime( timer );
}

/////////////////////////////////////////////////////
/// Method: SetSpeedLever
/// Params: [in]percent
///
/////////////////////////////////////////////////////
void MainGameUI::SetSpeedLever( float percent )
{
	if( m_pControlsUI != 0 )
		m_pControlsUI->SetSpeedLever( percent );
}

/////////////////////////////////////////////////////
/// Method: SetCountdownState
/// Params: [in]state
///
/////////////////////////////////////////////////////
void MainGameUI::SetCountdownState( bool state )
{
	if( m_pHUDUI != 0 )
		m_pHUDUI->SetCountdownState( state );
}

/////////////////////////////////////////////////////
/// Method: SetGamePoints
/// Params: [in]points
///
/////////////////////////////////////////////////////
void MainGameUI::SetGamePoints( const char* points )
{
	if( m_pHUDUI != 0 )
		m_pHUDUI->SetGamePoints( points );
}

/////////////////////////////////////////////////////
/// Method: SetShowTokenTime
/// Params: [in]state
///
/////////////////////////////////////////////////////
void MainGameUI::SetShowTokenTime( bool state )
{
	if( m_pPauseUI != 0 )
		m_pPauseUI->SetShowTokenTime( state );
}

/////////////////////////////////////////////////////
/// Method: SetTokenCountdown
/// Params: [in]time
///
/////////////////////////////////////////////////////
void MainGameUI::SetTokenCountdown( float time )
{
	if( m_pPauseUI != 0 )
		m_pPauseUI->SetTokenCountdown( time );
}

/////////////////////////////////////////////////////
/// Method: SetTokenCounts
/// Params: [in]taku, [in]mei, [in]earl, [in]fio, [in]mito, [in]uki
///
/////////////////////////////////////////////////////
void MainGameUI::SetTokenCounts( int taku, int mei, int earl, int fio, int mito, int uki )
{
	if( m_pPauseUI != 0 )
		m_pPauseUI->SetTokenCounts( taku, mei, earl, fio, mito, uki );
}

/////////////////////////////////////////////////////
/// Method: SetTotalFlightTime
/// Params: [in]time
///
/////////////////////////////////////////////////////
void MainGameUI::SetTotalFlightTime( float time )
{
	if( m_pPauseUI != 0 )
		m_pPauseUI->SetTotalFlightTime( time );
}

/////////////////////////////////////////////////////
/// Method: DoLevelComplete
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::DoLevelComplete()
{
	m_bShowLevelComplete = true;

	if( m_pLevelCompleteUI != 0 )
		m_pLevelCompleteUI->Setup();
}

