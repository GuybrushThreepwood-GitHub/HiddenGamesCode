
/*===================================================================
	File: HudUI.cpp
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

#include "GameEffects/FullscreenEffects.h"

#include "Profiles/ProfileManager.h"

#include "Resources/StringResources.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/HudUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/UI/GeneratedFiles/hud.hgm.h"
#include "GameStates/UI/GeneratedFiles/hud.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
HudUI::HudUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_HUD )
{
	m_Player = 0;

	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;

	m_Pause = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
HudUI::~HudUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	//m_UIMesh = res::LoadModel(1004);
	//DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/hud.hui", srcDims, m_UIMesh );

	m_LevelData = GetScriptDataHolder()->GetLevelData();
	m_GameData = GetScriptDataHolder()->GetGameData();

	if (core::app::IstvOS())
	{
		m_Elements.ChangeElementDrawState(HUI_TEXT_PAUSE, false);
	}

}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::Exit()
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
void HudUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int HudUI::TransitionIn()
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
int HudUI::TransitionOut()
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
void HudUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	m_Pause = false;

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	if( m_Elements.CheckElementForSingleTouch( HUI_BUTTON_PAUSE ) )
	{
		m_Pause = true;
	}

	//m_Player = 0;//PhysicsWorld::GetPlayer();
	//if( m_Player == 0  )
	//	return;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::Draw()
{
	//m_Player = 0;//PhysicsWorld::GetPlayer();
	//if( m_Player == 0 )
	//	return;

	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: SetCountdown
/// Params: [in]number
///
/////////////////////////////////////////////////////
void HudUI::SetCountdown( int number )
{
	if( number == -1 )
		m_Elements.ChangeElementText( HUI_TEXT_COUNTDOWN, "%s", res::GetScriptString(61) );
	else
	{
		m_Elements.ChangeElementText( HUI_TEXT_COUNTDOWN, "%d", number );
	}
}

/////////////////////////////////////////////////////
/// Method: SetGameTime
/// Params: [in]timer
///
/////////////////////////////////////////////////////
void HudUI::SetGameTime( float timer )
{
/*	char text[UI_MAXSTATICTEXTBUFFER_SIZE];

	int minAsInt = static_cast<int>(timer / 60.0f);
	int secsAsInt = static_cast<int>(timer) % 60;

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
	m_Elements.ChangeElementText( HUI_LABEL_TIME, "%s", text );*/

	SetTime( m_Elements, HUI_LABEL_TIME, timer );
	
}

/////////////////////////////////////////////////////
/// Method: SetGamePoints
/// Params: [in]points
///
/////////////////////////////////////////////////////
void HudUI::SetGamePoints( const char* points )
{
	m_Elements.ChangeElementText( HUI_LABEL_SCORE, "%s", points );
}

/////////////////////////////////////////////////////
/// Method: DisableScore
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::DisableScore()
{
	m_Elements.ChangeElementDrawState( HUI_LABEL_SCORE, false );
}

/////////////////////////////////////////////////////
/// Method: DisableTime
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::DisableTime()
{
	m_Elements.ChangeElementDrawState( HUI_LABEL_TIME, false );
}

/////////////////////////////////////////////////////
/// Method: SetCountdownState
/// Params: None
///
/////////////////////////////////////////////////////
void HudUI::SetCountdownState( bool state )
{
	m_Elements.ChangeElementDrawState( HUI_TEXT_COUNTDOWN, state );
}
