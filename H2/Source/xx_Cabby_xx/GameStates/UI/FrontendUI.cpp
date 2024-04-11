
/*===================================================================
	File: FrontendUI.cpp
	Game: Cabby

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include <list>

#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"
#include "SupportBase.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/IAPList.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/FrontendPromptsUI.h"
#include "GameStates/UI/FrontendItemSelectUI.h"
#include "GameStates/UI/FrontendInAppPurchaseUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/frontend.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendUI::FrontendUI( StateManager& stateManager, InputSystem& inputSystem, FrontendStates state )
: IBaseGameState( stateManager, inputSystem, UI_FRONTEND )
, m_FrontendState(state)
{
	m_UIMesh = 0;

	m_pProfile = 0;
	m_pScriptData = 0;
	m_PackList.clear();
	m_pArcadeBest = 0;

	m_ManualFade = false;
	m_NextFrontendState = state;
	m_ReturnFrontendState = state;

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendUI::~FrontendUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1003);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend.hui", srcDims, m_UIMesh );
	
	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pScriptData = GetScriptDataHolder();
	m_PackList = m_pScriptData->GetLevelPackList();
	m_GameData = m_pScriptData->GetGameData();
	
	switch( m_FrontendState )
	{
		case FrontendStates_MainMenu:
		{
            core::app::SetHandleMenuButton(false);
			SetupMainMenu();
		}break;
		case FrontendStates_CareerStats:
		{
            core::app::SetHandleMenuButton(true);
			SetupCareerStats();
		}break;
		case FrontendStates_Credits:
		{
            core::app::SetHandleMenuButton(true);
			SetupCredits();
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
	
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();

#ifdef CABBY_FREE
	core::app::SetAdBarState( true );
	#ifdef BASE_PLATFORM_WINDOWS
		core::app::SetAdBarState(  m_pScriptData->GetDevData().showPCAdBar );
	#endif // BASE_PLATFORM_WINDOWS
#endif // CABBY_FREE

}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::Exit()
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
void FrontendUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendUI::TransitionIn()
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
int FrontendUI::TransitionOut()
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
void FrontendUI::Update( float deltaTime )
{
	if( m_ManualFade )
		return;
	
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

	switch( m_FrontendState )
	{
		case FrontendStates_MainMenu:
		{
			if( UpdateMainMenu(deltaTime) )
				return;
		}break;
		case FrontendStates_CareerStats:
		{
			if( UpdateCareerStats(deltaTime) )
				return;
		}break;
		case FrontendStates_Credits:
		{
			if( UpdateCredits(deltaTime) )
				return;
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();
	
	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();
	m_Elements.DrawSelected();

	if( m_ManualFade )
	{
		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		
		// if I manually tell it to fade
		if( !m_FinishedTransitionOut )
		{
			// draw fade
			m_FadeTransition += 15;
			if( m_FadeTransition >= 255 )
			{
				m_FadeTransition = 255;
				m_FinishedTransitionOut = true;
				
				// get ready to fade in again
				PrepareTransitionIn();
				
				m_FrontendState = m_NextFrontendState;
				
				switch( m_FrontendState )
				{
					case FrontendStates_MainMenu:
					{
                        core::app::SetHandleMenuButton(false);
						SetupMainMenu();
					}break;
					case FrontendStates_CareerStats:
					{
                        core::app::SetHandleMenuButton(true);
                        
						SetupCareerStats();
					}break;
					case FrontendStates_Credits:
					{
                        core::app::SetHandleMenuButton(true);
                        
						SetupCredits();
					}break;
					default:
						DBG_ASSERT(0);
						break;
				}				
			}
		}
	
		if( !m_FinishedTransitionIn )
		{
			// draw fade
			m_FadeTransition -= 15;
			if( m_FadeTransition <= 0 )
			{
				m_ManualFade = false;
				m_FadeTransition = 0;
				m_FinishedTransitionIn = true;
			}			
		}
		
		DrawFullscreenQuad( 0.0f, m_FadeTransition );
		
		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		
	}

	// always draw
	GameSystems::GetInstance()->DrawAchievementUI();
}

/////////////////////////////////////////////////////
/// Method: SetupMainMenu
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::SetupMainMenu()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_UIMesh->SetMeshDrawState( BTN_CAREER_NEW, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CAREER_NEW_MAINMENU, true );
	m_Elements.ChangeElementDrawState( HUI_BTN_CAREER_NEW_MAINMENU, true );

	m_UIMesh->SetMeshDrawState( BTN_CAREER_CONTINUE, true );

	m_UIMesh->SetMeshDrawState( BTN_CAREER_STATS, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CAREER_STATS_MAINMENU, true );
	m_Elements.ChangeElementDrawState( HUI_BTN_CAREER_STATS_MAINMENU, true );

	m_UIMesh->SetMeshDrawState( BTN_ARCADE, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_ARCADE_MAINMENU, true );
	m_Elements.ChangeElementDrawState( HUI_BTN_ARCADE_MAINMENU, true );

	m_Elements.ChangeElementDrawState( HUI_BTN_CREDITS_MAINMENU, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_CREDITS_MAINMENU, true );

#ifdef CABBY_LITE
	// change new career to say it's full version
	m_Elements.ChangeElementText( HUI_LABEL_CAREER_NEW_MAINMENU, "%s", res::GetScriptString(55) );

	// disable career stats
	m_UIMesh->SetMeshDrawState( BTN_CAREER_STATS, false );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CAREER_STATS_MAINMENU, false );
	m_Elements.ChangeElementDrawState( HUI_BTN_CAREER_STATS_MAINMENU, false);
#endif // CABBY_LITE

	/*if( core::app::IsGameCenterAvailable() &&
	   core::app::IsNetworkAvailable() )*/
	{
		m_Elements.ChangeElementDrawState( HUI_BTN_SHOW_ACHIEVEMENTS, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_ACHIEVEMENTS_MAINMENU, true );
	}

	if( core::app::IsStoreAvailable() )
	{
		if( !core::app::IsPCOnly()  )
		{
			m_Elements.ChangeElementDrawState( HUI_BTN_RESTORE_PURCHASES, true );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_RESTORE_MAINMENU, true );
		}
	}
	
	int defaultSelectedElement = -1;

	if( !m_pProfile->careerActive ||
		m_pProfile->careerGameOver )
	{
		m_UIMesh->SetMeshDrawState( BTN_CAREER_CONTINUE, false );
		m_Elements.ChangeElementDrawState( HUI_LABEL_CAREER_CONTINUE_MAINMENU, false );
		m_Elements.ChangeElementDrawState(HUI_BTN_CAREER_CONTINUE_MAINMENU, false );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( BTN_CAREER_CONTINUE, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_CAREER_CONTINUE_MAINMENU, true );
		m_Elements.ChangeElementDrawState(HUI_BTN_CAREER_CONTINUE_MAINMENU, true);

		defaultSelectedElement = HUI_BTN_CAREER_CONTINUE_MAINMENU;
	}

	m_Elements.ChangeElementDrawState( HUI_BTN_TOGGLE_SOUND_MAINMENU, true );
	if( m_pProfile->sfxState )
	{
		m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_MAINMENU, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_OFF_MAINMENU, false );
	}
	else
	{
		m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_MAINMENU, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_OFF_MAINMENU, true );
	}

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );
	m_Elements.ChangeElementText( HUI_LABEL_TITLE_ALL, "%s", res::GetScriptString(1040) );

    if( !core::app::IstvOS() )
    {
        m_UIMesh->SetMeshDrawState( BTN_MOREGAMES, true );
        m_Elements.ChangeElementDrawState( HUI_LABEL_MOREGAMES, true );
        m_Elements.ChangeElementDrawState( HUI_BTN_MOREGAMES, true );

        m_Elements.ChangeElementDrawState( HUI_BTN_RATE, true );
        m_Elements.ChangeElementDrawState( HUI_GLYPH_RATE, true );
    }
    
	m_Elements.RefreshSelected(defaultSelectedElement);
}
		
/////////////////////////////////////////////////////
/// Method: SetupCareerStats
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::SetupCareerStats()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_UIMesh->SetMeshDrawState( BG_WINDOW, true  );
	m_UIMesh->SetMeshDrawState( BG_LINE1, true  );
	m_UIMesh->SetMeshDrawState( BG_LINE2, true  );
	m_UIMesh->SetMeshDrawState( BG_LINE3, true  );
	m_UIMesh->SetMeshDrawState( BG_LINE4, true  );
	m_UIMesh->SetMeshDrawState( BG_LINE5, true  );
	m_UIMesh->SetMeshDrawState( ICON_CUSTOMER, true  );
	m_UIMesh->SetMeshDrawState( ICON_PARCEL, true  );
	m_UIMesh->SetMeshDrawState( ICON_PERFECT, true  );
	m_UIMesh->SetMeshDrawState( ICON_TAXSILOST, true  );
	m_UIMesh->SetMeshDrawState( ICON_BANKBALANCE, true  );

	m_UIMesh->SetMeshDrawState( BTN_BACK_CAREERSTATS, true  );
	
	m_Elements.ChangeElementDrawState(HUI_BTN_BACK_CAREERSTATS, true);
	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK_CAREERSTATS, true  );

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );
	m_Elements.ChangeElementText( HUI_LABEL_TITLE_ALL, "%s", res::GetScriptString(1060) );

	m_Elements.ChangeElementDrawState( HUI_LABEL_CUSTOMERS_CAREERSTATS, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_CUSTOMERS_CAREERSTATS, true );
	m_Elements.ChangeElementText( HUI_TEXT_CUSTOMERS_CAREERSTATS, "%d", m_pProfile->totalCustomersTransported );

	m_Elements.ChangeElementDrawState( HUI_LABEL_PARCELS_CAREERSTATS, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_PARCELS_CAREERSTATS, true );
	m_Elements.ChangeElementText( HUI_TEXT_PARCELS_CAREERSTATS, "%d", m_pProfile->totalParcelsTransported );

	m_Elements.ChangeElementDrawState( HUI_LABEL_PERFECT_CAREERSTATS, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_PERFECT_CAREERSTATS, true );
	m_Elements.ChangeElementText( HUI_TEXT_PERFECT_CAREERSTATS, "%d", m_pProfile->perfectFares );

	m_Elements.ChangeElementDrawState( HUI_LABEL_TAXISLOST_CAREERSTATS, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_TAXISLOST_CAREERSTATS, true );
	m_Elements.ChangeElementText( HUI_TEXT_TAXISLOST_CAREERSTATS, "%d", m_pProfile->taxisLost );

	m_Elements.ChangeElementDrawState( HUI_LABEL_TOTAL_EARNINGS_CAREERSTATS, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_TOTAL_EARNINGS_CAREERSTATS, true );

	SetCashString( m_Elements, HUI_TEXT_TOTAL_EARNINGS_CAREERSTATS, m_pProfile->totalMoneyEarned );

	m_Elements.RefreshSelected();
}		

/////////////////////////////////////////////////////
/// Method: SetupCredits
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendUI::SetupCredits()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );
	m_Elements.ChangeElementText( HUI_LABEL_TITLE_ALL, "%s", res::GetScriptString(1013) );

	m_Elements.ChangeElementDrawState( HUI_LABEL_HIDDENLOGO, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_TITLE, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_NAMES, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_COPYRIGHT, true );

	m_UIMesh->SetMeshDrawState( BTN_BACK_CREDITS, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK_CREDITS, true );
	m_Elements.ChangeElementDrawState( HUI_BTN_BACK_CREDITS, true);

	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: UpdateMainMenu
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendUI::UpdateMainMenu( float deltaTime )
{
	if( m_Elements.CheckElementForTouch(HUI_BTN_CAREER_NEW_MAINMENU) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

#ifdef CABBY_LITE
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10000)*/);
#else
		ProfileManager::GetInstance()->SetGameMode( ProfileManager::CAREER_MODE );

		if( m_pProfile->careerActive &&
			!m_pProfile->careerGameOver )
		{
			ChangeState( new FrontendPromptsUI(*m_pStateManager, m_InputSystem, false) );
			return 1;
		}
		else
		{
			ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_TaxiSelect) );
			return 1;
		}
#endif // CABBY_LITE
	}
	else
	if( m_pProfile->careerActive &&
		!m_pProfile->careerGameOver &&
		m_Elements.CheckElementForTouch(HUI_BTN_CAREER_CONTINUE_MAINMENU) )
	{
#ifdef CABBY_LITE
		// not available
#else
		AudioSystem::GetInstance()->PlayUIAudio();

		ProfileManager::GetInstance()->SetGameMode( ProfileManager::CAREER_MODE );

		if( m_pProfile->newComplete )
		{
			// player has just last play completed the game, allow a new vehicle selection
			ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_TaxiSelect) );
			return 1;
		}
		else
		{
			// set to profile vehicle
			m_pScriptData->SetDefaultVehicle(m_pProfile->vehicleId);

			// where were we
			int index = m_pProfile->currentLevelIndex;
			int levelPack = m_pProfile->careerLevels[index].packIndex;
			int levelPackLevel = m_pProfile->careerLevels[index].levelIndex;

			script::LuaCallFunction( m_PackList[levelPack]->pPackLevelInfo[levelPackLevel].levelLoadFuncName, 0, 0 );
	
			ChangeState( new LevelStartUI(*m_pStateManager, m_InputSystem) );
			return 1;
		}
#endif // CABBY_LITE
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_ARCADE_MAINMENU) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ProfileManager::GetInstance()->SetGameMode( ProfileManager::ARCADE_MODE );

		ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_TaxiSelect) );
		return 1;
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_CAREER_STATS_MAINMENU) )
	{
#ifdef CABBY_LITE
		// not available
#else
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_CareerStats) );
		return 1;
#endif // CABBY_LITE
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_CREDITS_MAINMENU) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_Credits) );
		return 1;
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_TOGGLE_SOUND_MAINMENU) )
	{
		m_pProfile->sfxState = !m_pProfile->sfxState;

		AudioSystem::GetInstance()->SetSFXState( m_pProfile->sfxState );

		if( m_pProfile->sfxState )
		{
			m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_MAINMENU, true );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_OFF_MAINMENU, false );
		}
		else
		{
			AudioSystem::GetInstance()->StopAll();

			m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_MAINMENU, true );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_SFX_OFF_MAINMENU, true );
		}

		ProfileManager::GetInstance()->SaveProfile();
	}
	else
	if( core::app::IsGameCenterAvailable() && 
	   m_Elements.CheckElementForTouch(HUI_BTN_SHOW_ACHIEVEMENTS) )
	{
		/*if( core::app::IsNetworkAvailable() &&
		   support::Scores::GetInstance()->IsLoggedIn())*/
		{
			AudioSystem::GetInstance()->PlayUIAudio();
		
			support::Scores::GetInstance()->ShowAchievements();
		}
	}
	else
	if( core::app::IsStoreAvailable() && 
		m_Elements.CheckElementForTouch(HUI_BTN_RESTORE_PURCHASES) )
	{
		if( !core::app::IsPCOnly() )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
		
			ChangeState( new FrontendInAppPurchaseUI(*m_pStateManager, m_InputSystem, FrontendInAppPurchaseUI::FrontendInAppPurchaseStates_RestorePurchases, FrontendItemSelectUI::FrontendItemSelectStates_None, -1, 0, -1, -1 ) );
			return 1;
		}
	}	
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_RATE) )
	{
		if( core::app::GetAppRateURL() != 0 )
		{
			core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*core::app::GetAppRateURL()*/);
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_MOREGAMES) )
	{
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10003)*/);
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateCareerStats
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendUI::UpdateCareerStats( float deltaTime )
{
    if( m_Elements.CheckElementForTouch(HUI_BTN_BACK_CAREERSTATS) ||
       m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateCredits
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendUI::UpdateCredits( float deltaTime )
{
	if( m_Elements.CheckElementForTouch(HUI_LABEL_HIDDENLOGO) )
	{
#ifdef CABBY_LITE
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10002)*/);
#else
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10001)*/);
#endif // CABBY_LITE
	}
	else
        if( m_Elements.CheckElementForTouch(HUI_BTN_BACK_CREDITS) ||
           m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
		return 1;
	}

	return 0;
}
