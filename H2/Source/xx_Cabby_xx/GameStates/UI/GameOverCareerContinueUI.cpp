
/*===================================================================
	File: GameOverCareerContinueUI.cpp
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
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/GameOverCareerContinueUI.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/UI/TextFormattingFuncs.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/MainGameState.h"

#include "GameStates/UI/GeneratedFiles/gameover_career_continue.hgm.h"
#include "GameStates/UI/GeneratedFiles/gameover_career_continue.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameOverCareerContinueUI::GameOverCareerContinueUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_GAVEOVERCAREERCONTINUE )
{
	m_UIMesh = 0;

	m_pProfile = 0;
	m_pLevelData = 0;

	m_NumberOfLivesRequested = 0;
	m_MaxNumberOfLives = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameOverCareerContinueUI::~GameOverCareerContinueUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerContinueUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1007);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/gameover_career_continue.hui", srcDims, m_UIMesh );

	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( m_pLevelData != 0 );

	m_GameData = GetScriptDataHolder()->GetGameData();

	m_NumberOfLivesRequested = 0;
	m_MaxNumberOfLives = (m_pProfile->currentMoney / m_GameData.CONTINUE_COST);
	if( m_MaxNumberOfLives > 99 )
		m_MaxNumberOfLives = 99;

	m_Elements.ChangeElementText( HUI_LABEL_TIMES, "%s", res::GetScriptString(61) );
	SetLivesString( m_Elements, HUI_TEXT_LIVES_COUNT, m_NumberOfLivesRequested );

	if( (m_NumberOfLivesRequested*m_GameData.CONTINUE_COST) <= 0 )
	{
		m_UIMesh->SetMeshDrawState( BG_SPENT, false );
		m_Elements.ChangeElementDrawState( HUI_TEXT_TOTALSPENT, false );
	}
	else
		SetCashString( m_Elements, HUI_TEXT_TOTALSPENT, -(m_NumberOfLivesRequested*m_GameData.CONTINUE_COST) );

	SetCashString( m_Elements, HUI_TEXT_BANKBALANCE, m_pProfile->currentMoney );

	{
		const int BUFFER_SIZE = 16;
		char finalString[BUFFER_SIZE];

		int count = m_GameData.CONTINUE_COST;
		if( count >= 1000 )
		{
			int exp = count / 1000;
			int man = count % 1000;

			if( man == 0 )
			{
				snprintf( finalString, BUFFER_SIZE, "=%d,000", exp );
			}
			else
			{
				if( man < 10 && count >= 100 )
				{
					snprintf( finalString, BUFFER_SIZE, "=%d,00%d", exp, man );
				}
				else if( man < 100 && count >= 1000 )
				{
					snprintf( finalString, BUFFER_SIZE, "=%d,0%d", exp, man );
				}
				else
				{
					snprintf( finalString, BUFFER_SIZE, "=%d,%d", exp, man );
				}
			}
		}
		else
		{
			snprintf( finalString, BUFFER_SIZE, "=%d", count );
		}
		m_Elements.ChangeElementText( HUI_LABEL_TAXICOST, "%s", finalString );
	}

	// purchase and continue disable
	m_UIMesh->SetMeshDrawState( BTN_CONTINUE, false );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CONTINUE, false );
	m_Elements.ChangeElementDrawState(HUI_BTN_CONTINUE, false);

	AudioSystem::GetInstance()->StopAll();

#ifdef CABBY_FREE
	core::app::SetAdBarState( true );
	#ifdef BASE_PLATFORM_WINDOWS
		core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
	#endif // BASE_PLATFORM_WINDOWS
#endif // CABBY_FREE

	m_Elements.RefreshSelected(BTN_ADD_LIVES);
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerContinueUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerContinueUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerContinueUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int GameOverCareerContinueUI::TransitionIn()
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

	// play fail
	ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;
	sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	bufferId = AudioSystem::GetInstance()->FindSound( 103/*"fail.wav"*/ );

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		bufferId != snd::INVALID_SOUNDBUFFER )
	{
		AudioSystem::GetInstance()->PlayAudio( sourceId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
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
int GameOverCareerContinueUI::TransitionOut()
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
void GameOverCareerContinueUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	// new
	if (m_InputSystem.GetUIUp())
	{
		m_Elements.SelectPreviousRow();
	}
	else
	if (m_InputSystem.GetUIDown())
	{
		m_Elements.SelectNextRow();
	}
	else
	if (m_InputSystem.GetUILeft())
	{
		m_Elements.SelectPreviousColumn();
	}
	else
	if (m_InputSystem.GetUIRight())
	{
		m_Elements.SelectNextColumn();
	}

	if (m_InputSystem.GetUISelect())
		m_Elements.SelectionPress(true);
	else
		m_Elements.SelectionPress(false);

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	if( m_Elements.CheckElementForTouch(HUI_BTN_SUBTRACT_LIVES) )
	{
		if( m_NumberOfLivesRequested > 0 )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			m_NumberOfLivesRequested--;
			
			SetLivesString( m_Elements, HUI_TEXT_LIVES_COUNT, m_NumberOfLivesRequested );

			if( (m_NumberOfLivesRequested*m_GameData.CONTINUE_COST) <= 0 )
			{
				m_UIMesh->SetMeshDrawState( BG_SPENT, false );
				m_Elements.ChangeElementDrawState( HUI_TEXT_TOTALSPENT, false );
			}
			else
			{
				m_UIMesh->SetMeshDrawState( BG_SPENT, true );
				m_Elements.ChangeElementDrawState( HUI_TEXT_TOTALSPENT, true );
				SetCashString( m_Elements, HUI_TEXT_TOTALSPENT, -(m_NumberOfLivesRequested*m_GameData.CONTINUE_COST) );
			}
		}

		if( m_NumberOfLivesRequested > 0 )
		{
			m_UIMesh->SetMeshDrawState( BTN_CONTINUE, true );
			m_Elements.ChangeElementDrawState( HUI_LABEL_CONTINUE, true );
			m_Elements.ChangeElementDrawState(HUI_BTN_CONTINUE, true);
		}
		else
		{
			m_UIMesh->SetMeshDrawState( BTN_CONTINUE, false );
			m_Elements.ChangeElementDrawState( HUI_LABEL_CONTINUE, false );
			m_Elements.ChangeElementDrawState(HUI_BTN_CONTINUE, false);
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_ADD_LIVES) )
	{
		if( m_NumberOfLivesRequested+1 <= m_MaxNumberOfLives )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			m_NumberOfLivesRequested++;

			SetLivesString( m_Elements, HUI_TEXT_LIVES_COUNT, m_NumberOfLivesRequested );

			m_UIMesh->SetMeshDrawState( BG_SPENT, true );
			m_Elements.ChangeElementDrawState( HUI_TEXT_TOTALSPENT, true );
			SetCashString( m_Elements, HUI_TEXT_TOTALSPENT, -(m_NumberOfLivesRequested*m_GameData.CONTINUE_COST) );

			m_UIMesh->SetMeshDrawState( BTN_CONTINUE, true );
			m_Elements.ChangeElementDrawState( HUI_LABEL_CONTINUE, true );
			m_Elements.ChangeElementDrawState(HUI_BTN_CONTINUE, true);
		}
	}
	else
	if( m_NumberOfLivesRequested && m_Elements.CheckElementForTouch(HUI_BTN_CONTINUE) )
	{
		// play audio
		ALuint sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
		ALuint bufferId = AudioSystem::GetInstance()->FindSound( 105/*"money_big.wav"*/ );

		if( sourceId != snd::INVALID_SOUNDSOURCE &&
			bufferId != snd::INVALID_SOUNDBUFFER )
			AudioSystem::GetInstance()->PlayAudio( sourceId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE, AL_FALSE );	

		m_pProfile->currentMoney -= (m_NumberOfLivesRequested*m_GameData.CONTINUE_COST);
		m_pProfile->currentLives = m_NumberOfLivesRequested;
		m_pProfile->currentFuel = 100.0f;
		ProfileManager::GetInstance()->SaveProfile();

		// reset the main game
		if( GetStateManager()->GetSecondaryStateManager() )
		{
			MainGameState* pState = static_cast<MainGameState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

			pState->ReplayLevel();
		}

		// restart the level in arcade mode
		ChangeState( new MainGameUI(*m_pStateManager, m_InputSystem) );

#ifdef CABBY_FREE
		core::app::SetAdBarState( false );
#endif // CABBY_FREE
		return;
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_QUIT) ||
       m_InputSystem.GetMenu() )
	{
		m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
		return;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerContinueUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	if( m_UIMesh != 0 )
		m_UIMesh->Draw();
	
	m_Elements.Draw();
	m_Elements.DrawSelected();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );

	// always draw
	GameSystems::GetInstance()->DrawAchievementUI();

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
