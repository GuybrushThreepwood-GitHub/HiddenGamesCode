
/*===================================================================
	File: GameCompleteUI.cpp
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
#include "GameStates/UI/GameCompleteUI.h"
#include "GameStates/UI/FrontendUI.h"
#include "GameStates/UI/UnlockUI.h"
#include "GameStates/UI/LevelCompleteArcadeUI.h"
#include "GameStates/TitleScreenState.h"
#include "GameStates/UnlockState.h"
#include "GameStates/MainGameState.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/game_complete.hgm.h"
#include "GameStates/UI/GeneratedFiles/game_complete.hui.h"

namespace
{
	const float SPECIAL_ENDING_WAIT = 20.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameCompleteUI::GameCompleteUI( StateManager& stateManager, InputSystem& inputSystem, ProfileManager::eGameMode mode )
: IBaseGameState( stateManager, inputSystem, UI_GAMECOMPLETE )
, m_Mode(mode)
{
	m_UIMesh = 0;
	m_SpecialEnding = false;
	m_EndingAnimComplete = false;
	m_GameData = GetScriptDataHolder()->GetGameData();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameCompleteUI::~GameCompleteUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void GameCompleteUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1013);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/game_complete.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );

	if( ProfileManager::GetInstance()->GetVehicleUnlockState() )
	{
		int vehicleId = ProfileManager::GetInstance()->GetVehicleUnlockId();

		if( vehicleId != -1 )
		{
			m_Elements.ChangeElementText( HUI_TEXT_TITLE_UNLOCKS, "%s", res::GetScriptString(1300));

			// get pack and vehicle offset
			int packId = vehicleId / m_GameData.MAX_ITEMS_PER_PACK;
			int vehicleOffsetId = vehicleId % m_GameData.MAX_ITEMS_PER_PACK;

			m_Elements.ChangeElementText( HUI_TEXT_MESSAGE_UNLOCKS, "%s%s", res::GetScriptString(1301), GetScriptDataHolder()->GetVehiclePackList()[packId]->pPackVehicleInfo[vehicleOffsetId].vehicleName);
		}
		else
		{
			m_Elements.ChangeElementText( HUI_TEXT_TITLE_UNLOCKS, "%s", res::GetScriptString(1227));
		}
	}
	else
	{
		// no vehicle to unlock, special ending
		m_SpecialEnding = true;
		m_SpecialEndingWait = 0.0f;

		// hide until camera has finished
		m_Elements.ChangeElementDrawState( HUI_TEXT_TITLE_UNLOCKS, false );
		m_Elements.ChangeElementText( HUI_TEXT_TITLE_UNLOCKS, "%s", res::GetScriptString(1227));
		m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_UNLOCKS, false );
		m_UIMesh->SetMeshDrawState( BTN_CONTINUE, false );
		m_Elements.ChangeElementDrawState(HUI_BUTTON_CONTINUE, false);
	}

	m_Elements.RefreshSelected();

#ifdef CABBY_FREE
	core::app::SetAdBarState( false );
#endif // CABBY_FREE
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void GameCompleteUI::Exit()
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
void GameCompleteUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FinishedTransitionOut = true;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void GameCompleteUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FinishedTransitionIn = true;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int GameCompleteUI::TransitionIn()
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
int GameCompleteUI::TransitionOut()
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
void GameCompleteUI::Update( float deltaTime )
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

	m_SpecialEndingWait += deltaTime;
	if( m_SpecialEndingWait >= SPECIAL_ENDING_WAIT )
	{
		m_SpecialEndingWait = SPECIAL_ENDING_WAIT;

		m_Elements.ChangeElementDrawState( HUI_TEXT_TITLE_UNLOCKS, true );
		m_Elements.ChangeElementTextShadowState(HUI_TEXT_TITLE_UNLOCKS, true);
		m_Elements.ChangeElementDrawState( HUI_GLYPH_NEXT_UNLOCKS, true );
		m_UIMesh->SetMeshDrawState( BTN_CONTINUE, true );
		m_Elements.ChangeElementDrawState(HUI_BUTTON_CONTINUE, true);

		if (!m_EndingAnimComplete)
		{
			m_Elements.RefreshSelected();
			m_EndingAnimComplete = true;
		}
	}

	if( m_Elements.CheckElementForSingleTouch(HUI_BUTTON_CONTINUE) ||
       m_InputSystem.GetMenu() )
	{
		if( m_SpecialEnding )
		{
			if( m_SpecialEndingWait < SPECIAL_ENDING_WAIT )
			{
				return;
			}
		}

		// clear out the state
		ProfileManager::GetInstance()->RemoveVehicleFromUnlocks();

		// there is an unlock
		if( ProfileManager::GetInstance()->GetVehicleUnlockState() )
		{
			m_pStateManager->ChangeSecondaryState( new UnlockState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );		
			ChangeState( new GameCompleteUI(*m_pStateManager, m_InputSystem, m_Mode ) );
			return;
		}
		else
		{
			if( m_Mode == ProfileManager::CAREER_MODE )
			{
				m_pStateManager->ChangeSecondaryState( new TitleScreenState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );	
				ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
			}
			else
			{
				m_pStateManager->ChangeSecondaryState( new MainGameState(*m_pStateManager->GetSecondaryStateManager(), m_InputSystem) );	
				ChangeState( new LevelCompleteArcadeUI(*m_pStateManager, m_InputSystem) );
			}
			return;	
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void GameCompleteUI::Draw()
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

	// always draw
	GameSystems::GetInstance()->DrawAchievementUI();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}