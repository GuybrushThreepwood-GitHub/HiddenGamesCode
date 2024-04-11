
/*===================================================================
	File: FrontendItemSelectUI.cpp
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
#include "Resources/IAPList.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendItemSelectUI.h"
#include "GameStates/UI/FrontendInAppPurchaseUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/FrontendUI.h"

#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/frontend_itemselect.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_itemselect.hui.h"

namespace
{
	const int TOTAL_UI_SLOTS = 5;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendItemSelectUI::FrontendItemSelectUI( StateManager& stateManager, InputSystem& inputSystem, FrontendItemSelectStates state )
: IBaseGameState( stateManager, inputSystem, UI_FRONTENDITEMSELECT )
, m_FrontendState(state)
{
	m_UIMesh = 0;
	m_pScriptData = 0;

	m_VehiclePackList.clear();
	m_LevelPackList.clear();

	m_pArcadeBest = 0;

	m_LevelPackIndex = 0;
	m_LevelSelection = 0;
	m_SelectedLevelPack = 0;
	m_SelectedLevel = 0;
	m_ChangedPack = false;
	m_ChangedLevel = false;

	m_VehiclePackIndex = 0;
	m_VehicleSelection = 0;
	m_SelectedVehiclePack = 0;
	m_SelectedVehicle = 0;
	m_ChangedVehicle = false;

	m_ManualFade = false;
	m_NextFrontendState = state;
	m_ReturnFrontendState = state;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendItemSelectUI::~FrontendItemSelectUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendItemSelectUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1001);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_itemselect.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();

	m_VehiclePackList = m_pScriptData->GetVehiclePackList();
	m_LevelPackList = m_pScriptData->GetLevelPackList();

	switch( m_FrontendState )
	{
		case FrontendItemSelectStates_TaxiSelect:
		{
			SetupTaxiSelect();
		}break;			
		case FrontendItemSelectStates_LevelSelect:
		{
			SetupLevelSelect();
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendItemSelectUI::Exit()
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
void FrontendItemSelectUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendItemSelectUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendItemSelectUI::TransitionIn()
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
int FrontendItemSelectUI::TransitionOut()
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
void FrontendItemSelectUI::Update( float deltaTime )
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
		case FrontendItemSelectStates_TaxiSelect:
		{
			if( UpdateTaxiSelect(deltaTime) )
				return;
		}break;
		case FrontendItemSelectStates_LevelSelect:
		{
			if( UpdateLevelSelect(deltaTime) )
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
void FrontendItemSelectUI::Draw()
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
				m_FinishedTransitionOut = true;
				
				// get ready to fade in again
				PrepareTransitionIn();
				
				m_FrontendState = m_NextFrontendState;
				
				switch( m_FrontendState )
				{
					case FrontendItemSelectStates_TaxiSelect:
					{
						SetupTaxiSelect();
					}break;			
					case FrontendItemSelectStates_LevelSelect:
					{
						SetupLevelSelect();
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

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: SetupTaxiSelect
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendItemSelectUI::SetupTaxiSelect()
{
	int i=0, j=0;
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_Elements.ChangeElementDrawState(HUI_BTN_PREVIOUS_ITEMSELECT, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_NEXT_ITEMSELECT, true);

#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
	{
		if( i < m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount )
		{
			bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
			pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i;
			if( *pUnlockPointer )
			{
				// not locked
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, false  );
				m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
				m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

				// padlock off
				m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, false );

				// get the name of the taxi
				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s", res::GetScriptString(100+(m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i)  );
				m_Elements.ChangeElementDrawState(HUI_TEXT_NAME_1_ITEMSELECT + i, true);
			}
			else
			{
#ifdef CABBY_LITE
				if( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].liteVersionPurchase )
				{
					const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].purchaseId );
					if( purchaseString != 0 )
					{
						if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
						{				
							m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s %s",  m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].vehicleName, res::GetScriptString(53) );
							m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );
							m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
							m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);
							m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true );
						}
						else
						{
							m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s",  m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].vehicleName );
							m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, false );
							m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
							m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);
							m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, false );
						}
					}
				}
				else
				{
					m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true  );
					m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
					m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);
					// padlock on
					m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );

					//m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s", res::GetScriptString(52) );
					m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "----------"  );
				}
#else
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true  );
				m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
				m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

				// padlock on
				m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );

				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "??????" );
#endif // CABBY_LITE
			}
		}
		else
		{
			// empty slot
			m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, res::GetScriptString(52)  );
			m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "----------"  );

			// padlock on
			m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );

			m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
			m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, false);

			m_UIMesh->SetMeshDrawState(  ITEM_SLOT_LOCKED_1+i, true );
		}

		if( m_SelectedVehicle == i )
		{
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, false );
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, true );
		}
		else
		{
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, true );
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, false );
		}
	}

	m_UIMesh->SetMeshDrawState(  BTN_BACK_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_BACK_ITEMSELECT, true);

	m_UIMesh->SetMeshDrawState(  BTN_PREVIOUS_PAGE, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_MINUS_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_PREVIOUS_ITEMSELECT, true);

	m_UIMesh->SetMeshDrawState(  BG_PAGECOUNTER, true );
	m_UIMesh->SetMeshDrawState(  BTN_NEXT_PAGE, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_PLUS_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_NEXT_ITEMSELECT, true);

	m_UIMesh->SetMeshDrawState(  BTN_START_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, true);

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );
	m_Elements.ChangeElementText( HUI_LABEL_TITLE_ALL, "%s", res::GetScriptString(1080) );

	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_1_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_2_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_3_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_4_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_5_ITEMSELECT, true );

	m_Elements.ChangeElementDrawState( HUI_TEXT_PAGECOUNTER_ITEMSELECT, true );
	m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER_ITEMSELECT, "%s %d/%d", res::GetScriptString(1101), (m_VehiclePackIndex+1), m_GameData.TOTAL_VEHICLE_PACKS );

	m_Elements.RefreshSelected(HUI_BTN_START_ITEMSELECT/*HUI_BTN_START_ITEMSELECT*/);
}		

/////////////////////////////////////////////////////
/// Method: SetupLevelSelect
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendItemSelectUI::SetupLevelSelect()
{
	int i=0, j=0;
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_Elements.ChangeElementDrawState(HUI_BTN_PREVIOUS_ITEMSELECT, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_NEXT_ITEMSELECT, true);

#if !defined(CABBY_FREE)
	m_UIMesh->SetMeshDrawState( BG_TITLE_STRIP, true );
#endif // !CABBY_FREE

	int arcadeDataOffset = 0;

	for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
	{
		arcadeDataOffset = i*m_GameData.MAX_ITEMS_PER_PACK;

		for( j=1; j <= STAR_REWARDS; ++j )
			m_UIMesh->SetMeshDrawState( BG_STAR_SLOT1_1+arcadeDataOffset+(j-1), false );

		if( i < m_LevelPackList[m_LevelPackIndex]->packLevelCount )
		{
			bool isPurchased = true;

			// lite check
			if( m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchase )
			{
				const char* purchaseString = res::GetIAPString( m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchaseId );
				if( purchaseString != 0 )
				{
					if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
					{
						isPurchased = false;
				
						m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s %s",  m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].levelName, res::GetScriptString(53) );
						m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );
						m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
						m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

						m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true );
					}
				}
			}
			
			if( isPurchased )
			{
				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s",  m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].levelName );
				m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, false );
				m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
				m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

				// update best level
				m_pArcadeBest = ProfileManager::GetInstance()->GetArcadeBestData((m_LevelPackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i);
				DBG_ASSERT( m_pArcadeBest != 0 );

				// enable all backgrounds
				m_UIMesh->SetMeshDrawState( BG_ARCADESTARS_1+i, true );

				for( j=1; j <= STAR_REWARDS; ++j )
				{
					if( m_pArcadeBest->maxStars >= j )
						m_UIMesh->SetMeshDrawState( BG_STAR_SLOT1_1+arcadeDataOffset+(j-1), true );
				}

				m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, false );
			}
		}
		else
		{
			// empty slot
			m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, ""  );
			m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
			m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

			m_UIMesh->SetMeshDrawState(  ITEM_SLOT_LOCKED_1+i, true );
			m_UIMesh->SetMeshDrawState( BG_ARCADESTARS_1+i, false );
		}

		if( m_SelectedLevel == i )
		{
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, false );
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, true );
		}
		else
		{
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, true );
			m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, false );
		}
	}

	m_UIMesh->SetMeshDrawState(  BTN_BACK_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_BACK_ITEMSELECT, true);

	m_UIMesh->SetMeshDrawState(  BTN_PREVIOUS_PAGE, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_MINUS_ITEMSELECT, true );
	m_UIMesh->SetMeshDrawState(  BG_PAGECOUNTER, true );
	m_UIMesh->SetMeshDrawState(  BTN_NEXT_PAGE, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_PLUS_ITEMSELECT, true );

	m_UIMesh->SetMeshDrawState(  BTN_START_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, true);

	m_Elements.ChangeElementDrawState( HUI_LABEL_TITLE_ALL, true );
	m_Elements.ChangeElementText( HUI_LABEL_TITLE_ALL, "%s", res::GetScriptString(1100) );

	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_1_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_2_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_3_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_4_ITEMSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_5_ITEMSELECT, true );

	m_Elements.ChangeElementDrawState( HUI_TEXT_PAGECOUNTER_ITEMSELECT, true );
	m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER_ITEMSELECT, "%s %d/%d", res::GetScriptString(1101), (m_LevelPackIndex+1), m_GameData.TOTAL_LEVEL_PACKS );

	m_Elements.RefreshSelected(HUI_BTN_START_ITEMSELECT/*HUI_BTN_START_ITEMSELECT*/);

}

/////////////////////////////////////////////////////
/// Method: UpdateTaxiSelect
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendItemSelectUI::UpdateTaxiSelect( float deltaTime )
{
	int i=0, j=0;

	// check level select buttons
	for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
	{
		if( i < m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount )
		{
			bool isPurchased = true;

			if( !m_ChangedVehicle && 
				(i < m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount) )
			{
				if( m_Elements.CheckElementForTouch(HUI_BTN_SLOT_1_ITEMSELECT+i)  )
				{
#ifdef CABBY_LITE
					bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
					pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i;
					if( *pUnlockPointer )
					{
						m_ChangedVehicle = true;
						m_VehicleSelection = i;
						m_SelectedVehicle = i;
					}
					else
					{
						if( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].liteVersionPurchase )
						{
							const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].purchaseId );
							if( purchaseString != 0 )
							{
								if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
								{
									// launch in app
									ChangeState( new FrontendInAppPurchaseUI(*m_pStateManager, m_InputSystem, FrontendInAppPurchaseUI::FrontendInAppPurchaseStates_InAppPurchase, FrontendItemSelectStates_TaxiSelect, m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].purchaseId, 1, m_VehiclePackIndex, i ) );
									return 1;
								}
							}
						}
						else
						{
							// open web link in lite
							//core::app::OpenWebLink( res::GetScriptString(10000) );
						}
					}
#else
					bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
					pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i;
					if( *pUnlockPointer )
					{
						m_ChangedVehicle = true;
						m_VehicleSelection = i;
						m_SelectedVehicle = i;
					}
					else
					{
#ifndef BASE_PLATFORM_ANDROID // cannot purchase in android
						if( !core::app::IsPCOnly() )
						{
							const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].purchaseId );
							if( purchaseString != 0 )
							{
								if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
								{
									// launch in app
									ChangeState( new FrontendInAppPurchaseUI(*m_pStateManager, m_InputSystem, FrontendInAppPurchaseUI::FrontendInAppPurchaseStates_InAppPurchase, FrontendItemSelectStates_TaxiSelect, m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].purchaseId, 1, m_VehiclePackIndex, i ) );
									return 1;
								}
							}
						}
#endif // !BASE_PLATFORM_ANDROID
					}
#endif
				}
			}
		}
		else
		{
			if( m_Elements.CheckElementForTouch(HUI_BTN_SLOT_1_ITEMSELECT+i)  )
			{

			}
		}
	}

	if( m_Elements.CheckElementForTouch(HUI_BTN_PREVIOUS_ITEMSELECT) )
	{
		if( (m_VehiclePackIndex) > 0 )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_VehiclePackIndex--;
			m_ChangedPack = true;

			m_SelectedVehiclePack = m_VehiclePackIndex;
			m_SelectedVehicle = 0;

#ifdef CABBY_LITE
			bool isPurchased = true;

			// lite check
			if( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[m_SelectedVehicle].liteVersionPurchase )
			{
				const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[m_SelectedVehicle].purchaseId );
				if( purchaseString != 0 )
				{
					if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
					{
						m_SelectedVehicle = -1;
					}
				}
			}
			
			if( isPurchased )
			{
				m_SelectedVehicle = 0;
			}
#else
			bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
			pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+m_SelectedVehicle;
			if( *pUnlockPointer )
			{
				m_SelectedVehicle = 0;
			}
			else
			{
				m_SelectedVehicle = -1;
			}
#endif // CABBY_LITE

			if( m_SelectedVehicle == -1 )
			{
				m_UIMesh->SetMeshDrawState( BTN_START_ITEMSELECT, false );
				m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, false );
				m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, false);
			}
			else
			{
				m_UIMesh->SetMeshDrawState( BTN_START_ITEMSELECT, true );
				m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, true );
				m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, true);
			}
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_NEXT_ITEMSELECT) )
	{
		if( (m_VehiclePackIndex+1) < static_cast<int>( m_VehiclePackList.size()) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_VehiclePackIndex++;
			m_ChangedPack = true;

			m_SelectedVehiclePack = m_VehiclePackIndex;
			m_SelectedVehicle = 0;

#ifdef CABBY_LITE
			bool isPurchased = true;

			// lite check
			if( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[m_SelectedVehicle].liteVersionPurchase )
			{
				const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[m_SelectedVehicle].purchaseId );
				if( purchaseString != 0 )
				{
					if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
					{
						m_SelectedVehicle = -1;
					}
				}
			}
			
			if( isPurchased )
			{
				m_SelectedVehicle = 0;
			}
#else
			bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
			pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+m_SelectedVehicle;
			if( *pUnlockPointer )
			{
				m_SelectedVehicle = 0;
			}
			else
			{
				m_SelectedVehicle = -1;
			}
#endif // CABBY_LITE

			if( m_SelectedVehicle == -1 )
			{
				m_UIMesh->SetMeshDrawState( BTN_START_ITEMSELECT, false );
				m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, false );
				m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, false);
			}
			else
			{
				m_UIMesh->SetMeshDrawState( BTN_START_ITEMSELECT, true );
				m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, true );
				m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, true);
			}
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_START_ITEMSELECT) )
	{
		if( m_SelectedVehicle != -1 )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			if( VehicleSelected() )
				return 1 ;
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_BACK_ITEMSELECT) ||
       m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
		return 1;
	}

	if( m_ChangedPack )
	{
		m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER_ITEMSELECT, "%s %d/%d", res::GetScriptString(1101), (m_VehiclePackIndex+1), m_GameData.TOTAL_VEHICLE_PACKS );

		int vehicleDataOffset = 0;

		for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
		{
			vehicleDataOffset = i*m_GameData.MAX_ITEMS_PER_PACK;

			if( i < m_VehiclePackList[m_VehiclePackIndex]->packVehicleCount )
			{
#ifdef CABBY_LITE
				bool isPurchased = true;
				// lite check
				if( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].liteVersionPurchase )
				{
					const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].purchaseId );
					if( purchaseString != 0 )
					{
						if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
						{
							isPurchased = false;
				
							m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s %s",  m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].vehicleName, res::GetScriptString(53) );
							m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );
							m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
							m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

							m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true );
						}
					}
				}
			
				if( isPurchased )
				{
					m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s",  m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[i].vehicleName );
					m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, false );
					m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
					m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);
					m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, false );
				}
#else
				bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
				pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i;
				if( *pUnlockPointer )
				{
					// not locked
					m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, false  );
					m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
					m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

					// padlock off
					m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, false );

					// get the name of the taxi
					m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s", res::GetScriptString(100+(m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i)  );
				}
				else
				{
					m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true  );
					m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
					m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

					// padlock on
					m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );

					m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "??????" );
				}
#endif // CABBY_LITE

			}
			else
			{
				// empty slot
				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "----------" );
				m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
				m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);
				m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );
				m_UIMesh->SetMeshDrawState(  ITEM_SLOT_LOCKED_1+i, true );
			}
		}
		m_ChangedPack = false;

		// make sure first vehicle gets selected
		m_VehicleSelection = 0;
		m_ChangedVehicle = true;
	}

	if( m_ChangedVehicle )
	{
		for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
		{
			if( m_VehicleSelection == i )
			{
				// set the current level pack and level if it's unlocked
				m_SelectedVehiclePack = m_VehiclePackIndex;
				m_SelectedVehicle = i;
			
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, false );
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, true );
			}
			else
			{
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, true );
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, false );
			}
		}

#ifdef CABBY_LITE
		bool isPurchased = true;

		// lite check
		if( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[m_SelectedVehicle].liteVersionPurchase )
		{
			const char* purchaseString = res::GetIAPString( m_VehiclePackList[m_VehiclePackIndex]->pPackVehicleInfo[m_SelectedVehicle].purchaseId );
			if( purchaseString != 0 )
			{
				if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
				{
					m_SelectedVehicle = -1;
				}
			}
		}
#else
		bool* pUnlockPointer = &m_pProfile->yellowCabUnlocked;
		pUnlockPointer += (m_VehiclePackIndex*m_GameData.MAX_ITEMS_PER_PACK)+m_SelectedVehicle;
		if( *pUnlockPointer )
		{
			// valid
		}
		else
		{
			// not valid
			m_SelectedVehicle = -1;
		}
#endif // CABBY_LITE

		if( m_SelectedVehicle == -1 )
		{
			m_UIMesh->SetMeshDrawState( BTN_START_ITEMSELECT, false );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, false );
			m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, false);
		}
		else
		{
			m_UIMesh->SetMeshDrawState( BTN_START_ITEMSELECT, true );
			m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_ITEMSELECT, true );
			m_Elements.ChangeElementDrawState(HUI_BTN_START_ITEMSELECT, true);
		}

		m_ChangedVehicle = false;
	}

	return 0;
}	

/////////////////////////////////////////////////////
/// Method: SetupLevelSelect
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendItemSelectUI::UpdateLevelSelect( float deltaTime )
{
	int i=0, j=0;

	// check level select buttons
	for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
	{
		if( i < m_LevelPackList[m_LevelPackIndex]->packLevelCount )
		{
			bool isPurchased = true;

			if( !m_ChangedLevel && 
				(i < m_LevelPackList[m_LevelPackIndex]->packLevelCount) )
			{
				if( m_Elements.CheckElementForTouch(HUI_BTN_SLOT_1_ITEMSELECT+i)  )
				{
					if( m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchase )
					{
						const char* purchaseString = res::GetIAPString( m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchaseId );
						if( purchaseString != 0 )
						{
							if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
							{
								ChangeState( new FrontendInAppPurchaseUI(*m_pStateManager, m_InputSystem, FrontendInAppPurchaseUI::FrontendInAppPurchaseStates_InAppPurchase, FrontendItemSelectStates_LevelSelect, m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchaseId, 0, m_LevelPackIndex, i ) );
								return 1;
							}
						}
					}

					if( isPurchased )
					{
						m_ChangedLevel = true;
						m_LevelSelection = i;
						m_SelectedLevel = i;
					}
				}
			}
		}
	}

	if( m_Elements.CheckElementForTouch(HUI_BTN_PREVIOUS_ITEMSELECT) )
	{
		if( (m_LevelPackIndex) > 0 )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_LevelPackIndex--;
			m_ChangedPack = true;

			m_SelectedLevelPack = m_LevelPackIndex;
			m_SelectedLevel = 0;
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_NEXT_ITEMSELECT) )
	{
		if( (m_LevelPackIndex+1) < static_cast<int>( m_LevelPackList.size()) )
		{
			AudioSystem::GetInstance()->PlayUIAudio();

			m_LevelPackIndex++;
			m_ChangedPack = true;

			m_SelectedLevelPack = m_LevelPackIndex;
			m_SelectedLevel = 0;
		}
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_START_ITEMSELECT) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::ARCADE_MODE )
			ProfileManager::GetInstance()->SetArcadeModeLevel( m_SelectedLevelPack, m_SelectedLevel );

		script::LuaCallFunction( m_LevelPackList[m_SelectedLevelPack]->pPackLevelInfo[m_SelectedLevel].levelLoadFuncName, 0, 0 );
	
		ChangeState( new LevelStartUI(*m_pStateManager, m_InputSystem) );
		return 1 ;
	}
	else
        if( m_Elements.CheckElementForTouch(HUI_BTN_BACK_ITEMSELECT) ||
           m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_TaxiSelect) );
		return 1;
	}

	if( m_ChangedPack )
	{
		m_Elements.ChangeElementText( HUI_TEXT_PAGECOUNTER_ITEMSELECT, "%s %d/%d", res::GetScriptString(1101), (m_LevelPackIndex+1), m_GameData.TOTAL_LEVEL_PACKS );

		int arcadeDataOffset = 0;

		for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
		{
			arcadeDataOffset = i*m_GameData.MAX_ITEMS_PER_PACK;

			for( j=1; j <= STAR_REWARDS; ++j )
				m_UIMesh->SetMeshDrawState( BG_STAR_SLOT1_1+arcadeDataOffset+(j-1), false );

			if( i < m_LevelPackList[m_LevelPackIndex]->packLevelCount )
			{
				bool isPurchased = true;

				// lite check
				if( m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchase )
				{
					const char* purchaseString = res::GetIAPString( m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].liteVersionPurchaseId );
					if( purchaseString != 0 )
					{
						if( !ProfileManager::GetInstance()->HasBeenPurchased(purchaseString) )
						{
							isPurchased = false;
				
							m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s %s",  m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].levelName, res::GetScriptString(53) );
							m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, true );
							m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, false );
							m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

							m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, true );
						}
					}
				}
			
				if( isPurchased )
				{
					m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "%s",  m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].levelName );
					m_UIMesh->SetMeshDrawState( ICON_PADLOCK_1+i, false );
					m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
					m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

					// update best level
					m_pArcadeBest = ProfileManager::GetInstance()->GetArcadeBestData((m_LevelPackIndex*m_GameData.MAX_ITEMS_PER_PACK)+i);
					DBG_ASSERT( m_pArcadeBest != 0 );

					// enable all backgrounds
					m_UIMesh->SetMeshDrawState( BG_ARCADESTARS_1+i, true );

					for( j=1; j <= STAR_REWARDS; ++j )
					{
						if( m_pArcadeBest->maxStars >= j )
							m_UIMesh->SetMeshDrawState( BG_STAR_SLOT1_1+arcadeDataOffset+(j-1), true );
					}

					m_UIMesh->SetMeshDrawState( ITEM_SLOT_LOCKED_1+i, false );
				}
			}
			else
			{
				// empty slot
				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_ITEMSELECT+i, "" );
				m_Elements.ChangeElementDrawState( HUI_LABEL_1_ITEMSELECT+i, true );
				m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_ITEMSELECT + i, true);

				m_UIMesh->SetMeshDrawState(  ITEM_SLOT_LOCKED_1+i, true );
				m_UIMesh->SetMeshDrawState( BG_ARCADESTARS_1+i, false );
			}

			//pUnlockValue++;
		}
		m_ChangedPack = false;

		// make sure first level gets selected
		m_LevelSelection = 0;
		m_ChangedLevel = true;
	}

	if( m_ChangedLevel )
	{
		for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
		{
			if( m_LevelSelection == i )
			{
				// set the current level pack and level if it's unlocked
				m_SelectedLevelPack = m_LevelPackIndex;
				m_SelectedLevel = i;
			
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, false );
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, true );
			}
			else
			{
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_OFF_1+i, true );
				m_UIMesh->SetMeshDrawState( ITEM_SLOT_ON_1+i, false );
			}
		}

		m_ChangedLevel = false;
	}

	return 0;
}

/////////////////////////////////////////////////////
/// Method: VehicleSelected
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendItemSelectUI::VehicleSelected()
{
	int i=0, j=0, k=0;

	AudioSystem::GetInstance()->PlayUIAudio();

	// STORE SELECTED VEHICLE
	int vehicleId = (m_SelectedVehiclePack*m_GameData.MAX_ITEMS_PER_PACK) + m_SelectedVehicle;

	m_pScriptData->SetDefaultVehicle(vehicleId);

	// selected vehicle, now start career mode
	if( ProfileManager::GetInstance()->GetGameMode() == ProfileManager::CAREER_MODE )
	{
		m_pProfile->vehicleId = vehicleId;
		// now in use
		m_pProfile->careerActive = true;
		m_pProfile->careerGameOver = false;
		m_pProfile->newComplete = false;

		// generate the levels
		
		std::vector<int> levelIds;
		std::vector<int> usedIds;

		// now pick LEVELS_IN_A_CAREER 
		for( i=0; i<LEVELS_IN_A_CAREER; ++i )
		{
			// put every level in the list
			for( j=0; j<m_GameData.TOTAL_LEVELS; ++j )
			{
				bool okToAdd = true;

				for( k=0; k < static_cast<int>( usedIds.size() ); ++k )
				{
					if( j == usedIds[k] )
						okToAdd = false;
				}

				if( okToAdd )
					levelIds.push_back(j);
			}

			// get an index
			int whichIndex = math::RandomNumber( 0, static_cast<int>(levelIds.size()-1) );
			int value = levelIds[whichIndex];

			m_pProfile->careerLevels[i].complete = false;
			m_pProfile->careerLevels[i].packIndex = value / m_GameData.MAX_ITEMS_PER_PACK;
			m_pProfile->careerLevels[i].levelIndex = value % m_GameData.MAX_ITEMS_PER_PACK;

			usedIds.push_back(value);
			levelIds.clear();
		}

		m_pProfile->currentLevelIndex = 0;
		levelIds.clear();
		usedIds.clear();

		ProfileManager::GetInstance()->SaveProfile();

		// set the first level up
		int levelPack = m_pProfile->careerLevels[m_pProfile->currentLevelIndex].packIndex;
		int levelId = m_pProfile->careerLevels[m_pProfile->currentLevelIndex].levelIndex;

		script::LuaCallFunction( m_pScriptData->GetLevelPackList()[levelPack]->pPackLevelInfo[levelId].levelLoadFuncName, 0, 0 );

		// start the game	
		ChangeState( new LevelStartUI(*m_pStateManager, m_InputSystem) );
		return 1;
	}
	else
	{
		ProfileManager::GetInstance()->SetArcadeModeVehicle(vehicleId);

		// arcade mode allows selection of level	
		ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_LevelSelect) );

		//SetupLevelSelect();
		//ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_LevelSelect) );
		return 1;
	}
}
