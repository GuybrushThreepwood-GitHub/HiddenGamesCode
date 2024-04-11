
/*===================================================================
	File: LevelCompleteUI.cpp
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
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/LevelCompleteUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendMainMenuUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/MainGameState.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/ingame_levelcomplete.hgm.h"
#include "GameStates/UI/GeneratedFiles/ingame_levelcomplete.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
LevelCompleteUI::LevelCompleteUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_LEVELCOMPLETEARCADE )
{
	m_UIMesh = 0;

	m_ResumeGame = false;
	m_ReplayMode = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
LevelCompleteUI::~LevelCompleteUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1005);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/ingame_levelcomplete.hui", srcDims, m_UIMesh );
	
	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	if( !m_pProfile->adsRemoved )
	{
		core::app::SetAdBarState( true );
		#ifdef BASE_PLATFORM_WINDOWS
			core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
		#endif // BASE_PLATFORM_WINDOWS
	}
    
    m_Elements.RefreshSelected(HUI_BUTTON_RESUME);
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}

	m_Elements.Release();
	
	// sync achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int LevelCompleteUI::TransitionIn()
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

	// play success
	/*ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;
	sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	bufferId = AudioSystem::GetInstance()->FindSound( 101 );

	if( sourceId != snd::INVALID_SOUNDSOURCE &&
		bufferId != snd::INVALID_SOUNDBUFFER )
	{
		AudioSystem::GetInstance()->PlayAudio( sourceId, bufferId, math::Vec3(0.0f,0.0f,0.0f), AL_TRUE );
	}*/

	// transition finished, make sure to do a clean draw
	Draw();
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int LevelCompleteUI::TransitionOut()
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
void LevelCompleteUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;
	m_ResumeGame = false;
	m_ReplayMode = false;

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

    if( m_Elements.CheckElementForTouch( HUI_BUTTON_QUIT ) ||
       m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
		ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_MainMenu) );
		return;
	}
	else
	if( core::app::IsGameCenterAvailable() && 
	   m_Elements.CheckElementForTouch(HUI_BUTTON_SHOW_ACHIEVEMENTS) )
	{
		/*if( core::app::IsNetworkAvailable() &&
		   support::Scores::GetInstance()->IsLoggedIn())*/
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			
			support::Scores::GetInstance()->ShowAchievements();
		}
	}
	else
	if( m_Elements.CheckElementForTouch( HUI_BUTTON_RESUME ) )
	{
		// switch to freeflight
		GameSystems::GetInstance()->SetGameMode( GameSystems::GAMEMODE_FREEFLIGHT );

		// continue the level
		if( GetStateManager()->GetSecondaryStateManager() )
		{
			MainGameState* pState = static_cast<MainGameState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

			pState->ContinueLevel();
		}

		m_ResumeGame = true;
	}
	else
	if( m_Elements.CheckElementForTouch( HUI_BUTTON_RESTART ) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		// reset the main game
		if( GetStateManager()->GetSecondaryStateManager() )
		{
			MainGameState* pState = static_cast<MainGameState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

			pState->ReplayLevel();

			m_ReplayMode = true;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteUI::Draw()
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
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: Setup
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteUI::Setup()
{
	ProfileManager::ActiveLevelData* pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( pLevelData != 0 );

	m_GameData = GetScriptDataHolder()->GetGameData();

	// find out if this was a level record
	int packId = ProfileManager::GetInstance()->GetModePackId();
	int levelId = ProfileManager::GetInstance()->GetModeLevelId();
	int gameMode = GameSystems::GetInstance()->GetGameMode();
	int gameModeIndex = GameSystems::GetInstance()->GetGameModeIndex();

	ProfileManager::LevelBest bestScore;

	if( ProfileManager::GetInstance()->GetScore( packId, levelId, gameMode, gameModeIndex, &bestScore ) )
	{
		if( gameMode == GameSystems::GAMEMODE_RINGRUN )
		{
			if( pLevelData->time <= bestScore.bestTime )
			{
				bestScore.bestTime = pLevelData->time;
				ProfileManager::GetInstance()->AddScore( true, packId, levelId, gameMode, gameModeIndex, 0, 0, bestScore.bestTime );
			}
		}
		else
		if( gameMode == GameSystems::GAMEMODE_TARGET )
		{
			if( pLevelData->targetsDestroyed >= bestScore.targetsDestroyed )
			{
				bestScore.targetsDestroyed = pLevelData->targetsDestroyed;
				bestScore.targetsTotal = pLevelData->targetsTotal;

				if( pLevelData->time <= bestScore.bestTime )
				{
					bestScore.bestTime = pLevelData->time;
					ProfileManager::GetInstance()->AddScore( true, packId, levelId, gameMode, gameModeIndex, bestScore.targetsDestroyed, bestScore.targetsTotal, bestScore.bestTime );
				}
			}
		}
	}
	else
	{
		// no score found, best score is the current score
		bestScore.packId = packId;
		bestScore.levelId = levelId;
		bestScore.mode = gameMode;
		bestScore.layout = gameModeIndex;

		bestScore.bestTime = pLevelData->time;
		bestScore.targetsDestroyed = pLevelData->targetsDestroyed;
		bestScore.targetsTotal = pLevelData->targetsTotal;

		ProfileManager::GetInstance()->AddScore( false, packId, levelId, gameMode, gameModeIndex, bestScore.targetsDestroyed, bestScore.targetsTotal, bestScore.bestTime );
	}

	if( gameMode == GameSystems::GAMEMODE_RINGRUN )
	{
		m_Elements.ChangeElementDrawState( HUI_TEXT_SCORE_LAST, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_SCORE_LAST, true );

		m_Elements.ChangeElementDrawState( HUI_TEXT_TIME_LAST, false );
		m_Elements.ChangeElementDrawState( HUI_LABEL_TIME_LAST, false );

		m_Elements.ChangeElementDrawState( HUI_TEXT_SCORE_BEST, false );
		m_Elements.ChangeElementDrawState( HUI_LABEL_SCORE_BEST, false );
		
		m_Elements.ChangeElementDrawState( HUI_TEXT_TIME_BEST, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_TIME_BEST, true );

		m_Elements.ChangeElementText( HUI_LABEL_SCORE_LAST, "%s", res::GetScriptString(1221) );
		m_Elements.ChangeElementText( HUI_LABEL_TIME_BEST, "%s", res::GetScriptString(1223) );

		SetTime( m_Elements, HUI_TEXT_SCORE_LAST, pLevelData->time );
		SetTime( m_Elements, HUI_TEXT_TIME_BEST, bestScore.bestTime );
	}
	else if( gameMode == GameSystems::GAMEMODE_TARGET )
	{
		m_Elements.ChangeElementDrawState( HUI_TEXT_SCORE_LAST, true );
		m_Elements.ChangeElementDrawState( HUI_TEXT_TIME_LAST, true );
		m_Elements.ChangeElementDrawState( HUI_TEXT_SCORE_BEST, true );
		m_Elements.ChangeElementDrawState( HUI_TEXT_TIME_BEST, true );

		m_Elements.ChangeElementDrawState( HUI_LABEL_SCORE_LAST, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_TIME_LAST, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_SCORE_BEST, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_TIME_BEST, true );

		m_Elements.ChangeElementText( HUI_LABEL_SCORE_LAST, "%s", res::GetScriptString(1220) );
		m_Elements.ChangeElementText( HUI_LABEL_TIME_LAST, "%s", res::GetScriptString(1221) );
		m_Elements.ChangeElementText( HUI_LABEL_SCORE_BEST, "%s", res::GetScriptString(1222) );
		m_Elements.ChangeElementText( HUI_LABEL_TIME_BEST, "%s", res::GetScriptString(1223) );

		m_Elements.ChangeElementText( HUI_TEXT_SCORE_LAST, "%d / %d", pLevelData->targetsDestroyed, pLevelData->targetsTotal );
		m_Elements.ChangeElementText( HUI_TEXT_SCORE_BEST, "%d / %d", bestScore.targetsDestroyed, bestScore.targetsTotal );

		SetTime( m_Elements, HUI_TEXT_TIME_LAST, pLevelData->time );
		SetTime( m_Elements, HUI_TEXT_TIME_BEST, bestScore.bestTime );
	}

	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	DBG_ASSERT( pScriptData != 0 );
	ScriptDataHolder::LevelScriptData levelData = pScriptData->GetLevelData();

	m_Elements.ChangeElementText( HUI_TEXT_LEVELNAME, "%s", levelData.levelName );

	if( !core::app::IsGameCenterAvailable() ||
	   !core::app::IsNetworkAvailable() )
	{
		m_Elements.ChangeElementDrawState( HUI_BUTTON_SHOW_ACHIEVEMENTS, false );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_ACHIEVEMENT, false );
	}
	else
	{
		// sync achievements
		if( support::Scores::IsInitialised() )
			support::Scores::GetInstance()->SyncAchievements();
	}

	ProfileManager::GetInstance()->SaveProfile();
    
    m_Elements.RefreshSelected(HUI_BUTTON_RESUME);
    core::app::SetHandleMenuButton(true);
}