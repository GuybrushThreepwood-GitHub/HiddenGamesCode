
/*===================================================================
	File: MainMenuUI.cpp
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
#include "GameStates/UI/MainMenuUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/MainGameState.h"

#include "GameStates/UI/GeneratedFiles/mainmenu.hgm.h"
#include "GameStates/UI/GeneratedFiles/mainmenu.hui.h"

namespace
{
	const float TAPFLASH_TIME = 1.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MainMenuUI::MainMenuUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_MAINMENU )
{
	m_UIMesh = 0;
	m_StartGame = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MainMenuUI::~MainMenuUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void MainMenuUI::Enter()
{
	m_UIMesh = res::LoadModel( 10001 );
	DBG_ASSERT( m_UIMesh != 0 );

	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_Elements.Load( "assets/ui/mainmenu.hui", srcDims, m_UIMesh );

	m_TapFlashTime = TAPFLASH_TIME;
	m_ShowTapFlash = false;

	m_Elements.ChangeElementDrawState( TS_MODE_1, m_ShowTapFlash );

#ifndef BASE_PLATFORM_iOS
	m_Elements.ChangeElementDrawState( TS_OPTIONS_LEADERBOARD, false );
#endif // BASE_PLATFORM_iOS

	if (core::app::IstvOS())
	{
		m_Elements.ChangeElementDrawState(TS_MODE_1, true);
		m_Elements.ChangeElementText(TS_MODE_1, res::GetScriptString(101));

		m_Elements.ChangeElementDrawState(TS_OPTIONS_LEADERBOARD, true);

		m_Elements.ChangeElementDrawState(TS_OPTIONS_MOREGAMES, false);
		m_Elements.ChangeElementDrawState(TS_OPTIONS_ABOUT, false);
		m_Elements.ChangeElementDrawState(TS_OPTIONS_RATE, false);
        
        // change achievement icon to be centered
        m_Elements.ChangeElementTextAlignment(TS_OPTIONS_LEADERBOARD, UIFileLoader::TextAlignment_Center);
        math::Vec3 posText = m_Elements.GetElementPositionNonAABB(TS_MODE_1);
        math::Vec3 posIcon = m_Elements.GetElementPosition(TS_OPTIONS_LEADERBOARD);
        posIcon.X = posText.X;
        m_Elements.ChangeElementPosition(TS_OPTIONS_LEADERBOARD, posIcon);
	}

	m_Elements.RefreshSelected(TS_MODE_1);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void MainMenuUI::Exit()
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
void MainMenuUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = true;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void MainMenuUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int MainMenuUI::TransitionIn()
{
	m_FadeTransition = 0;
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int MainMenuUI::TransitionOut()
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
void MainMenuUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;
	m_StartGame = false;

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

	if (!core::app::IstvOS())
	{
		m_TapFlashTime -= deltaTime;
		if (m_TapFlashTime <= 0.0f)
		{
			m_ShowTapFlash = !m_ShowTapFlash;
			m_Elements.ChangeElementDrawState(TS_MODE_1, m_ShowTapFlash);

			if (m_ShowTapFlash)
			{
				ALuint bufferId = AudioSystem::GetInstance()->AddAudioFile(101);

				if (bufferId != snd::INVALID_SOUNDBUFFER)
				{
					AudioSystem::GetInstance()->PlayAudio(bufferId, math::Vec3(0.0f, 0.0f, 0.0f), AL_TRUE, AL_FALSE, 1.5f);
				}
			}
			m_TapFlashTime = TAPFLASH_TIME;
		}
	}
	if( m_Elements.CheckElementForSingleTouch( TS_OPTIONS_MOREGAMES ) )
	{
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10000)*/);
	}
	else if( m_Elements.CheckElementForSingleTouch( TS_OPTIONS_ABOUT ) )
	{
		core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*res::GetScriptString(10001)*/);
	}
	else if( m_Elements.CheckElementForSingleTouch( TS_OPTIONS_RATE ) )
	{
		//if( core::app::GetAppRateURL() != 0 )
			core::app::OpenWebLink("https://github.com/GuybrushThreepwood-GitHub/HiddenGamesCode"/*core::app::GetAppRateURL()*/);
	}
	else
	if( m_Elements.CheckElementForSingleTouch( TS_OPTIONS_LEADERBOARD ) )
	{
#ifdef BASE_PLATFORM_iOS
		//if( support::Scores::GetInstance()->IsLoggedIn() )
			support::Scores::GetInstance()->ShowLeaderboard("uk.co.hiddengames.firewall.hiscores");
#endif // BASE_PLATFORM_iOS
	}
	else if (m_Elements.CheckElementForSingleTouch(TS_MODE_1) ||
		input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
	{
		// start game
		m_StartGame = true;
        
        core::app::SetHandleMenuButton(true);
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MainMenuUI::Draw()
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
/// Method: RefreshSelected
/// Params: None
///
/////////////////////////////////////////////////////
void MainMenuUI::RefreshSelected()
{
	m_Elements.RefreshSelected();
}
