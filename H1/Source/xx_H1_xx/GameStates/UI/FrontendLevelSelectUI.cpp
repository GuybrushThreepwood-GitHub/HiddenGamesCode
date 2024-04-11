
/*===================================================================
	File: FrontendLevelSelectUI.cpp
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
#include "Resources/IAPList.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendAircraftSelectUI.h"
#include "GameStates/UI/FrontendLevelSelectUI.h"
#include "GameStates/UI/LevelStartUI.h"
#include "GameStates/UI/FrontendMainMenuUI.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/frontend_levelselect.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_levelselect.hui.h"

namespace
{
	const int TOTAL_UI_SLOTS = 4;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendLevelSelectUI::FrontendLevelSelectUI( StateManager& stateManager, InputSystem& inputSystem, FrontendItemSelectStates state )
: IBaseGameState( stateManager, inputSystem, UI_FRONTENDLEVELSELECT )
, m_FrontendState(state)
{
	m_UIMesh = 0;
	m_pScriptData = 0;

	m_VehiclePackList.clear();
	m_LevelPackList.clear();

	m_pBestData = 0;

	m_LayoutIndex = 0;

	m_LevelPackIndex = 0;
	m_LevelSelection = 0;
	m_SelectedLevelPack = 0;
	m_SelectedLevel = 0;
	m_ChangedLayout = false;
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
FrontendLevelSelectUI::~FrontendLevelSelectUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendLevelSelectUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1001);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_levelselect.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	m_pScriptData = GetScriptDataHolder();
	m_GameData = m_pScriptData->GetGameData();

	m_VehiclePackList = m_pScriptData->GetVehiclePackList();
	m_LevelPackList = m_pScriptData->GetLevelPackList();

	switch( m_FrontendState )
	{		
		case FrontendItemSelectStates_LevelSelect:
		{
			SetupLevelSelect();
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
	
	if( !m_pProfile->adsRemoved )
	{
		core::app::SetAdBarState( true );
		#ifdef BASE_PLATFORM_WINDOWS
			core::app::SetAdBarState(  m_pScriptData->GetDevData().showPCAdBar );
		#endif // BASE_PLATFORM_WINDOWS
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendLevelSelectUI::Exit()
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
void FrontendLevelSelectUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendLevelSelectUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendLevelSelectUI::TransitionIn()
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
int FrontendLevelSelectUI::TransitionOut()
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
void FrontendLevelSelectUI::Update( float deltaTime )
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
void FrontendLevelSelectUI::Draw()
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

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


/////////////////////////////////////////////////////
/// Method: SetupLevelSelect
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendLevelSelectUI::SetupLevelSelect()
{
	int i=0;
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_GameMode = GameSystems::GetInstance()->GetGameMode();

	m_UIMesh->SetMeshDrawState( GFX_BG_SKY, true );
	m_UIMesh->SetMeshDrawState( GFX_BORDER_BOTTOM, true );

	m_Elements.ChangeElementDrawState( HUI_GFX_BG_PAGINATION, true );
	m_Elements.ChangeElementDrawState( HUI_GFX_BG_LAYOUT, true );

	m_UIMesh->SetMeshDrawState( GFX_BG_PAGINATION, true );
	m_UIMesh->SetMeshDrawState( GFX_BG_LAYOUT, true );
	m_UIMesh->EnableMeshDefaultColour(GFX_BG_LAYOUT);
	
	if( m_GameMode == GameSystems::GAMEMODE_RINGRUN ||
	   m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		math::Vec4Lite col( 255,255,255,255 );
		m_UIMesh->SetMeshDefaultColour(GFX_BG_LAYOUT, col);
	}
	else
	{
		math::Vec4Lite col( 255,255,255,32);
		m_UIMesh->SetMeshDefaultColour(GFX_BG_LAYOUT, col);
	}
	
	for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
	{
		if( i < m_LevelPackList[m_LevelPackIndex]->packLevelCount )
		{
			m_Elements.ChangeElementText( HUI_TEXT_NAME_1_LEVELSELECT+i, "%s",  m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].levelName );

			ProfileManager::LevelBest bestScore;

			// update best level
			if( m_GameMode == GameSystems::GAMEMODE_RINGRUN ||
				m_GameMode == GameSystems::GAMEMODE_TARGET )
			{
				if( ProfileManager::GetInstance()->GetScore( m_LevelPackIndex, i, m_GameMode, m_LayoutIndex, &bestScore ) )
				{
					m_Elements.ChangeElementDrawState( HUI_TEXT_BEST_1_LEVELSELECT+i, true );

					if( m_GameMode == GameSystems::GAMEMODE_RINGRUN )
					{
						char text[UI_MAXSTATICTEXTBUFFER_SIZE];

						int minAsInt = static_cast<int>(bestScore.bestTime / 60.0f);
						int secsAsInt = static_cast<int>(bestScore.bestTime) % 60;
						int milliSecsAsInt = static_cast<int>( (bestScore.bestTime-(static_cast<float>(minAsInt)*60.0f)-static_cast<float>(secsAsInt)) * 100.0f );

						if( minAsInt > 99 )
							minAsInt = 99;

						if( secsAsInt < 10 )
						{
							if( milliSecsAsInt < 10 )
								snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
							else
								snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
						}
						else
						{
							if( milliSecsAsInt < 10 )
								snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
							else
								snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
						}

						m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s %s" , res::GetScriptString(1102), text );

					}
					else if( m_GameMode == GameSystems::GAMEMODE_TARGET )
					{
						if( bestScore.targetsDestroyed == bestScore.targetsTotal )
						{
							char text[UI_MAXSTATICTEXTBUFFER_SIZE];

							int minAsInt = static_cast<int>(bestScore.bestTime / 60.0f);
							int secsAsInt = static_cast<int>(bestScore.bestTime) % 60;
							int milliSecsAsInt = static_cast<int>( (bestScore.bestTime-(static_cast<float>(minAsInt)*60.0f)-static_cast<float>(secsAsInt)) * 100.0f );

							if( minAsInt > 99 )
								minAsInt = 99;

							if( secsAsInt < 10 )
							{
								if( milliSecsAsInt < 10 )
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
								else
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
							}
							else
							{
								if( milliSecsAsInt < 10 )
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
								else
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
							}

							m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s %d / %d %s %s" , res::GetScriptString(1102), bestScore.targetsDestroyed, bestScore.targetsTotal, res::GetScriptString(1103), text );
						}
						else
							m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s %d / %d", res::GetScriptString(1102), bestScore.targetsDestroyed, bestScore.targetsTotal );
					}
				}
				else
				{
					m_Elements.ChangeElementDrawState( HUI_TEXT_BEST_1_LEVELSELECT+i, true );

					if( m_GameMode == GameSystems::GAMEMODE_RINGRUN )
						m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s --:--", res::GetScriptString(1102) );
					else if( m_GameMode == GameSystems::GAMEMODE_TARGET )
						m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s --/--", res::GetScriptString(1102) );
				}
			}
			else
			{
				m_Elements.ChangeElementDrawState( HUI_TEXT_BEST_1_LEVELSELECT+i, false );
			}
		}
		else
		{
			// empty slot
			m_Elements.ChangeElementText( HUI_TEXT_NAME_1_LEVELSELECT+i, ""  );
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

	m_Elements.ChangeElementDrawState( HUI_GLYPH_BACK, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT, true );

	m_Elements.ChangeElementDrawState(HUI_BTN_BACK, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_NEXT, true);

	m_Elements.ChangeElementDrawState( HUI_GLYPH_PAGINATION_MINUS, true );
	m_Elements.ChangeElementDrawState( HUI_GLYPH_PAGINATION_PLUS, true );

	m_Elements.ChangeElementDrawState(HUI_BTN_PAGINATION_MINUS, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_PAGINATION_PLUS, true);

	if( m_GameMode == GameSystems::GAMEMODE_RINGRUN ||
		m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		m_Elements.ChangeElementDrawState( HUI_LABEL_LAYOUT, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_LAYOUT_MINUS, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_LAYOUT_PLUS, true );

		m_Elements.ChangeElementDrawState(HUI_BTN_LAYOUT_MINUS, true);
		m_Elements.ChangeElementDrawState(HUI_BTN_LAYOUT_PLUS, true);

		m_Elements.ChangeElementDrawState( HUI_TEXT_LAYOUT_COUNTER, true );
		m_Elements.ChangeElementText( HUI_TEXT_LAYOUT_COUNTER, "%d/%d", (m_LayoutIndex+1), m_GameData.TOTAL_LAYOUTS );
		m_Elements.ChangeElementDrawState( HUI_LABEL_LAYOUT, true );
	}
	else
	{

		m_Elements.ChangeElementDrawState( HUI_LABEL_LAYOUT, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_LAYOUT_MINUS, true );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_LAYOUT_PLUS, true );

		//m_Elements.ChangeElementDrawState(HUI_BTN_LAYOUT_MINUS, true);
		//m_Elements.ChangeElementDrawState(HUI_BTN_LAYOUT_PLUS, true);

		m_Elements.ChangeElementDrawState( HUI_TEXT_LAYOUT_COUNTER, true );
		m_Elements.ChangeElementText( HUI_TEXT_LAYOUT_COUNTER, "--/--" );
		m_Elements.ChangeElementDrawState( HUI_LABEL_LAYOUT, true );

		math::Vec4Lite col(255,255,255,32);
		m_Elements.ChangeElementTextColour(HUI_TEXT_LAYOUT_COUNTER,col);
		m_Elements.ChangeElementTextColour(HUI_LABEL_LAYOUT,col);

		m_Elements.ChangeElementTextColour(HUI_GLYPH_LAYOUT_MINUS,col);
		m_Elements.ChangeElementTextColour(HUI_GLYPH_LAYOUT_PLUS,col);
	}

	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_1_LEVELSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_2_LEVELSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_3_LEVELSELECT, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_NAME_4_LEVELSELECT, true );

	m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_1_LEVELSELECT, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_2_LEVELSELECT, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_3_LEVELSELECT, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_SLOT_4_LEVELSELECT, true);

	m_Elements.ChangeElementDrawState( HUI_LABEL_PAGE, true );
	m_Elements.ChangeElementDrawState( HUI_TEXT_PAGINATION_COUNTER, true );
	m_Elements.ChangeElementText( HUI_TEXT_PAGINATION_COUNTER, "%d/%d", (m_LevelPackIndex+1), m_GameData.TOTAL_LEVEL_PACKS );

	m_Elements.RefreshSelected();
}

/////////////////////////////////////////////////////
/// Method: SetupLevelSelect
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendLevelSelectUI::UpdateLevelSelect( float deltaTime )
{
	int i=0;

	// check level select buttons
	for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
	{
		if( i < m_LevelPackList[m_LevelPackIndex]->packLevelCount )
		{
			if( !m_ChangedLevel && 
				(i < m_LevelPackList[m_LevelPackIndex]->packLevelCount) )
			{
				if( m_Elements.CheckElementForTouch(HUI_BTN_SLOT_1_LEVELSELECT+i)  )
				{
					m_ChangedLevel = true;
					m_LevelSelection = i;
					m_SelectedLevel = i;
				}
			}
		}
	}

	if( m_Elements.CheckElementForTouch(HUI_BTN_PAGINATION_MINUS) )
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
	if( m_Elements.CheckElementForTouch(HUI_BTN_PAGINATION_PLUS) )
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
	
	if( m_GameMode == GameSystems::GAMEMODE_RINGRUN ||
		m_GameMode == GameSystems::GAMEMODE_TARGET )
	{
		if( m_Elements.CheckElementForTouch(HUI_BTN_LAYOUT_MINUS) )
		{
			if( (m_LayoutIndex) > 0 )
			{
				AudioSystem::GetInstance()->PlayUIAudio();

				m_LayoutIndex--;

				m_Elements.ChangeElementText( HUI_TEXT_LAYOUT_COUNTER, "%d/%d", (m_LayoutIndex+1), m_GameData.TOTAL_LAYOUTS );

				m_ChangedLayout = true;
			}
		}
		else
		if( m_Elements.CheckElementForTouch(HUI_BTN_LAYOUT_PLUS) )
		{
			if( (m_LayoutIndex+1) < m_GameData.TOTAL_LAYOUTS )
			{
				AudioSystem::GetInstance()->PlayUIAudio();

				m_LayoutIndex++;

				m_Elements.ChangeElementText( HUI_TEXT_LAYOUT_COUNTER, "%d/%d", (m_LayoutIndex+1), m_GameData.TOTAL_LAYOUTS );

				m_ChangedLayout = true;
			}
		}
	}
	
	if( m_Elements.CheckElementForTouch(HUI_BTN_NEXT) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		GameSystems::GetInstance()->SetGameModeIndex( m_LayoutIndex );

		ProfileManager::GetInstance()->SetModeLevel( m_SelectedLevelPack, m_SelectedLevel );

		script::LuaCallFunction( m_LevelPackList[m_SelectedLevelPack]->pPackLevelInfo[m_SelectedLevel].levelLoadFuncName, 0, 0 );
	
		ChangeState( new LevelStartUI(*m_pStateManager, m_InputSystem) );
		return 1 ;
	}
	else
	if( m_Elements.CheckElementForTouch(HUI_BTN_BACK) ||
		m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendAircraftSelectUI(*m_pStateManager, m_InputSystem) );
		return 1;
	}

	if( m_ChangedPack || m_ChangedLayout )
	{
		m_Elements.ChangeElementText( HUI_TEXT_PAGINATION_COUNTER, "%d/%d", (m_LevelPackIndex+1), m_GameData.TOTAL_LEVEL_PACKS );

		for( i=0; i < m_GameData.MAX_ITEMS_PER_PACK; ++i )
		{
			if( i < m_LevelPackList[m_LevelPackIndex]->packLevelCount )
			{
				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_LEVELSELECT+i, "%s",  m_LevelPackList[m_LevelPackIndex]->pPackLevelInfo[i].levelName );

				ProfileManager::LevelBest bestScore;

				// update best level
				if( m_GameMode == GameSystems::GAMEMODE_RINGRUN ||
					m_GameMode == GameSystems::GAMEMODE_TARGET )
				{
					if( ProfileManager::GetInstance()->GetScore( m_LevelPackIndex, i, m_GameMode, m_LayoutIndex, &bestScore ) )
					{
						m_Elements.ChangeElementDrawState( HUI_TEXT_BEST_1_LEVELSELECT+i, true );

						if( m_GameMode == GameSystems::GAMEMODE_RINGRUN )
						{
							char text[UI_MAXSTATICTEXTBUFFER_SIZE];

							int minAsInt = static_cast<int>(bestScore.bestTime / 60.0f);
							int secsAsInt = static_cast<int>(bestScore.bestTime) % 60;
							int milliSecsAsInt = static_cast<int>( (bestScore.bestTime-(static_cast<float>(minAsInt)*60.0f)-static_cast<float>(secsAsInt)) * 100.0f );

							if( minAsInt > 99 )
								minAsInt = 99;

							if( secsAsInt < 10 )
							{
								if( milliSecsAsInt < 10 )
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
								else
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
							}
							else
							{
								if( milliSecsAsInt < 10 )
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
								else
									snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
							}

							m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s %s" , res::GetScriptString(1102), text );
						}
						else if( m_GameMode == GameSystems::GAMEMODE_TARGET )
						{
							if( bestScore.targetsDestroyed == bestScore.targetsTotal )
							{
								char text[UI_MAXSTATICTEXTBUFFER_SIZE];

								int minAsInt = static_cast<int>(bestScore.bestTime / 60.0f);
								int secsAsInt = static_cast<int>(bestScore.bestTime) % 60;
								int milliSecsAsInt = static_cast<int>( (bestScore.bestTime-(static_cast<float>(minAsInt)*60.0f)-static_cast<float>(secsAsInt)) * 100.0f );

								if( minAsInt > 99 )
									minAsInt = 99;

								if( secsAsInt < 10 )
								{
									if( milliSecsAsInt < 10 )
										snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
									else
										snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
								}
								else
								{
									if( milliSecsAsInt < 10 )
										snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.0%d", minAsInt, secsAsInt, milliSecsAsInt );
									else
										snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d.%d", minAsInt, secsAsInt, milliSecsAsInt );
								}

								m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s %d / %d %s %s", res::GetScriptString(1102), bestScore.targetsDestroyed, bestScore.targetsTotal, res::GetScriptString(1103), text );
							}
							else
								m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s %d / %d", res::GetScriptString(1102), bestScore.targetsDestroyed, bestScore.targetsTotal );
						}
					}
					else
					{
						m_Elements.ChangeElementDrawState( HUI_TEXT_BEST_1_LEVELSELECT+i, true );

						if( m_GameMode == GameSystems::GAMEMODE_RINGRUN )
							m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s --:--", res::GetScriptString(1102) );
						else if( m_GameMode == GameSystems::GAMEMODE_TARGET )
							m_Elements.ChangeElementText( HUI_TEXT_BEST_1_LEVELSELECT+i, "%s --/--", res::GetScriptString(1102) );
					}
				}
				else
				{
					m_Elements.ChangeElementDrawState( HUI_TEXT_BEST_1_LEVELSELECT+i, false );
				}
			}
			else
			{
				// empty slot
				m_Elements.ChangeElementText( HUI_TEXT_NAME_1_LEVELSELECT+i, "" );
			}
		}

		if( m_ChangedPack )
		{
			m_ChangedPack = false;

			// make sure first level gets selected
			m_LevelSelection = 0;
			m_ChangedLevel = true;
		}

		m_ChangedLayout = false;
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
