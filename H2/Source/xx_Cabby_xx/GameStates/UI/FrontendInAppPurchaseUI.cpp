
/*===================================================================
	File: FrontendInAppPurchaseUI.cpp
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
#include "SupportBase.h"

#include "CabbyConsts.h"
#include "Cabby.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"
#include "Resources/IAPList.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/TitleScreenState.h"
#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendInAppPurchaseUI.h"
#include "GameStates/UI/FrontendItemSelectUI.h"
#include "GameStates/UI/FrontendUI.h"

#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/frontend_inapppurchase.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_inapppurchase.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendInAppPurchaseUI::FrontendInAppPurchaseUI( StateManager& stateManager, InputSystem& inputSystem, 
													FrontendInAppPurchaseStates state, FrontendItemSelectUI::FrontendItemSelectStates returnState, 
													int purchaseId, int levelOrVehicle, int packIndex, int itemIndex )
: IBaseGameState( stateManager, inputSystem, UI_FRONTENDINAPPPURCHASES )
, m_FrontendState(state)
, m_NextFrontendState(state)
, m_LevelOrVehicle(levelOrVehicle)
, m_PackIndex(packIndex)
, m_ItemIndex(itemIndex)
{
	m_UIMesh = 0;
	m_pScriptData = 0;

	m_PurchaseIndex = purchaseId;
	m_StoreErrorCode = -1;

	m_ManualFade = false;
	m_ReturnState = returnState;

	m_WaitForModelLoad = false;
	m_RequestLoad = false;
	
	if( m_LevelOrVehicle == 0 ) // levels don't do pre reveal text
		m_Revealed = true;
	else
		m_Revealed = false;

	if( state == FrontendInAppPurchaseStates_RestorePurchases )
	{
		m_RestorePurchases = true;
	}
	else
	{
		m_RestorePurchases = false;
	}
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendInAppPurchaseUI::~FrontendInAppPurchaseUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::Enter()
{
	//renderer::OpenGL::GetInstance()->ClearColour( 0.1f, 0.1f, 0.1f, 1.0f );

	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1015);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_inapppurchase.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();

	// set the purchase callback
	support::Purchase::GetInstance()->SetCallback(this);

	switch( m_FrontendState )
	{
		case FrontendInAppPurchaseStates_RestorePurchases:
		{
			SetupRestorePuchases();
		}break;				
		case FrontendInAppPurchaseStates_InAppPurchase:
		{
			SetupInAppPurchase(m_Revealed);
		}break;	
		case FrontendInAppPurchaseStates_InAppPurchaseSuccess:
		{
			SetupInAppPurchaseSuccess();
		}break;	
		case FrontendInAppPurchaseStates_InAppPurchaseFailure:
		{
			SetupInAppPurchaseFailure();
		}break;			
		case FrontendInAppPurchaseStates_InAppPurchaseWaiting:
		{
			SetupInAppPurchaseWaiting();
		}break;				
		default:
			DBG_ASSERT(0);
			break;
	}

	// call main state show models
	TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

	if( m_RestorePurchases )
		pState->SetShowModelState( true, 0 );	
	else
	{
		if( m_LevelOrVehicle == 0 )
			pState->SetShowModelState( true, 1 );
		else if( m_LevelOrVehicle == 1 )
			pState->SetShowModelState( true, -1 );
	}
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::Exit()
{
	//renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}

	// clear the purchase callback
	support::Purchase::GetInstance()->SetCallback(0);

	// call main state show models
	TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());
	if( pState )
	{
		pState->SetShowModelState( false, -1 );

		pState->ClearVehicleModel();
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendInAppPurchaseUI::TransitionIn()
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
int FrontendInAppPurchaseUI::TransitionOut()
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
void FrontendInAppPurchaseUI::Update( float deltaTime )
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

	switch( m_FrontendState )
	{
		case FrontendInAppPurchaseStates_RestorePurchases:
		{
			if( UpdateRestorePurchases(deltaTime) )
				return;
		}break;				
		case FrontendInAppPurchaseStates_InAppPurchase:
		{
			if( UpdateInAppPurchase(deltaTime) )
				return;
		}break;			
		case FrontendInAppPurchaseStates_InAppPurchaseSuccess:
		{
			if( UpdateInAppPurchaseSuccess(deltaTime) )
				return;
		}break;			
		case FrontendInAppPurchaseStates_InAppPurchaseFailure:
		{
			if( UpdateInAppPurchaseFailure(deltaTime) )
				return;
		}break;	
		case FrontendInAppPurchaseStates_InAppPurchaseWaiting:
		{
			if( UpdateInAppPurchaseWaiting(deltaTime) )
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
void FrontendInAppPurchaseUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	renderer::OpenGL::GetInstance()->SetColour4ub(255,255,255,255);
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

				if( m_RequestLoad == false )
				{
					// call main state to load
					TitleScreenState* pState = static_cast<TitleScreenState*>(GetStateManager()->GetSecondaryStateManager()->GetCurrentState());

					pState->LoadRequest( m_PackIndex, m_ItemIndex );
					m_RequestLoad = true;
				}

				if( !m_WaitForModelLoad )
				{
					m_FinishedTransitionOut = true;
				
					// get ready to fade in again
					PrepareTransitionIn();
				
					m_FrontendState = m_NextFrontendState;
				
					switch( m_FrontendState )
					{
						case FrontendInAppPurchaseStates_RestorePurchases:
						{
							SetupRestorePuchases();
						}break;				
						case FrontendInAppPurchaseStates_InAppPurchase:
						{
							SetupInAppPurchase(m_Revealed);
						}break;	
						case FrontendInAppPurchaseStates_InAppPurchaseSuccess:
						{
							SetupInAppPurchaseSuccess();
						}break;	
						case FrontendInAppPurchaseStates_InAppPurchaseFailure:
						{
							SetupInAppPurchaseFailure();
						}break;			
						case FrontendInAppPurchaseStates_InAppPurchaseWaiting:
						{
							SetupInAppPurchaseWaiting();
						}break;				
						default:
							DBG_ASSERT(0);
							break;
					}	
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

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: SetupRestorePuchases
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::SetupRestorePuchases()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState(false);
	
#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );

	m_UIMesh->SetMeshDrawState( BG_MESSAGE_REVEALED, true );
	m_UIMesh->SetMeshDrawState( BTN_NO_REVEALED, true );
	m_UIMesh->SetMeshDrawState( BTN_YES_REVEALED, true );
		
	m_Elements.ChangeElementDrawState( HUI_LABEL_NO_REVEALED, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_NO_REVEALED, true);

	m_Elements.ChangeElementDrawState( HUI_LABEL_YES_REVEALED, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_YES_REVEALED, true);

	m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1330));
	m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
	m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_REVEALED, "%s", res::GetScriptString(1331));
	m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_REVEALED, true );
	
	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: SetupInAppPurchase
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::SetupInAppPurchase( bool revealed )
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState(false);

#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );

	if( revealed )
	{
		m_UIMesh->SetMeshDrawState( BG_MESSAGE_REVEALED, true );
		m_UIMesh->SetMeshDrawState( BTN_NO_REVEALED, true );
		m_UIMesh->SetMeshDrawState( BTN_YES_REVEALED, true );
		
		m_Elements.ChangeElementDrawState( HUI_LABEL_NO_REVEALED, true );
		m_Elements.ChangeElementDrawState( HUI_LABEL_YES_REVEALED, true );

		m_Elements.ChangeElementDrawState(HUI_BTN_NO_REVEALED, true);
		m_Elements.ChangeElementDrawState(HUI_BTN_YES_REVEALED, true);

#if defined(CABBY_LITE) 
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1334));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_2_REVEALED, "%s", res::GetScriptString(1335));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_2_REVEALED, false );

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1336));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_REVEALED, "%s", res::GetScriptString(1337));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_REVEALED, true );
#elif defined(CABBY_FREE)
		if( m_PurchaseIndex < 5 ) 
		{
			// normal unlock or purchase
			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1320));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_2_REVEALED, "%s", res::GetScriptString(1321));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_2_REVEALED, true );

			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1322));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );

			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_REVEALED, "%s", res::GetScriptString(1323));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_REVEALED, true );
		}
		else
		{
			// purchase only
			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1342));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_2_REVEALED, "%s", res::GetScriptString(1343));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_2_REVEALED, false );

			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1344));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );

			m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_REVEALED, "%s", res::GetScriptString(1345));
			m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_REVEALED, true );
		}
#else
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1320));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_2_REVEALED, "%s", res::GetScriptString(1321));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_2_REVEALED, true );

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1322));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_REVEALED, "%s", res::GetScriptString(1323));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_REVEALED, true );
#endif // 
	}
	else
	{
		m_UIMesh->SetMeshDrawState( BG_MESSAGE_HIDDEN, true );
		m_UIMesh->SetMeshDrawState( BTN_BACK_HIDDEN, true );
		m_UIMesh->SetMeshDrawState( BTN_REVEAL_HIDDEN, true );

		m_Elements.ChangeElementDrawState( HUI_LABEL_YES_HIDDEN, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK_HIDDEN, true );

		m_Elements.ChangeElementDrawState(HUI_BTN_BACK_HIDDEN, true);
		m_Elements.ChangeElementDrawState(HUI_BTN_REVEAL_HIDDEN, true);

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_HIDDEN, "%s", res::GetScriptString(1338));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_HIDDEN, true );
	
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_2_HIDDEN, "%s", res::GetScriptString(1339));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_2_HIDDEN, true );

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_HIDDEN, "%s", res::GetScriptString(1340));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_HIDDEN, true );

		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_HIDDEN, "%s", res::GetScriptString(1341));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_HIDDEN, true );
	}

	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: SetupInAppPurchaseSuccess
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::SetupInAppPurchaseSuccess()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState(false);
	
#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );

	m_UIMesh->SetMeshDrawState( BG_MESSAGE_REVEALED, true );

	if( m_RestorePurchases )
	{
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1332));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
		
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1328));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );		
	}
	else 
	{
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1326));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1328));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );
	}

	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: SetupInAppPurchaseFailure
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::SetupInAppPurchaseFailure()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState(false);
	
#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );

	m_UIMesh->SetMeshDrawState( BG_MESSAGE_REVEALED, true );

	if( m_RestorePurchases )
	{
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1333));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
		
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1328));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );		
	}
	else 
	{
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1327));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
		
		m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1328));
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );
	}		
	
	m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_RED_REVEALED, "%s %d", res::GetScriptString(1329), m_StoreErrorCode);
	m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED_REVEALED, true );

	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: SetupInAppPurchaseWaiting
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::SetupInAppPurchaseWaiting()
{
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState(false);
	
#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );

	m_UIMesh->SetMeshDrawState( BG_MESSAGE_REVEALED, true );

	m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_1_REVEALED, "%s", res::GetScriptString(1324));
	m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_1_REVEALED, true );
	
	m_Elements.ChangeElementText(HUI_TEXT_MESSAGE_LINE_3_REVEALED, "%s", res::GetScriptString(1325));
	m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_LINE_3_REVEALED, true );

	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: UpdateInAppPurchase
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendInAppPurchaseUI::UpdateRestorePurchases( float deltaTime )
{

    if( m_Elements.CheckElementForSingleTouch(HUI_BTN_NO_REVEALED) ||
       m_InputSystem.GetMenu())
	{
		AudioSystem::GetInstance()->PlayUIAudio();
		
		// user doesn't want to restore, just exit	
		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
		return 1;
	}
	else if( m_Elements.CheckElementForSingleTouch(HUI_BTN_YES_REVEALED) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();
		
		// now waiting for a store response, show waiting?
		
		m_FrontendState = FrontendInAppPurchaseStates_InAppPurchaseWaiting;
		SetupInAppPurchaseWaiting();

		support::Purchase::GetInstance()->RestorePurchases();
		return 0;
	}
	
	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateInAppPurchase
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendInAppPurchaseUI::UpdateInAppPurchase( float deltaTime )
{
	if( !m_Revealed )
	{
		if( m_Elements.CheckElementForSingleTouch(HUI_BTN_BACK_HIDDEN) ||
           m_InputSystem.GetMenu() )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			
			ChangeState( new FrontendItemSelectUI( *m_pStateManager, m_InputSystem, m_ReturnState ) );
			return 1;
		}
		else if( m_Elements.CheckElementForSingleTouch(HUI_BTN_REVEAL_HIDDEN) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();
			
			// now waiting for a store response, show waiting?
			m_Revealed = true;

			m_ManualFade = true;

			if( m_LevelOrVehicle == 0 )
			{
				// level load, do nothing
				m_WaitForModelLoad = false;
				m_RequestLoad = true;
			}
			else
			{
				// vehicle load
				m_WaitForModelLoad = true;
				m_RequestLoad = false;
			}

			m_NextFrontendState = FrontendInAppPurchaseStates_InAppPurchase;
			return 0;
		}
	}
	else
	{
		if( m_PurchaseIndex )
		{
            if( m_Elements.CheckElementForSingleTouch(HUI_BTN_NO_REVEALED) ||
               m_InputSystem.GetMenu())
			{
				AudioSystem::GetInstance()->PlayUIAudio();
			
				ChangeState( new FrontendItemSelectUI( *m_pStateManager, m_InputSystem, m_ReturnState ) );
				return 1;
			}
			else if( m_Elements.CheckElementForSingleTouch(HUI_BTN_YES_REVEALED) )
			{
				AudioSystem::GetInstance()->PlayUIAudio();
			
				// now waiting for a store response, show waiting?
			
				m_NextFrontendState = FrontendInAppPurchaseStates_InAppPurchaseWaiting;
				SetupInAppPurchaseWaiting();

				support::Purchase::GetInstance()->PurchaseItem(res::GetIAPString(m_PurchaseIndex));
				return 0;
			}
		}
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateInAppPurchaseSuccess
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendInAppPurchaseUI::UpdateInAppPurchaseSuccess( float deltaTime )
{
	// tap to continue
	if (input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease ||
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease)
	{
		AudioSystem::GetInstance()->PlayUIAudio();
		
		if( m_RestorePurchases )
		{
			// back to the main menu
			ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
			return 1;			
		}
		else 
		{
			// back to the taxi select
			ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, m_ReturnState) );
			return 1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateInAppPurchaseFailure
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendInAppPurchaseUI::UpdateInAppPurchaseFailure( float deltaTime )
{
	// tap to continue
	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease ||
		input::gInputState.TouchesData[input::SECOND_TOUCH].bRelease )
	{
		AudioSystem::GetInstance()->PlayUIAudio();
		
		if( m_RestorePurchases )
		{
			// back to the main menu
			ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
			return 1;			
		}
		else 	
		{
			// back to return state
			ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, m_ReturnState) );
			return 1;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: UpdateInAppPurchaseWaiting
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendInAppPurchaseUI::UpdateInAppPurchaseWaiting( float deltaTime )
{
	return 0;
}

/////////////////////////////////////////////////////
/// Method: Success
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::Success( const char* purchaseId )
{
	DBGLOG( "FRONTENDUI: Purchase Success %s\n", purchaseId );

#ifdef CABBY_LITE
	ProfileManager::GetInstance()->AddToPurchases( purchaseId );

	int purchaseNumber = res::GetIAPIndex(purchaseId);
	switch( purchaseNumber )
	{
		case 4: // hot rod
		{
			m_pProfile->hotRodUnlocked = true;
		}break;

		case 100: // level 1
		case 101: // level 2
		break;

		default:
			DBG_ASSERT(0);
			break;
	};
#elif CABBY_FREE
	// write the unlock to the profile
	bool* pUnlock = &m_pProfile->yellowCabUnlocked;
	pUnlock +=  res::GetIAPIndex(purchaseId);
	*pUnlock = true;
#else
	// write the unlock to the profile
	bool* pUnlock = &m_pProfile->yellowCabUnlocked;
	pUnlock +=  res::GetIAPIndex(purchaseId);
	*pUnlock = true;
#endif // 	CABBY_LITE

	ProfileManager::GetInstance()->SaveProfile();
	
	// vehicle purchased/redeemed, go back to taxi select
	m_FrontendState = FrontendInAppPurchaseStates_InAppPurchaseSuccess;
	SetupInAppPurchaseSuccess();
	
	m_PurchaseIndex = -1;

}

/////////////////////////////////////////////////////
/// Method: Failure
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::Failure( const char* purchaseId, const char* errorString, int errorId )
{
	DBGLOG( "FRONTENDUI: Purchase Failure %s\n", purchaseId );	
	
	if( errorId == -1 )
	{
		if( m_RestorePurchases )
		{
			// user didn't want to purchase
			ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
			return;
		}
		else 
		{
			// user didn't want to purchase
			ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_TaxiSelect) );		
			return;
		}
	}
	else 
	{
		//
		m_StoreErrorCode = errorId;
		
		m_FrontendState = FrontendInAppPurchaseStates_InAppPurchaseFailure;
		SetupInAppPurchaseFailure();
	}
}

/////////////////////////////////////////////////////
/// Method: Restored
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::Restored( const char* purchaseId )
{
	DBGLOG( "FRONTENDUI: Purchase Restored %s\n", purchaseId );	
	
	Success( purchaseId );
}

/////////////////////////////////////////////////////
/// Method: RestoreComplete
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::RestoreComplete()
{
	// go to the success dailog
	m_FrontendState = FrontendInAppPurchaseStates_InAppPurchaseSuccess;
	SetupInAppPurchaseSuccess();		
}

/////////////////////////////////////////////////////
/// Method: RestoreFailed
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendInAppPurchaseUI::RestoreFailed(int errorId)
{
	// go to the fail dailog
	m_StoreErrorCode = errorId;
	
	m_FrontendState = FrontendInAppPurchaseStates_InAppPurchaseFailure;
	SetupInAppPurchaseFailure();		
}
