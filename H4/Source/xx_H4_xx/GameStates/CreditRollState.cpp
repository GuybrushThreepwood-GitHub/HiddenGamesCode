
/*===================================================================
	File: CreditRollState.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#ifdef USE_CREDITROLLSTATE

#include "CoreBase.h"
#include <cmath>
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "ModelBase.h"
#include "InputBase.h"
#include "ScriptBase.h"
#include "PhysicsBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"
#include "Camera/GameCamera.h"

#include "Resources/FontResources.h"
#include "Resources/ModelResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/IBaseGameState.h"

#include "GameStates/MainState.h"

#include "GameStates/FrontendState.h"
#include "GameStates/CreditRollState.h"
#include "GameStates/LoadingState.h"

// generated files
#include "GameStates/UI/GeneratedFiles/frontend.hgm.h"

namespace
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
	const int LOAD_FADE = 15;
	const int MAX_TOUCH_TESTS = 1;

	const float ROLL_SPEED = 15.0f;
	const float STRING_OFFSET = 20.0f;
	const float LOGO_START_POS = -1290.0f;
	const float LOGO_END_POS = -330.0f;

	struct CreditData
	{
		int fontId;
		char* textString;
	};

	CreditData creditsRoll[] = 
	{
		{ 0, "" },
		{ 1, "Design" },
		{ 0, "Gareth West" },
		{ 0, "John Murray" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 1, "Code" },
		{ 0, "John Murray" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 1, "2D/3D Art, Levels & Animation" },
		{ 0, "Gareth West" },
		{ 1, "" },
		{ 0, "" },
		{ 0, "" },
		{ 1, "Music" },
		{ 0, "Ian Tomlinson" },
		{ 0, "Michael Lehane" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "" },
		{ 0, "Copyright 2010 Hidden Games." },
		{ 0, "All rights reserved." },
		{ 0, "www.hiddengames.co.uk" },
		{ 0, "" },
	};
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
CreditRollState::CreditRollState( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
{
	m_LastDelta = 0.0f;

	m_LoadTransitionState = LoadState_TransitionIn;
	m_TransitionAlpha = 255;

	m_pFont12 = 0;
	m_pFont16 = 0;

	m_TotalStrings = (sizeof(creditsRoll)-1)/sizeof(CreditData);
	m_TitleFadeAlpha = 0;
	m_ShakeText = false;

	m_MovementTimer = 0.0f;
	m_CurrentMovement = math::Vec3( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ), 0.0f );
	m_Alpha = math::RandomNumber( 64, 64 );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
CreditRollState::~CreditRollState()
{
	if( m_pCredits != 0 )
	{
		delete[] m_pCredits;
		m_pCredits = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void CreditRollState::Enter()
{
	int i=0;
	const res::FontResourceStore *fr = 0;

	math::Vec4Lite defaultColour( 255,255,255,0 );

	m_FrontendMesh = res::LoadModel( 1002 );
	DBG_ASSERT( m_FrontendMesh != 0 );
	m_FrontendMesh->SetDefaultMeshColour(defaultColour);

	for( i=FRONTEND_BGMESH; i<=FRONTEND_YESMESH; ++i )
		m_FrontendMesh->SetMeshDrawState(i, false);

	m_FrontendMesh->SetMeshDrawState( FRONTEND_TITLEMESH, false );

	fr = res::GetFontResource( 1 );
	m_pFont12				= fr->fontRender;
	m_pFont16				= fr->fontRender;

	m_pCredits = new CreditLine[m_TotalStrings];

	float startPosY = 0.0f;
	for( i=0; i<m_TotalStrings; ++i )
	{
		m_pCredits[i].creditText.SetAlignment( UIFileLoader::TextAlignment_Center );
		m_pCredits[i].creditText.SetDimensions( math::Vec2( 480.0f, 16.0f ) );
		m_pCredits[i].pos = math::Vec2( 240.0f, startPosY-=STRING_OFFSET );
		m_pCredits[i].creditText.SetPosition( m_pCredits[i].pos );
		if( creditsRoll[i].fontId == 0 )
			m_pCredits[i].creditText.SetFont( *m_pFont16 );
		else 
		{
			m_pCredits[i].creditText.SetFontBlockColourState(true);
			m_pCredits[i].creditText.SetActiveColour( math::Vec4Lite( 128,128,128,255 ) );
			m_pCredits[i].creditText.SetFont( *m_pFont12 );
		}
		m_pCredits[i].creditText.SetText( creditsRoll[i].textString );
	}

	m_RollY = LOGO_START_POS;

	//AudioSystem::GetInstance()->StopBGMTrack();
	//AudioSystem::GetInstance()->ClearBGMTrack();
	//AudioSystem::GetInstance()->PlayMusicTrack( "mp3/Prisoner84 - Credits.mp3", false );
	//AudioSystem::GetInstance()->SetBGMTrackVolume( 1.0f );
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void CreditRollState::Exit()
{
	res::RemoveModel( m_FrontendMesh );
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int CreditRollState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int CreditRollState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void CreditRollState::Update( float deltaTime )
{
	int i=0;
	m_LastDelta = deltaTime;

	// do any transitions
	if( UpdateLoadState() )
	{
		// returning true means the state has changed, this is not a valid class anymore
		return;
	}

	if( m_RollY <= LOGO_END_POS )
	{
		for( i=0; i<m_TotalStrings; ++i )
		{
			math::Vec2 currentPos = m_pCredits[i].pos;
			currentPos.Y += ROLL_SPEED*deltaTime;

			m_pCredits[i].pos = currentPos;
			m_pCredits[i].creditText.SetPosition( m_pCredits[i].pos );
			m_pCredits[i].creditText.SetText( creditsRoll[i].textString );
		}

		m_RollY += (ROLL_SPEED*deltaTime);
		//m_LogoImage.SetPosition( math::CVector2D( 240.0f, m_LogoStartY ) );
	}
	else
	{
		m_FrontendMesh->EnableDefaultMeshColour();

		m_TitleFadeAlpha += 1;
		if( m_TitleFadeAlpha > 255 )
		{
			m_TitleFadeAlpha = 255;
			m_ShakeText = true;
		}

		math::Vec4Lite defaultColour( 255,255,255,m_TitleFadeAlpha );
		m_FrontendMesh->SetMeshDrawState( FRONTEND_TITLEMESH, true );
		m_FrontendMesh->SetDefaultMeshColour(defaultColour);

		m_MovementTimer += deltaTime;

		if( m_MovementTimer >= 0.1f )
		{
			m_MovementTimer = 0.0f;
			m_CurrentMovement = math::Vec3( math::RandomNumber( -10.0f, 10.0f ), math::RandomNumber( -10.0f, 10.0f ), 0.0f );
			m_Alpha = math::RandomNumber( 64, 64 );
		}
	}

	if( !AudioSystem::GetInstance()->IsMusicTrackPlaying() )
	{
		ChangeState( new FrontendState(*m_pStateManager, m_InputSystem) );
		return;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void CreditRollState::Draw()
{
	int i=0;
	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	//m_DocsModels->Draw();

	for( i=0; i<m_TotalStrings; ++i )
	{
		m_pCredits[i].creditText.Draw();
	}

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_FrontendMesh->Draw();
	if( m_ShakeText )
	{
		// draw shifted ghost text
		m_FrontendMesh->EnableDefaultMeshColour();
		renderer::OpenGL::GetInstance()->DepthMode( true, GL_LEQUAL );
		glPushMatrix();
			math::Vec4Lite defaultColour( 255,255,255, m_Alpha );
			m_FrontendMesh->SetDefaultMeshColour( defaultColour );
			
			glTranslatef( m_CurrentMovement.X, m_CurrentMovement.Y, m_CurrentMovement.Z );
			m_FrontendMesh->Draw();
		glPopMatrix();
		m_FrontendMesh->DisableDefaultMeshColour();
	}

	// draw any load state
	switch( m_LoadTransitionState )
	{
		case LoadState_None:
		break;
		case LoadState_TransitionIn:
		case LoadState_TransitionOut:
		{
			// fade out then load
			DrawFullscreenQuad( 0.0f, m_TransitionAlpha );
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}
}

/////////////////////////////////////////////////////
/// Method: UpdateLoadState
/// Params: None
///
/////////////////////////////////////////////////////
bool CreditRollState::UpdateLoadState()
{
	switch( m_LoadTransitionState )
	{
		case LoadState_None:

		break;
		case LoadState_TransitionIn:
		{
			m_TransitionAlpha -= LOAD_FADE;
			
			core::app::SetSmoothUpdate(false);
			
			if( m_TransitionAlpha < 0 )
			{
				m_TransitionAlpha = 0;
				core::app::SetSmoothUpdate(true);

				m_LoadTransitionState = LoadState_None;
			}
			return(false);
		}break;
		case LoadState_TransitionOut:
		{
			m_TransitionAlpha += LOAD_FADE;
			
			core::app::SetSmoothUpdate(false);
			
			if( m_TransitionAlpha > 255 )
			{
				m_TransitionAlpha = 255;
				core::app::SetSmoothUpdate(true);

				m_LoadTransitionState = LoadState_None;

				/*if( m_GoToGameStart )
				{
					ChangeState( new IntroOutroState( IntroOutroState::IntroOutroState_Intro, *m_pStateManager, m_InputSystem) );
					return(true);
				}
				else if( m_GoToGameEnd )
				{
					ChangeState( new IntroOutroState( IntroOutroState::IntroOutroState_Outro, *m_pStateManager, m_InputSystem) );
					return(true);
				}*/
			}
			return(false);
		}break;

		default:
			DBG_ASSERT(0);
		break;
	}

	return(false);
}

/////////////////////////////////////////////////////
/// Method: ScreenTap
/// Params: None
///
/////////////////////////////////////////////////////
bool CreditRollState::ScreenTap()
{
#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )
	const input::TInputState::TouchData* pData = 0;
	pData = input::GetTouch(0);
		
	if( pData->bActive == true )
	{
		if( pData->nTicks <= 1 )
		{
			return(true);
		}
	}
#else
	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
	{
		return(true);
	}
#endif 

	return(false);
}

#endif // USE_CREDITROLLSTATE
