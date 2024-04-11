
/*===================================================================
	File: LevelCompleteArcadeUI.cpp
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
#include "GameStates/UI/LevelCompleteArcadeUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/MainGameUI.h"
#include "GameStates/MainGameState.h"
#include "GameStates/UnlockState.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/level_complete_arcade.hgm.h"
#include "GameStates/UI/GeneratedFiles/level_complete_arcade.hui.h"

namespace
{
	const int NUM_STARS = 5;
	const int ORIGINAL_LEVEL_COUNT = 15;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
LevelCompleteArcadeUI::LevelCompleteArcadeUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_LEVELCOMPLETEARCADE )
{
	m_UIMesh = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
LevelCompleteArcadeUI::~LevelCompleteArcadeUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteArcadeUI::Enter()
{
	int i=0;
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1012);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/level_complete_arcade.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	ProfileManager::ActiveLevelData* pLevelData = ProfileManager::GetInstance()->GetActiveLevelData();
	DBG_ASSERT( pLevelData != 0 );

	m_GameMode = ProfileManager::GetInstance()->GetGameMode();
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_Elements.ChangeElementText( HUI_TEXT_STARCHALLENGE_2, "%s%d", res::GetScriptString(1251), pLevelData->levelArcadeCustomers  );
	m_Elements.ChangeElementText( HUI_TEXT_STARCHALLENGE_3, "%s%d", res::GetScriptString(1252), pLevelData->levelArcadeMoney );

	// level complete
	m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_1, false );
	m_UIMesh->SetMeshDrawState( ICON_STAR_ON_1, true );

	int runningStarCount = 1;

	// customer
	if( pLevelData->customerCountStar )
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_2, false );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_2, true );	

		runningStarCount++;
	}
	else
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_2, true );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_2, false );	
	}

	// money
	if( pLevelData->moneyStar )
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_3, false );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_3, true );	

		runningStarCount++;
	}
	else
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_3, true );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_3, false );	
	}

	// lives lost
	if( pLevelData->taxisLost <= 0 )
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_4, false );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_4, true );	

		runningStarCount++;
	}
	else
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_4, true );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_4, false );	
	}

	if( pLevelData->starCollected )
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_5, false );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_5, true );	

		runningStarCount++;
	}
	else
	{
		m_UIMesh->SetMeshDrawState( ICON_STAR_OFF_5, true );
		m_UIMesh->SetMeshDrawState( ICON_STAR_ON_5, false );	
	}

	for( i=0; i < NUM_STARS; ++i )
	{
		if( (runningStarCount-1) >= i )
			m_UIMesh->SetMeshDrawState( ICON_BIGSTAR1_ON_1+i, true );
		else
			m_UIMesh->SetMeshDrawState( ICON_BIGSTAR1_ON_1+i, false );
	}

	// find out if this was a level record
	ProfileManager::ArcadeBest* pBestArcadeData = 0;
	int packId = ProfileManager::GetInstance()->GetArcadeModePackId();
	int levelId = ProfileManager::GetInstance()->GetArcadeModeLevelId();

	pBestArcadeData = ProfileManager::GetInstance()->GetArcadeBestData(((packId)*m_GameData.MAX_ITEMS_PER_PACK)+(levelId));
	DBG_ASSERT( pBestArcadeData != 0 );

	// update if it was a new record
	if( pLevelData->moneyEarned >= pBestArcadeData->highestMoney )
		pBestArcadeData->highestMoney = pLevelData->moneyEarned;

	if( runningStarCount >= pBestArcadeData->maxStars )
	{
		pBestArcadeData->maxStars = runningStarCount;
		ProfileManager::GetInstance()->SaveProfile();
	}
	
	// 5 star awards
#ifdef CABBY_LITE
	// check all levels
	for( i=0; i < m_GameData.TOTAL_LEVELS; ++i )
	{
		ProfileManager::ArcadeBest *pData = ProfileManager::GetInstance()->GetArcadeBestData( i );
		DBG_ASSERT( pData != 0 );

		if( pData != 0 )
		{
			if( pData->maxStars >= NUM_STARS )
			{
				GameSystems::GetInstance()->AwardAchievement(i+1);
			}
		}
	}
#else
	
	// 5 star for this level ?
	if( pBestArcadeData != 0 )
	{
		if( pBestArcadeData->maxStars >= NUM_STARS )
		{
			GameSystems::GetInstance()->AwardAchievement(26+((packId)*m_GameData.MAX_ITEMS_PER_PACK)+(levelId));
			
#ifdef BASE_SUPPORT_KIIP
			GameSystems::GetInstance()->AwardKiipAchievement(5+((packId)*m_GameData.MAX_ITEMS_PER_PACK)+(levelId));
#endif // BASE_SUPPORT_KIIP
		}
	}

	// check for 5 stars in all arcade levels
	bool do5StarAward = true;
	if( runningStarCount >= NUM_STARS )
	{
		// only check original 15 levels
		for( i=0; i < ORIGINAL_LEVEL_COUNT; ++i )
		{
			ProfileManager::ArcadeBest *pData = ProfileManager::GetInstance()->GetArcadeBestData( i );
			DBG_ASSERT( pData != 0 );
			
			if( pData != 0 )
			{
				if( pData->maxStars < NUM_STARS )
					do5StarAward = false;
			}
		}

		if( do5StarAward )
		{
			GameSystems::GetInstance()->AwardAchievement(21);
			
#ifdef BASE_SUPPORT_KIIP
			GameSystems::GetInstance()->AwardKiipAchievement(20);
#endif // BASE_SUPPORT_KIIP
		}
	}	
	
#endif // CABBY_LITE

	// sync achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();

#ifdef CABBY_FREE
	core::app::SetAdBarState( true );
	#ifdef BASE_PLATFORM_WINDOWS
		core::app::SetAdBarState(  GetScriptDataHolder()->GetDevData().showPCAdBar );
	#endif // BASE_PLATFORM_WINDOWS
#endif // CABBY_FREE

	m_Elements.ChangeElementDrawState(HUI_BUTTON_QUIT, true);
	m_Elements.ChangeElementDrawState(HUI_BUTTON_START, true);

	m_Elements.RefreshSelected(HUI_BUTTON_START);
    
    core::app::SetHandleMenuButton(true);

}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteArcadeUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}

	// sync achievements
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteArcadeUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteArcadeUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int LevelCompleteArcadeUI::TransitionIn()
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

		GameSystems::GetInstance()->DrawAchievementUI();

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}

	// play success
	ALuint sourceId = snd::INVALID_SOUNDSOURCE;
	ALuint bufferId = snd::INVALID_SOUNDBUFFER;
	sourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();
	bufferId = AudioSystem::GetInstance()->FindSound( 101/*"complete.wav"*/ );

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
int LevelCompleteArcadeUI::TransitionOut()
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
void LevelCompleteArcadeUI::Update( float deltaTime )
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

	if( m_Elements.CheckElementForTouch( HUI_BUTTON_QUIT ) ||
       m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );

		return;
	}

	if( m_Elements.CheckElementForTouch( HUI_BUTTON_START ) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

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
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void LevelCompleteArcadeUI::Draw()
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

	GameSystems::GetInstance()->DrawAchievementUI();
	
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
