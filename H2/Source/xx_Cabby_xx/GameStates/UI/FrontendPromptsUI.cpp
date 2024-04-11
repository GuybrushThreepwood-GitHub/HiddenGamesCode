
/*===================================================================
	File: FrontendPromptsUI.cpp
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
#include "GameStates/UI/FrontendPromptsUI.h"
#include "GameStates/UI/FrontendItemSelectUI.h"
#include "GameStates/UI/FrontendUI.h"

#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/frontend_prompts.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_prompts.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendPromptsUI::FrontendPromptsUI( StateManager& stateManager, InputSystem& inputSystem, bool fullClear )
: IBaseGameState( stateManager, inputSystem, UI_FRONTENDPROMPTS )
, m_FullClear(fullClear)
{
	m_UIMesh = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendPromptsUI::~FrontendPromptsUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendPromptsUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1002);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_prompts.hui", srcDims, m_UIMesh );

	m_pProfile = 0;
	m_pProfile = ProfileManager::GetInstance()->GetProfile();
	DBG_ASSERT( m_pProfile != 0 );
    
	if( m_FullClear )
	{
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_WHITE_1, true );
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED, true );

		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_WHITE_2, false );
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_GREEN, false );
	}
	else
	{
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_WHITE_1, false );
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_RED, false );

		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_WHITE_2, true );
		m_Elements.ChangeElementDrawState( HUI_TEXT_MESSAGE_GREEN, true );
	}

	m_Elements.ChangeElementDrawState(HUI_BTN_NO, true);
	m_Elements.ChangeElementDrawState(HUI_BTN_YES, true);

	m_Elements.RefreshSelected();
    
    core::app::SetHandleMenuButton(true);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendPromptsUI::Exit()
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
void FrontendPromptsUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendPromptsUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendPromptsUI::TransitionIn()
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
int FrontendPromptsUI::TransitionOut()
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
void FrontendPromptsUI::Update( float deltaTime )
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

	if( m_Elements.CheckElementForTouch( HUI_BTN_NO ) ||
       m_InputSystem.GetMenu() )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		/*if( m_FullClear )
		{
			ChangeState( new FrontendFileSelectUI(*m_pStateManager, m_InputSystem) );
			return;	
		}
		else*/
		{
			ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
			return;		
		}
	}

	if( m_Elements.CheckElementForTouch( HUI_BTN_YES ) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		/*if( m_FullClear )
		{
			// clear the profile
			ProfileManager::GetInstance()->ClearProfile( m_pProfile->profileSlot, true );

			ChangeState( new FrontendFileSelectUI(*m_pStateManager, m_InputSystem) );
			return;		
		}
		else*/
		{
			// clear the profile
			ProfileManager::GetInstance()->ClearProfile( false );

			ChangeState( new FrontendItemSelectUI(*m_pStateManager, m_InputSystem, FrontendItemSelectUI::FrontendItemSelectStates_TaxiSelect) );
			return;		
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendPromptsUI::Draw()
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

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}