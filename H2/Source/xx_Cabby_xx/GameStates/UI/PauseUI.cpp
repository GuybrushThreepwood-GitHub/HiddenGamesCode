
/*===================================================================
	File: PauseUI.cpp
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

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/PauseUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/pause.hgm.h"
#include "GameStates/UI/GeneratedFiles/pause.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
PauseUI::PauseUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_PAUSE )
{
	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;

	m_QuitPress = false;
	m_ContinuePress = false;
	m_ControlChange = false;

	m_pProfile = ProfileManager::GetInstance()->GetProfile();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
PauseUI::~PauseUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1008);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/pause.hui", srcDims, m_UIMesh );
	
	/*if( !core::app::IsGameCenterAvailable() ||
	   !core::app::IsNetworkAvailable() )
	{
		m_Elements.ChangeElementDrawState( HUI_BUTTON_SHOW_ACHIEVEMENTS, false );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_ACHIEVEMENT, false );
	}*/

    /*
	if( m_pProfile->tiltControl )
		m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1202) );
	else
		m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1201) );
    */
    
    if( m_pProfile->controlType == 0 )
        m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1201) );
    else if( m_pProfile->controlType == 1 )
        m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1202) );
    else if( m_pProfile->controlType == 2 )
        m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1203) );
    
	if( core::app::IsPCOnly() ||
		core::app::IstvOS() )
	{
		m_UIMesh->SetMeshDrawState( BG_OPTIONS, false );
		m_Elements.ChangeElementDrawState( HUI_LABEL_CONTROL, false );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_LEFT, false );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_RIGHT, false );
	}

	m_Elements.RefreshSelected(HUI_BUTTON_RESUME);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::Exit()
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
void PauseUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int PauseUI::TransitionIn()
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
int PauseUI::TransitionOut()
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
void PauseUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	m_QuitPress = false;
	m_ContinuePress = false;
	m_ControlChange = false;

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

	if( m_Elements.CheckElementForSingleTouch(HUI_BUTTON_QUIT) ||
       m_InputSystem.GetMenu() )
	{
		m_QuitPress = true;
	}
	else
	if( m_Elements.CheckElementForSingleTouch(HUI_BUTTON_RESUME) ||
		m_InputSystem.GetPause() )
	{
		m_ContinuePress = true;
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
    if( m_Elements.CheckElementForTouch(HUI_GLYPH_LEFT) )
    {
        m_ControlChange = true;
        
        m_pProfile->controlType--;
        if( m_pProfile->controlType < 0 )
            m_pProfile->controlType = 0;
        
        if( m_pProfile->controlType == 0 )
        {
            m_pProfile->tiltControl = false;
            core::app::SetAccelerometerState(false);
            m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1201) );
        }
        else if( m_pProfile->controlType == 1 )
        {
            m_pProfile->tiltControl = true;
            core::app::SetAccelerometerState(true, ACCELEROMETER_FREQ);
            m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1202) );
        }
        else if( m_pProfile->controlType == 2 )
        {
            m_pProfile->tiltControl = false;
            core::app::SetAccelerometerState(false);
            m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1203) );
        }
        
        ProfileManager::GetInstance()->SaveProfile();
    }
    else
    if( m_Elements.CheckElementForTouch(HUI_GLYPH_RIGHT) )
    {
        m_ControlChange = true;
        
        m_pProfile->controlType++;
        if( m_pProfile->controlType > 2 )
            m_pProfile->controlType = 2;
        
        if( m_pProfile->controlType == 0 )
        {
            m_pProfile->tiltControl = false;
            core::app::SetAccelerometerState(false);
            m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1201) );
        }
        else if( m_pProfile->controlType == 1 )
        {
            m_pProfile->tiltControl = true;
            core::app::SetAccelerometerState(true, ACCELEROMETER_FREQ);
            m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1202) );
        }
        else if( m_pProfile->controlType == 2 )
        {
            m_pProfile->tiltControl = false;
            core::app::SetAccelerometerState(false);
            m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1203) );
        }
        
        ProfileManager::GetInstance()->SaveProfile();
    }
	/*if( m_Elements.CheckElementForTouch(HUI_BUTTON_OPTIONCYCLE_CONTROLS) )
	{
		if( !core::app::IsPCOnly() )
		{
			m_ControlChange = true;
			m_pProfile->tiltControl = !m_pProfile->tiltControl;
	
			AudioSystem::GetInstance()->PlayUIAudio();
		
			if( m_pProfile->tiltControl )
			{
				core::app::SetAccelerometerState(true, ACCELEROMETER_FREQ);
				m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1202) );
			}
			else
			{
				core::app::SetAccelerometerState(false);
				m_Elements.ChangeElementText( HUI_LABEL_CONTROL, "%s", res::GetScriptString(1201) );
			}

			ProfileManager::GetInstance()->SaveProfile();
		}
	}*/
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::Draw()
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
