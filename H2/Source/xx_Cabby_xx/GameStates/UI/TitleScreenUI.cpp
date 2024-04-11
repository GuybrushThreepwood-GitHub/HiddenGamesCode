
/*===================================================================
	File: TitleScreenUI.cpp
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
#include "GameStates/UI/TitleScreenUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/FrontendUI.h"

#include "GameStates/UI/GeneratedFiles/titlescreen.hgm.h"
#include "GameStates/UI/GeneratedFiles/titlescreen.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
TitleScreenUI::TitleScreenUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_TITLESCREEN )
{

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
TitleScreenUI::~TitleScreenUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenUI::Enter()
{
	int i=0;
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(1000);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/titlescreen.hui", srcDims, m_UIMesh );

#ifdef CABBY_LITE
	m_UIMesh->SetMeshDrawState( GFX_LITE_TABLETSD, true );
#else
	m_UIMesh->SetMeshDrawState( GFX_LITE_TABLETSD, false );
#endif // CABBY_LITE

	m_Background = renderer::INVALID_OBJECT;
	if (core::app::IstvOS())
	{
		// hi-res title screen
		const res::TextureResourceStore* rs = 0;
		rs = res::GetTextureResource(3000);
		DBG_ASSERT(rs != 0);

		res::LoadTexture(3000);

		m_Background = rs->texId;

		m_UIMesh->SetMeshDrawState(BG_YELLOW, false);
		//const int GFX_C = 1;
		//const int GFX_BORDER_TOP = 2;
		//const int GFX_BORDER_BOTTOM = 3;
		//const int GFX_TOPFADE = 4;
		m_UIMesh->SetMeshDrawState(GFX_CABBY_LOGO, false);
		m_UIMesh->SetMeshDrawState(GFX_TAXI_SWOOSH, false);

		m_Elements.ChangeElementDrawState(HUI_LABEL_TAPTOSTART, false);
	}

	ScriptDataHolder* pScriptData = GetScriptDataHolder();
	DBG_ASSERT( pScriptData != 0 );

	for( i=0; i < TOTAL_TITLE_ICONS; ++i )
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
	}
		
	ProfileManager::Profile* pProfile = ProfileManager::GetInstance()->GetProfile();

	bool *pActiveUnlock = &pProfile->yellowCabUnlocked;
	for( i=0; i < TOTAL_TITLE_ICONS; ++i )
	{
		if( *pActiveUnlock )	
			m_VehicleIcon[i].unlocked = true;

		m_UIMesh->SetMeshDrawState( ICON_YELLOWCAB+i, *pActiveUnlock );
		pActiveUnlock++;
	}

	m_Elements.RefreshSelected();
    
    core::app::SetHandleMenuButton(false);
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenUI::Exit()
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

	if (m_Background != renderer::INVALID_OBJECT)
	{
		glDeleteTextures(1, &m_Background );
		m_Background = renderer::INVALID_OBJECT;
	}

	if( m_UIMesh != 0 )
		res::RemoveModel( m_UIMesh );
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int TitleScreenUI::TransitionIn()
{
	m_FadeTransition = 0;
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int TitleScreenUI::TransitionOut()
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
void TitleScreenUI::Update( float deltaTime )
{
	int i=0;
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

	for( i=0; i < TOTAL_TITLE_ICONS; ++i )
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

		if( m_VehicleIcon[i].unlocked )
		{
			/*if( m_VehicleIcon[i].screenPos.X < -790.0f &&
				m_VehicleIcon[i].soundSourceId != snd::INVALID_SOUNDSOURCE )
			{
				AudioSystem::GetInstance()->StopAudio( m_VehicleIcon[i].soundSourceId );	
				m_VehicleIcon[i].soundSourceId = snd::INVALID_SOUNDSOURCE;
			}
			else
			// try a sound source
			if( m_VehicleIcon[i].soundSourceId == snd::INVALID_SOUNDSOURCE &&
				m_VehicleIcon[i].allowAudio )
			{
				m_VehicleIcon[i].soundSourceId = snd::SoundManager::GetInstance()->GetFreeSoundSource();

				if( m_VehicleIcon[i].soundSourceId != snd::INVALID_SOUNDSOURCE )
				{
					math::Vec3 pos = math::Vec3( m_VehicleIcon[i].screenPos );
					AudioSystem::GetInstance()->PlayAudio( m_VehicleIcon[i].soundSourceId, m_FlyByAudioId, pos, AL_TRUE, AL_TRUE );	
				}
			}

			if( m_VehicleIcon[i].soundSourceId != snd::INVALID_SOUNDSOURCE  )
			{
				if( m_VehicleIcon[i].screenPos.X > 160.0f )
				{
					m_VehicleIcon[i].gain += 0.1f*deltaTime;

					if( m_VehicleIcon[i].gain > 1.0f )
						m_VehicleIcon[i].gain = 1.0f;
				}
				else
				{
					m_VehicleIcon[i].gain -= 0.1f*deltaTime;

					if( m_VehicleIcon[i].gain < 0.0f )
						m_VehicleIcon[i].gain = 0.01f;
				}

				CHECK_OPENAL_ERROR( alSourcef( m_VehicleIcon[i].soundSourceId, AL_GAIN, m_VehicleIcon[i].gain ) )

				// update position
				CHECK_OPENAL_ERROR( alSource3f( m_VehicleIcon[i].soundSourceId, AL_POSITION, m_VehicleIcon[i].screenPos.X, m_VehicleIcon[i].screenPos.Y, 0.0f ) )
			}*/
		}

		m_UIMesh->SetMeshTranslation( ICON_YELLOWCAB+i, m_VehicleIcon[i].screenPos );
	}

	if( m_Elements.CheckElementForSingleTouch(HUI_BTN_BG_YELLOW) )
	{
		AudioSystem::GetInstance()->PlayUIAudio();

		ChangeState( new FrontendUI(*m_pStateManager, m_InputSystem, FrontendUI::FrontendStates_MainMenu) );
				
		return;	
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void TitleScreenUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	DrawFullscreenQuad(-10.0f, m_Background);

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
