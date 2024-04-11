
/*===================================================================
	File: HudUI.cpp
	Game: Cabby

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

#include "CabbyConsts.h"
#include "Cabby.h"

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
#include "GameStates/UI/TimeUpUI.h"
#include "GameStates/UI/LevelCompleteArcadeUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/GameCompleteUI.h"
#include "GameStates/UI/UnlockUI.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UnlockState.h"

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
	m_Player = PhysicsWorld::GetPlayer();

	m_pControlsUI = new ControlsUI( stateManager, inputSystem );
	DBG_ASSERT( m_pControlsUI != 0 );

	m_pIconsUI = new IconsUI( stateManager, inputSystem );
	DBG_ASSERT( m_pIconsUI != 0 );

	m_pHUDUI = new HudUI( stateManager, inputSystem );
	DBG_ASSERT( m_pHUDUI != 0 );

	m_pPauseUI = new PauseUI( stateManager, inputSystem );
	DBG_ASSERT( m_pPauseUI != 0 );

	m_pTimeUpUI = new TimeUpUI( stateManager, inputSystem );
	DBG_ASSERT( m_pTimeUpUI != 0 );

	m_bShowPauseOverlay = false;
	m_bShowTimeUp = false;

	m_RespawnFadeToBlack = false;
	m_RespawnFadeToGame = false;
	m_RespawnFade = 0;

	m_FinishedTowTruckFlyBy = true;
	m_FirstCostDraw = false;
	m_RescueFadeToBlack = false;
	m_RescueFadeToGame = false;
	m_RescueFade = 0;

	m_pProfile = ProfileManager::GetInstance()->GetProfile();
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

	if( m_pTimeUpUI != 0 )
		delete m_pTimeUpUI;
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
		m_pControlsUI->SetupControls( m_pProfile->controlType );
	}

	if( m_pIconsUI != 0 )
		m_pIconsUI->Enter();

	if( m_pHUDUI != 0 )
		m_pHUDUI->Enter();

	if( m_pPauseUI != 0 )
		m_pPauseUI->Enter();

	if( m_pTimeUpUI != 0 )
		m_pTimeUpUI->Enter();

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

	if( m_pTimeUpUI != 0 )
		m_pTimeUpUI->Exit();
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

		// always draw
		GameSystems::GetInstance()->DrawAchievementUI();

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

		// always draw
		GameSystems::GetInstance()->DrawAchievementUI();

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
/*			AudioSystem::GetInstance()->PlayUIAudio();

			// start audio track
			int trackId = 0;
			AudioSystem::GetInstance()->StopMusicTrack();
			AudioSystem::GetInstance()->ClearMusicTrack();

			AudioSystem::GetInstance()->PlayMusicTrack( m_pScriptData->GetMusicTrackList()[trackId]->musicFile, true );
			AudioSystem::GetInstance()->SetCurrentMusicTrack( trackId );

			// make sure the volume is up
			AudioSystem::GetInstance()->SetMusicTrackVolume( m_pScriptData->GetMusicTrackList()[trackId]->musicVolume );
*/
			m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
			ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );

			return;			
		}
		else
        if( m_pPauseUI->ContinuePress() )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_bShowPauseOverlay = false;

#ifdef CABBY_FREE
			core::app::SetAdBarState( false );
#endif // CABBY_FREE

            input::gInputState.KeyStates.bKeyPressTime[input::KEY_SPACE] = 0;
            input::gInputState.KeyStates.bKeys[input::KEY_SPACE] = false;
		}
		else
		if( m_pPauseUI->ControlChange() )
		{
            if( m_pControlsUI != 0 )
                m_pControlsUI->SetupControls(m_pProfile->controlType);
		}
	}
	else if( m_bShowTimeUp )
	{
		if( m_pTimeUpUI != 0 )
			m_pTimeUpUI->Update( deltaTime );

        if( m_pTimeUpUI->ContinuePress() ||
           m_InputSystem.GetMenu() )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			
			m_bShowTimeUp = false;

			// tow truck can unlock in arcade
			if( m_pProfile->towTruckShouldUnlock )
			{
				if( !m_pProfile->towTruckUnlocked )
				{
					ProfileManager::GetInstance()->AddVehicleToUnlocks( 2 );
					m_pProfile->towTruckUnlocked = true;

					// go to the unlock state	
					ChangeState( new UnlockUI(*m_pStateManager, m_InputSystem, ProfileManager::ARCADE_MODE ) );
				}
				else
					ChangeState( new LevelCompleteArcadeUI(*m_pStateManager, m_InputSystem) );
			}
			else
				ChangeState( new LevelCompleteArcadeUI(*m_pStateManager, m_InputSystem) );
			
			return;
		}
	}
	else
	{
		if( m_pControlsUI != 0 )
		{
			if (drawGameControlsUI)
			{
				m_pControlsUI->Update(deltaTime);

				if (m_pControlsUI->LeftPress())
					m_InputSystem.SetLeft(true);
				else
					m_InputSystem.SetLeft(false);

				if (m_pControlsUI->RightPress())
					m_InputSystem.SetRight(true);
				else
					m_InputSystem.SetRight(false);

				if (m_pControlsUI->UpPress())
					m_InputSystem.SetThrottle(true);
				else
					m_InputSystem.SetThrottle(false);

				if (m_pControlsUI->DownPress())
					m_InputSystem.SetAirBrake(true);
				else
					m_InputSystem.SetAirBrake(false);

				if (m_pControlsUI->LandingGearPress())
					m_InputSystem.SetLandingGear(true);
				else
					m_InputSystem.SetLandingGear(false);
			}

			if( !m_bShowPauseOverlay &&
				!m_RespawnFadeToBlack &&
				!m_RespawnFadeToGame &&
				!m_RescueFadeToBlack &&
				!m_RescueFadeToGame )
			{
				if( m_pControlsUI->PausePress() ||
					m_InputSystem.GetPause() ||
                   core::app::IsInBackground() ||
                   m_InputSystem.GetMenu() )
				{
					//AudioSystem::GetInstance()->PlayUIAudio();

					m_InputSystem.SetReset(true);
					m_bShowPauseOverlay = true;

#ifdef CABBY_FREE
					core::app::SetAdBarState( true );
					#ifdef BASE_PLATFORM_WINDOWS
						core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
					#endif // BASE_PLATFORM_WINDOWS
#endif // CABBY_FREE

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

		if( m_pIconsUI != 0 )
			m_pIconsUI->Update( deltaTime );

		if( m_pHUDUI != 0 )
			m_pHUDUI->Update( deltaTime );
	}
	//m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );
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
	if (drawGameControlsUI)
	{
		if( m_pControlsUI != 0 )
			m_pControlsUI->Draw();
	}
		
	if( m_pHUDUI != 0 )
		m_pHUDUI->Draw();


	if( m_pIconsUI != 0 )
		m_pIconsUI->Draw();

	// PAUSE OVERLAY
	if( m_bShowPauseOverlay )
	{
		if( drawGameUI )
		{
			if( m_pPauseUI != 0 )
				m_pPauseUI->Draw();
		}
	}

	// TIMEUP OVERLAY
	if( m_bShowTimeUp )
	{
		if( drawGameUI )
		{
			if( m_pTimeUpUI != 0 )
				m_pTimeUpUI->Draw();
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

	if( m_FinishedTowTruckFlyBy && m_RescueFadeToGame )
	{
		// draw fade
		m_RescueFade -= 15;
		if( m_RescueFade <= 0 )
		{
			m_RescueFade = 0;
			m_RescueFadeToGame = false;
		}

		DrawFullscreenQuad( 0.0f, m_RescueFade );

		m_pHUDUI->Draw();
	}

	if( m_RescueFadeToBlack )
	{
		// draw fade
		m_RescueFade += 15;
		if( m_RescueFade >= 255 )
		{
			m_RescueFade = 255;
			m_RescueFadeToBlack = false;
			m_FinishedTowTruckFlyBy = false;
			m_RescueFadeToGame = true;

			m_pHUDUI->InitTowTruck();
		}

		DrawFullscreenQuad( 0.0f, m_RescueFade );

		m_pHUDUI->Draw();
	}

	if( m_RescueFadeToGame && !m_FinishedTowTruckFlyBy )
	{
		DrawFullscreenQuad( 0.0f, 255 );

		// draw flying icon
		m_pHUDUI->UpdateTowTruck(m_LastDeltaTime);

		m_FinishedTowTruckFlyBy = m_pHUDUI->TowTruckFinished();

		m_pHUDUI->Draw();
	}

	// always draw last
	GameSystems::GetInstance()->DrawAchievementUI();
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
/// Method: DoRescueFadeToBlack
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::DoRescueFadeToBlack()
{
	m_RescueFadeToBlack = true;
	m_FinishedTowTruckFlyBy = false;
	m_FirstCostDraw = true;
	m_RescueFade = 0;
}

/////////////////////////////////////////////////////
/// Method: IsRescueFadeToGameComplete
/// Params: None
///
/////////////////////////////////////////////////////
bool MainGameUI::IsRescueFadeToGameComplete()
{
	return( !m_RescueFadeToGame );
}

/////////////////////////////////////////////////////
/// Method: IsRescueFadeToGameComplete
/// Params: None
///
/////////////////////////////////////////////////////
bool MainGameUI::HasFlyByFinished()
{
	return(m_FinishedTowTruckFlyBy);
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
/// Method: DoTimeUp
/// Params: None
///
/////////////////////////////////////////////////////
void MainGameUI::DoTimeUp()
{
	m_bShowTimeUp = true;
}
