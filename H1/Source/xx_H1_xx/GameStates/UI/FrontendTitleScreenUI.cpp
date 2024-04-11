
/*===================================================================
	File: FrontendTitleScreenUI.cpp
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

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/FrontendTitleScreenUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/FrontendMainMenuUI.h"

#include "GameStates/UI/GeneratedFiles/frontend_titlescreen.hgm.h"
#include "GameStates/UI/GeneratedFiles/frontend_titlescreen.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
FrontendTitleScreenUI::FrontendTitleScreenUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_TITLESCREEN )
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
FrontendTitleScreenUI::~FrontendTitleScreenUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendTitleScreenUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1003);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/frontend_titlescreen.hui", srcDims, m_UIMesh );

	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	DBG_ASSERT( pScriptData != 0 );

	/*for( i=0; i < TOTAL_TITLE_ICONS; ++i )
	{
		m_VehicleIcon[i].soundSourceId = snd::INVALID_SOUNDSOURCE;

		m_VehicleIcon[i].unlocked = false;

		if( renderer::OpenGL::GetInstance()->GetIsRotated() )
		{
			m_VehicleIcon[i].screenPos.X = math::RandomNumber( static_cast<float>( core::app::GetOrientationHeight() ) , static_cast<float>( core::app::GetOrientationHeight() )+500.0f );
			m_VehicleIcon[i].screenPos.Y = math::RandomNumber(0.0f, static_cast<float>( core::app::GetOrientationWidth() ) );
		}
		else
		{
			m_VehicleIcon[i].screenPos.X = math::RandomNumber( static_cast<float>( core::app::GetOrientationWidth() ), static_cast<float>( core::app::GetOrientationWidth() )+500.0f );
			m_VehicleIcon[i].screenPos.Y = math::RandomNumber(0.0f, static_cast<float>( core::app::GetOrientationHeight() ) );
		}

		m_UIMesh->SetMeshTranslation( ICON_YELLOWCAB+i, m_VehicleIcon[i].screenPos );

		if( m_VehicleIcon[i].speed > 450.0f )
			m_VehicleIcon[i].allowAudio = true;
		else
			m_VehicleIcon[i].allowAudio = false;

		m_VehicleIcon[i].speed = math::RandomNumber(200.0f,500.0f);
		m_VehicleIcon[i].sinVal = 0.0f;
		m_VehicleIcon[i].sinVal2 = math::RandomNumber( 1.0f, 2.0f );
	}*/
		
	/*bool *pActiveUnlock = &pProfile->planeUnlocked[0];
	for( i=0; i < TOTAL_TITLE_ICONS; ++i )
	{
		if( *pActiveUnlock )	
			m_VehicleIcon[i].unlocked = true;

		m_UIMesh->SetMeshDrawState( ICON_YELLOWCAB+i, *pActiveUnlock );
		pActiveUnlock++;
	}*/

	if (core::app::IstvOS())
	{
		m_Elements.ChangeElementDrawState(HUI_LABEL_TAPTOSTART, false);
	}

	m_Elements.RefreshSelected();

	core::app::SetHandleMenuButton(false);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendTitleScreenUI::Exit()
{
/*	int i=0;

	for( i=0; i < TOTAL_TITLE_ICONS; ++i )
	{
		if( m_VehicleIcon[i].soundSourceId != snd::INVALID_SOUNDSOURCE )
		{
			AudioSystem::GetInstance()->StopAudio( m_VehicleIcon[i].soundSourceId );	
			m_VehicleIcon[i].soundSourceId = snd::INVALID_SOUNDSOURCE;
		}
	}*/

	if( m_UIMesh != 0 )
		res::RemoveModel( m_UIMesh );
	
	m_Elements.Release();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendTitleScreenUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendTitleScreenUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendTitleScreenUI::TransitionIn()
{
	m_FadeTransition = 0;
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int FrontendTitleScreenUI::TransitionOut()
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
void FrontendTitleScreenUI::Update( float deltaTime )
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

	/*for( i=0; i < TOTAL_TITLE_ICONS; ++i )
	{
		m_VehicleIcon[i].sinVal += m_VehicleIcon[i].sinVal2*deltaTime;
		m_VehicleIcon[i].screenPos.X -= m_VehicleIcon[i].speed*deltaTime;
		m_VehicleIcon[i].screenPos.Y += std::sin(m_VehicleIcon[i].sinVal);

		if( m_VehicleIcon[i].screenPos.X < -800.0f )
		{
			m_VehicleIcon[i].gain = 0.0f;

			m_VehicleIcon[i].speed = math::RandomNumber(200.0f,500.0f);

			if( m_VehicleIcon[i].speed > 450.0f )
				m_VehicleIcon[i].allowAudio = true;
			else
				m_VehicleIcon[i].allowAudio = false;

			if( renderer::OpenGL::GetInstance()->GetIsRotated() )
			{
				m_VehicleIcon[i].screenPos.X = math::RandomNumber( static_cast<float>( core::app::GetOrientationHeight() ) , static_cast<float>( core::app::GetOrientationHeight() )+500.0f );
				m_VehicleIcon[i].screenPos.Y = math::RandomNumber(0.0f, static_cast<float>( core::app::GetOrientationWidth() ) );
			}
			else
			{
				m_VehicleIcon[i].screenPos.X = math::RandomNumber( static_cast<float>( core::app::GetOrientationWidth() ), static_cast<float>( core::app::GetOrientationWidth() )+500.0f );
				m_VehicleIcon[i].screenPos.Y = math::RandomNumber(0.0f, static_cast<float>( core::app::GetOrientationHeight() ) );
			}

			m_VehicleIcon[i].sinVal = 0.0f;
			m_VehicleIcon[i].sinVal2 = math::RandomNumber( 1.0f, 2.0f );
		}

		m_UIMesh->SetMeshTranslation( ICON_YELLOWCAB+i, m_VehicleIcon[i].screenPos );
	}*/

	if (m_Elements.CheckElementForSingleTouch(HUI_BTN_BG))
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendMainMenuUI(*m_pStateManager, m_InputSystem, FrontendMainMenuUI::FrontendStates_MainMenu) );
		
		/*if( !ProfileManager::GetInstance()->GetProfile()->adsRemoved )
		{
			// show fullscreen ad
			core::app::SetFullscreenAdState( true );
		}*/

		return;	
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void FrontendTitleScreenUI::Draw()
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

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
