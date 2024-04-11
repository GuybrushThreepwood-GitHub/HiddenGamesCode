
/*===================================================================
	File: FrontendMainMenuUI.cpp
	Game: AirCadets

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

#include "H1Consts.h"
#include "H1.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/IAPList.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendMainMenuUI.h"
#include "GameStates/UI/FrontendAircraftSelectUI.h"
#include "GameStates/UI/FrontendLevelSelectUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/frontend_mainmenu.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_mainmenu.hui.h"

namespace
{
	const float CADET_CHANGE_TIME = 5.0f;
	math::Vec4Lite cadetColour( 255,255,255,255 );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendMainMenuUI::FrontendMainMenuUI( StateManager& stateManager, InputSystem& inputSystem, FrontendStates state )
: IBaseGameState( stateManager, inputSystem, UI_FRONTENDMAINMENU )
, m_FrontendState(state)
{
	m_UIMesh = 0;

	m_pProfile = 0;
	m_pScriptData = 0;
	m_PackList.clear();
	m_pBestData = 0;

	m_ManualFade = false;
	m_NextFrontendState = state;
	m_ReturnFrontendState = state;

	m_CurrentCadet = 0;
	m_NextCadet = -1;
	m_CadetChangeTime = CADET_CHANGE_TIME;
	m_CadetAlpha = 255;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendMainMenuUI::~FrontendMainMenuUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1002);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_mainmenu.hui", srcDims, m_UIMesh );
	
	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pScriptData = GetScriptDataHolder();
	m_PackList = m_pScriptData->GetLevelPackList();
	m_GameData = m_pScriptData->GetGameData();
	m_DevData = m_pScriptData->GetDevData();

	m_WaitingForPurchaseFeedback = false;

	if( !m_pProfile->adsRemoved )
	{
		core::app::SetAdBarState( true );
	
		#ifdef BASE_PLATFORM_WINDOWS
			core::app::SetAdBarState(  m_DevData.showPCAdBar );
		#endif // BASE_PLATFORM_WINDOWS
	}

	// set the purchase callback
	support::Purchase::GetInstance()->SetCallback(this);

	switch( m_FrontendState )
	{
		case FrontendStates_MainMenu:
		{
			SetupMainMenu();
		}break;
		case FrontendStates_Credits:
		{
			SetupCredits();
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
	
	if( support::Scores::IsInitialised() )
		support::Scores::GetInstance()->SyncAchievements();
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
	
	m_Elements.Release();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendMainMenuUI::TransitionIn()
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
int FrontendMainMenuUI::TransitionOut()
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
void FrontendMainMenuUI::Update( float deltaTime )
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
void FrontendMainMenuUI::Draw()
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
						SetupMainMenu();
					}break;
					case FrontendStates_Credits:
					{
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
}

/////////////////////////////////////////////////////
/// Method: SetupMainMenu
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::SetupMainMenu()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_UIMesh->SetMeshDrawState( BG_SKY, true );
	m_UIMesh->SetMeshDrawState( GFX_BORDER_BOTTOM, true );
	m_UIMesh->SetMeshDrawState( GFX_LOGO, true );

	m_CurrentCadet = math::RandomNumber( 0, 5 );
	m_UIMesh->SetMeshDrawState( GFX_CADET_TAKU+m_CurrentCadet, true );

	m_UIMesh->SetMeshDrawState( BTN_GAMEMODE1, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_GAMEMODE1_MAINMENU, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_GAMEMODE1_MAINMENU, true);

	m_UIMesh->SetMeshDrawState( BTN_GAMEMODE2, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_GAMEMODE2_MAINMENU, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_GAMEMODE2_MAINMENU, true);

	m_UIMesh->SetMeshDrawState( BTN_GAMEMODE3, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_GAMEMODE3_MAINMENU, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_GAMEMODE3_MAINMENU, true);

	m_Elements.ChangeElementDrawState( HUI_BTN_CREDITS_MAINMENU, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_CREDITS_MAINMENU, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_CREDITS_MAINMENU, true);

	if (!core::app::IstvOS())
	{
		m_Elements.ChangeElementDrawState(HUI_BTN_RATE, true);
		m_Elements.ChangeElementDrawState(HUI_GLYPH_RATE, true);
		
		m_Elements.ChangeElementDrawState(HUI_BTN_MOREGAMES_MAINMENU, true);
		m_Elements.ChangeElementDrawState(HUI_LABEL_MOREGAMES_MAINMENU, true);
		m_UIMesh->SetMeshDrawState(BTN_MOREGAMES, true);
	}
	else
	{
		//shift icons so the more games button can be removed
		math::Vec3 rateBtnPos = m_Elements.GetElementPosition(HUI_BTN_RATE);
		math::Vec3 rateGlyphPos = m_Elements.GetElementPosition(HUI_GLYPH_RATE);

		math::Vec3 restoreBtnPos = m_Elements.GetElementPosition(HUI_BTN_RESTOREPURCHASES);
		math::Vec3 restoreGlyphPos = m_Elements.GetElementPosition(HUI_GLYPH_RESTOREPURCHASE);

		math::Vec3 achievementsBtnPos = m_Elements.GetElementPosition(HUI_BTN_SHOW_ACHIEVEMENTS);
		math::Vec3 achievementsGlyphPos = m_Elements.GetElementPosition(HUI_GLYPH_ACHIEVEMENTS_MAINMENU);

		m_Elements.ChangeElementPosition(HUI_BTN_SHOW_ACHIEVEMENTS, rateBtnPos);
		m_Elements.ChangeElementPosition(HUI_GLYPH_ACHIEVEMENTS_MAINMENU, rateGlyphPos);

		m_Elements.ChangeElementPosition(HUI_BTN_RESTOREPURCHASES, achievementsBtnPos);
		m_Elements.ChangeElementPosition(HUI_GLYPH_RESTOREPURCHASE, achievementsGlyphPos);

        m_Elements.ChangeElementDrawState(HUI_BTN_RATE, false);
        m_Elements.ChangeElementDrawState(HUI_GLYPH_RATE, false);
        
		m_Elements.ChangeElementDrawState(HUI_BTN_MOREGAMES_MAINMENU, false);
		m_Elements.ChangeElementDrawState(HUI_LABEL_MOREGAMES_MAINMENU, false);
		m_UIMesh->SetMeshDrawState(BTN_MOREGAMES, false);
	}

	m_Elements.ChangeElementDrawState( HUI_BTN_RESTOREPURCHASES, true ); 
	m_Elements.ChangeElementDrawState( HUI_GLYPH_RESTOREPURCHASE, true );

	/*if( core::app::IsGameCenterAvailable() &&
	   core::app::IsNetworkAvailable() )*/
	{
		m_Elements.ChangeElementDrawState( HUI_BTN_SHOW_ACHIEVEMENTS, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_ACHIEVEMENTS_MAINMENU, true );
	}

	/*if( core::app::IsStoreAvailable() )
	{
		if( !core::app::IsPCOnly() )
		{
			m_Elements.ChangeElementDrawState( HUI_BTN_RESTORE_PURCHASES, true );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_RESTORE_MAINMENU, true );
		}
	}*/

	if( m_pProfile->adsRemoved )
	{
		m_Elements.ChangeElementDrawState( HUI_GLYPH_REMOVEADS, false );
		m_Elements.ChangeElementDrawState(HUI_BTN_REMOVEADS, false);
	}
	else
	{
		m_Elements.ChangeElementDrawState( HUI_GLYPH_REMOVEADS, true );
		m_Elements.ChangeElementDrawState(HUI_BTN_REMOVEADS, true);
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

	m_Elements.RefreshSelected(HUI_BTN_GAMEMODE1_MAINMENU);
    
    core::app::SetHandleMenuButton(false);
}

/////////////////////////////////////////////////////
/// Method: SetupCredits
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::SetupCredits()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_UIMesh->SetMeshDrawState( BG_SKY, true );
	m_UIMesh->SetMeshDrawState( GFX_BORDER_BOTTOM, false );
	m_UIMesh->SetMeshDrawState( GFX_PANEL, true );

	m_Elements.ChangeElementDrawState( HUI_LABEL_HIDDENLOGO, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_TITLE_1, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_NAMES_1, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_TITLE_2, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_NAMES_2, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_MESSAGE, true );
	m_Elements.ChangeElementDrawState( HUI_LABEL_CREDITS_COPYRIGHT, true );

	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK_CREDITS, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_BACK_CREDITS, true);

	m_Elements.RefreshSelected(HUI_BTN_BACK_CREDITS);
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: UpdateMainMenu
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendMainMenuUI::UpdateMainMenu( float deltaTime )
{
	// do this first to lock out any changes until we have feedback
	if( m_Elements.CheckElementForTouch(HUI_BTN_REMOVEADS) )
	{
		if( m_pProfile->adsRemoved )
		{
			// do nothing
			m_WaitingForPurchaseFeedback = false;
		}
		else
		{
			// request iap
			m_WaitingForPurchaseFeedback = true;

			// request to purchase
			support::Purchase::GetInstance()->PurchaseItem(res::GetIAPString(0));
		}
	}
	else if( m_Elements.CheckElementForTouch(HUI_BTN_RESTOREPURCHASES) )
	{
		m_WaitingForPurchaseFeedback = true;

		support::Purchase::GetInstance()->RestorePurchases();
	}

	if( !m_WaitingForPurchaseFeedback )
	{
		if( m_Elements.CheckElementForTouch(HUI_BTN_GAMEMODE1_MAINMENU) )
		{		
			AudioSystem::GetInstance()->PlayUIAudio();

			GameSystems::GetInstance()->SetGameMode( GameSystems::GAMEMODE_FREEFLIGHT );

			ChangeState( new FrontendAircraftSelectUI(*m_pStateManager, m_InputSystem) );
			return 1;
		}
		else
		if( m_Elements.CheckElementForTouch(HUI_BTN_GAMEMODE2_MAINMENU) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			GameSystems::GetInstance()->SetGameMode( GameSystems::GAMEMODE_TARGET );

			ChangeState( new FrontendAircraftSelectUI(*m_pStateManager, m_InputSystem) );
			return 1;
		}
		else
		if( m_Elements.CheckElementForTouch(HUI_BTN_GAMEMODE3_MAINMENU) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			GameSystems::GetInstance()->SetGameMode( GameSystems::GAMEMODE_RINGRUN );

			ChangeState( new FrontendAircraftSelectUI(*m_pStateManager, m_InputSystem) );
			return 1;
		}
		else
		if( m_Elements.CheckElementForTouch(HUI_BTN_CREDITS_MAINMENU) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_Credits) );
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
		if( m_Elements.CheckElementForTouch( HUI_BTN_RATE ) )
		{
			if( core::app::GetAppRateURL() != 0 )
			{
				AudioSystem::GetInstance()->PlayUIAudio();

				core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*core::app::GetAppRateURL()*/);
			}
		}
		else
		if( m_Elements.CheckElementForTouch(HUI_BTN_MOREGAMES_MAINMENU) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
		
			core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10000)*/);
		}
	
		if( m_CadetChangeTime > 0.0f )
			m_CadetChangeTime -= deltaTime;

		if( m_CadetChangeTime <= 0.0f )
		{
			m_CadetChangeTime = 0.0f;

			if( m_NextCadet == -1 )
			{
				while( (m_NextCadet == -1) || (m_NextCadet == m_CurrentCadet) )
				{
					m_NextCadet = math::RandomNumber( 0, 5 );
					m_CadetAlpha = 255;
				}

				cadetColour.A = 255 - m_CadetAlpha;
				m_UIMesh->SetMeshDrawState( GFX_CADET_TAKU+m_NextCadet, true );
				m_UIMesh->SetMeshDefaultColour( GFX_CADET_TAKU+m_NextCadet, cadetColour );
				m_UIMesh->EnableMeshDefaultColour( GFX_CADET_TAKU+m_NextCadet );
			}
			else
			{
				m_CadetAlpha -= 5;
				if( m_CadetAlpha < 0 )
				{
					m_UIMesh->SetMeshDrawState( GFX_CADET_TAKU+m_CurrentCadet, false );
				
					m_CadetAlpha = 0;
					m_CadetChangeTime = CADET_CHANGE_TIME;
					m_CurrentCadet = m_NextCadet; 
					m_NextCadet = -1;
				}
				else
				{
					cadetColour.A = m_CadetAlpha;
					m_UIMesh->SetMeshDefaultColour( GFX_CADET_TAKU+m_CurrentCadet, cadetColour );
					m_UIMesh->EnableMeshDefaultColour( GFX_CADET_TAKU+m_CurrentCadet );

					cadetColour.A = 255 - m_CadetAlpha;
					m_UIMesh->SetMeshDefaultColour( GFX_CADET_TAKU+m_NextCadet, cadetColour );
					m_UIMesh->EnableMeshDefaultColour( GFX_CADET_TAKU+m_NextCadet );
				}

				if( m_NextCadet == -1 )
					m_CadetAlpha = 255;
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateCredits
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendMainMenuUI::UpdateCredits( float deltaTime )
{
	if( m_Elements.CheckElementForTouch(HUI_LABEL_HIDDENLOGO) )
	{
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10001)*/);
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_BACK_CREDITS) ||
       m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_MainMenu) );
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Success
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::Success( const char* purchaseId )
{
	DBGLOG( "FRONTENDUI: Purchase Success %s\n", purchaseId );

	int purchaseNumber = res::GetIAPIndex(purchaseId);
	switch( purchaseNumber )
	{
		case 0: // remove ads
		{
			m_pProfile->adsRemoved = true;

			core::app::SetAdBarState( false );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_REMOVEADS, false );

			ProfileManager::GetInstance()->SaveProfile();

		}break;

		default:
			DBG_ASSERT(0);
			break;
	};

	ProfileManager::GetInstance()->SaveProfile();
	
	// vehicle purchased/redeemed, go back to taxi select
	//m_FrontendState = FrontendInAppPurchaseStates_InAppPurchaseSuccess;
	//SetupInAppPurchaseSuccess();
	
	//m_PurchaseIndex = -1;

	m_WaitingForPurchaseFeedback = false;

}

/////////////////////////////////////////////////////
/// Method: Failure
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::Failure( const char* purchaseId, const char* errorString, int errorId )
{
	DBGLOG( "FRONTENDMAINMENUI: Purchase Failure %s\n", purchaseId );	
	
	m_WaitingForPurchaseFeedback = false;
	
#ifdef BASE_PLATFORM_iOS
	#ifdef BASE_PLATFORM_tvOS
        [core::app::GetViewController() showAlert:@"Purchase Error" message:@"Could not complete the purchase"];
	#else
		UIAlertView* alert= [[UIAlertView alloc] initWithTitle: @"Purchase Error" message: @"Could not complete the purchase"
                                                   delegate:nil cancelButtonTitle: @"OK" otherButtonTitles: NULL];
		[alert show];
	#endif
#endif // BASE_PLATFORM_iOS
}

/////////////////////////////////////////////////////
/// Method: Restored
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::Restored( const char* purchaseId )
{
	DBGLOG( "FRONTENDUI: Purchase Restored %s\n", purchaseId );	
	
	Success( purchaseId );
	
#ifdef BASE_PLATFORM_iOS
	#ifdef BASE_PLATFORM_tvOS
        [core::app::GetViewController() showAlert:@"Purchases Restored" message:@"Your purchases have successfully been restored"];
	#else
		UIAlertView* alert= [[UIAlertView alloc] initWithTitle: @"Purchases Restored" message: @"Your purchases have successfully been restored"
                                                   delegate:nil cancelButtonTitle: @"OK" otherButtonTitles: NULL];
		[alert show];
	#endif
#endif // BASE_PLATFORM_iOS
}

/////////////////////////////////////////////////////
/// Method: RestoreComplete
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::RestoreComplete()
{
	m_WaitingForPurchaseFeedback = false;	
}

/////////////////////////////////////////////////////
/// Method: RestoreFailed
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendMainMenuUI::RestoreFailed(int errorId)
{
	m_WaitingForPurchaseFeedback = false;
	
#ifdef BASE_PLATFORM_iOS
	#ifdef BASE_PLATFORM_tvOS
        [core::app::GetViewController() showAlert:@"Restore Error" message:@"Could not restore your purchases"];
	#else
		UIAlertView* alert= [[UIAlertView alloc] initWithTitle: @"Restore Error" message: @"Could not restore your purchases"
                                                   delegate: nil cancelButtonTitle: @"OK" otherButtonTitles: NULL];
		[alert show];
	#endif
#endif // BASE_PLATFORM_iOS
}

