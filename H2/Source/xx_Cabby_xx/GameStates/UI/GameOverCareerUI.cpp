
/*===================================================================
	File: GameOverCareerUI.cpp
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
#include "GameStates/UI/GameOverCareerUI.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/TitleScreenState.h"

#include "GameStates/UI/GeneratedFiles/gameover_career.hgm.h"
#include "GameStates/UI/GeneratedFiles/gameover_career.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameOverCareerUI::GameOverCareerUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_GAVEOVERCAREER )
{
	m_UIMesh = 0;
	m_pProfile = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameOverCareerUI::~GameOverCareerUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1006);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/gameover_career.hui", srcDims, m_UIMesh );

	m_GameData = GetScriptDataHolder()->GetGameData();

	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	// not enough cash, it's game over
	m_pProfile->careerGameOver		= true;
	m_pProfile->currentLevelIndex	= 0;

	m_pProfile->currentFuel			= 100.0f;
	m_pProfile->currentLives		= m_GameData.DEFAULT_LIVES_COUNT;
	m_pProfile->currentMoney		= 0;
	m_pProfile->currentPerfectFares	= 0;
	m_pProfile->currentCustomersLost= 0;
	m_pProfile->currentLivesLost	= 0;
	m_pProfile->extraLifeTotal		= 0;
	ProfileManager::GetInstance()->SaveProfile();

	AudioSystem::GetInstance()->StopAll();

#ifdef CABBY_FREE
	core::app::SetAdBarState( true );
	#ifdef BASE_PLATFORM_WINDOWS
		core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
	#endif // BASE_PLATFORM_WINDOWS
#endif // CABBY_FREE

	m_Elements.RefreshSelected();
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerUI::Exit()
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
void GameOverCareerUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverCareerUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int GameOverCareerUI::TransitionIn()
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
int GameOverCareerUI::TransitionOut()
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
void GameOverCareerUI::Update( float deltaTime )
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

	if( m_Elements.CheckElementForTouch(HUI_BTN_QUIT) ||
       m_InputSystem.GetMenu() )
	{
		ProfileManager::GetInstance()->SaveProfile();
		
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
void GameOverCareerUI::Draw()
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

	// always draw
	GameSystems::GetInstance()->DrawAchievementUI();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
