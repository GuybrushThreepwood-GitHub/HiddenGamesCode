
/*===================================================================
	File: GameOverUI.cpp
	Game: H8

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

#include "H8Consts.h"
#include "H8.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/GameOverUI.h"
#include "GameStates/UI/UIFileLoader.h"
#include "GameStates/UI/TextFormattingFuncs.h"

#include "GameStates/MainGameState.h"

#include "GameStates/UI/GeneratedFiles/gameover.hgm.h"
#include "GameStates/UI/GeneratedFiles/gameover.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
GameOverUI::GameOverUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_GAMEOVER )
{
	m_UIMesh = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
GameOverUI::~GameOverUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::Enter()
{
	m_UIMesh = res::LoadModel( 10003 );
	DBG_ASSERT( m_UIMesh != 0 );

	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_Elements.Load( "assets/ui/gameover.hui", srcDims, m_UIMesh );

	m_Elements.ChangeElementDrawState( TS_SUBMITSCORE, false );

#ifndef BASE_PLATFORM_iOS
	m_Elements.ChangeElementDrawState( TS_LEADERBOARD, false );
#endif // BASE_PLATFORM_iOS

	if (core::app::IstvOS())
	{
		m_Elements.ChangeElementDrawState(TS_LEADERBOARD, true);
		m_Elements.ChangeElementDrawState(TS_TWITTER, false);
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::Exit()
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
void GameOverUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int GameOverUI::TransitionIn()
{
	m_FadeTransition = 0;
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int GameOverUI::TransitionOut()
{
	if( !m_FinishedTransitionOut )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition += 15;
		if( m_FadeTransition > 255 )
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
void GameOverUI::Update( float deltaTime )
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

	m_Quit = false;
	m_Restart = false;

	if( m_Elements.CheckElementForSingleTouch( TS_QUIT ) )
	{
		m_Quit = true;
	}
	else
	if( m_Elements.CheckElementForSingleTouch( TS_RESTART ) )
	{
		m_Restart = true;
	}
	else
	if( m_Elements.CheckElementForSingleTouch( TS_TWITTER ) )
	{
		//char szTwitterLink[1024];

		//snprintf( szTwitterLink, 1024-1, "http://twitter.com/home?status=I%%20just%%20scored%%20%s%%20points%%20playing%%20%%3Ca%%20href=%%22http://hiddengames.co.uk/firewall%%22%%3EFirewall%%3C/a%%3E%%20by%%20@hiddengames", 
		//	FormatScoreToHTML( GameSystems::GetInstance()->GetCurrentScore()  ) );

		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*szTwitterLink*/ );
	}
	/*else
	if( m_Elements.CheckElementForSingleTouch( TS_SUBMITSCORE ) )
	{

	}*/
	else
	if( m_Elements.CheckElementForSingleTouch( TS_LEADERBOARD ) )
	{
#ifdef BASE_PLATFORM_iOS
		support::Scores::GetInstance()->ShowLeaderboard("uk.co.hiddengames.firewall.hiscores");
#endif // BASE_PLATFORM_iOS
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::Draw()
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

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: SetGameTime
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::SetGameTime( float time )
{
	SetTime( m_Elements, TD_CURRENTTIME, time );
}
		
/////////////////////////////////////////////////////
/// Method: SetGameScore
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::SetGameScore( unsigned long long int score )
{
	SetScore( m_Elements, TD_CURRENTPOINTS, score );
}
	
/////////////////////////////////////////////////////
/// Method: SetGameLevel
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::SetGameLevel( int level )
{
	m_Elements.ChangeElementText( TD_CURRENLEVEL, "%d", level );
}

/////////////////////////////////////////////////////
/// Method: RefreshSelected
/// Params: None
///
/////////////////////////////////////////////////////
void GameOverUI::RefreshSelected()
{
	m_Elements.RefreshSelected(TS_RESTART);
}
