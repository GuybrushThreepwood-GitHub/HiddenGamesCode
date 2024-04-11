
/*===================================================================
	File: PauseUI.cpp
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
#include "SupportBase.h"

#include "H1Consts.h"
#include "H1.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/PauseUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/ingame_paused.hgm.h"
#include "GameStates/UI/GeneratedFiles/ingame_paused.hui.h"

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
	m_RestartPress = false;
	m_ContinuePress = false;
	m_ControlChange = false;

	m_ShowTokenTime = false;
	m_TokenCountdown = 0.0f;

	m_TakuCount = 0;
	m_MeiCount = 0;
	m_EarlCount = 0;
	m_FioCount = 0;
	m_MitoCount = 0;
	m_UkiCount = 0;

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
	m_UIMesh = res::LoadModel(1007);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/ingame_paused.hui", srcDims, m_UIMesh );
	
	if( !core::app::IsGameCenterAvailable() ||
	   !core::app::IsNetworkAvailable() )
	{
		m_Elements.ChangeElementDrawState( HUI_BUTTON_SHOW_ACHIEVEMENTS, false );
		m_Elements.ChangeElementDrawState( HUI_GLYPH_ACHIEVEMENT, false );
	}

	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_1, "%d" , m_TakuCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_2, "%d" , m_MeiCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_3, "%d" , m_EarlCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_4, "%d" , m_FioCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_5, "%d" , m_MitoCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_6, "%d" , m_UkiCount );

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
	
	m_Elements.Release();	
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
	m_RestartPress = false;
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
	if( m_Elements.CheckElementForSingleTouch(HUI_BUTTON_RESTART) )
	{
		m_RestartPress = true;
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

/////////////////////////////////////////////////////
/// Method: SetTokenCountdown
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::SetTokenCountdown( float time )				
{ 
	m_TokenCountdown = time; 

	if( m_ShowTokenTime )
	{
		char text[UI_MAXSTATICTEXTBUFFER_SIZE];

		int minAsInt = static_cast<int>(time/ 60.0f);
		int secsAsInt = static_cast<int>(time) % 60;

		if( minAsInt > 99 )
			minAsInt = 99;

		if( minAsInt < 10 )
		{
			if( secsAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:0%d", minAsInt, secsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0%d:%d", minAsInt, secsAsInt );
		}
		else
		{
			if( secsAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:0%d", minAsInt, secsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d:%d", minAsInt, secsAsInt );
		}

		m_Elements.ChangeElementText( HUI_LABEL_TITLE, "%s ( %s )" , res::GetScriptString(1201), text );
	}
	else
	{
		m_Elements.ChangeElementText( HUI_LABEL_TITLE, "%s" , res::GetScriptString(1201) );
	}
}

/////////////////////////////////////////////////////
/// Method: SetTokenCounts
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::SetTokenCounts( int taku, int mei, int earl, int fio, int mito, int uki )
{
	m_TakuCount = taku;
	m_MeiCount = mei;
	m_EarlCount = earl;
	m_FioCount = fio;
	m_MitoCount = mito;
	m_UkiCount = uki;

	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_1, "%d" , m_TakuCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_2, "%d" , m_MeiCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_3, "%d" , m_EarlCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_4, "%d" , m_FioCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_5, "%d" , m_MitoCount );
	m_Elements.ChangeElementText( HUI_TEXT_COUNTER_6, "%d" , m_UkiCount );
}

/////////////////////////////////////////////////////
/// Method: SetTotalFlightTime
/// Params: [in]time
///
/////////////////////////////////////////////////////
void PauseUI::SetTotalFlightTime( float time )
{
	char text[UI_MAXSTATICTEXTBUFFER_SIZE];

	int hourAsInt = static_cast<int>((time / 60.0f) / 60.0f);
	int minAsInt = static_cast<int>(time / 60.0f) % 60;
	int secsAsInt = static_cast<int>(time) % 60;

	if( hourAsInt > 99 )
		hourAsInt = 99;

	if( hourAsInt < 10 )
	{
		if( minAsInt < 10 )
		{
			if( secsAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: 0%d:0%d.0%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: 0%d:0%d.%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
		}
		else
		{
			if( secsAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: 0%d:%d.0%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: 0%d:%d.%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
		}
	}
	else
	{
		if( minAsInt < 10 )
		{
			if( secsAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: %d:0%d.0%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: %d:0%d.%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
		}
		else
		{
			if( secsAsInt < 10 )
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: %d:%d.0%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
			else
				snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%s: %d:0%d.%d", res::GetScriptString(1202), hourAsInt, minAsInt, secsAsInt );
		}
	}

	m_Elements.ChangeElementText( HUI_LABEL_PAUSED, text );
}

/////////////////////////////////////////////////////
/// Method: RefreshSelected
/// Params: None
///
/////////////////////////////////////////////////////
void PauseUI::RefreshSelected()
{
    m_Elements.RefreshSelected(HUI_BUTTON_RESUME);
}

