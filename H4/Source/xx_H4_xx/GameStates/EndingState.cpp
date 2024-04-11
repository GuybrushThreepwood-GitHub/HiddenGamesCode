
/*===================================================================
	File: EndingState.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

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

#include "Resources/ModelResources.h"
#include "Resources/SoundResources.h"
#include "Resources/SpriteResources.h"
#include "Resources/StringResources.h"
#include "Resources/EmitterResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/IBaseGameState.h"

#include "GameStates/MainState.h"

#include "GameStates/IntroOutroState.h"
#include "GameStates/EndingState.h"
#include "GameStates/LoadingState.h"

// generated files
#include "GameStates/UI/GeneratedFiles/cutscene.hui.h"

namespace
{
	math::Vec4 lightPos( 4.5f, -2.5f, 1.8f, 0.0f );

	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
	const int LOAD_FADE = 15;
	const int MAX_TOUCH_TESTS = 1;

	math::Vec3 fagOffset( -0.022f, 1.645f, 0.136f );
	math::Vec3 cupSteam( -0.296f, 1.31f, 0.970f );


	// intro
	math::Vec3 shoulderIntroPos( -0.625f, 1.77f, -0.74f );
	math::Vec3 shoulderIntroTarget( 42.08f, -13.22f, 89.65f );

	math::Vec3 screenZoomPos( -0.38f, 1.57f, -0.04f );
	math::Vec3 screenZoomTarget( 37.29f, -4.43f, 92.57f );


	// ending
	math::Vec3 screenOutroZoomPos( -0.38f, 1.57f, -0.04f );
	math::Vec3 screenOutroZoomTarget( 37.29f, -4.43f, 92.57f );

	math::Vec3 shoulderOutroPos( -0.625f, 1.77f, -0.74f );
	math::Vec3 shoulderOutroTarget( 42.08f, -13.22f, 89.65f );

	math::Vec3 facePos( -0.05f, 1.67f, 0.21f );
	math::Vec3 faceTarget( 46.30f, 25.67f, -88.39f );

	math::Vec3 faceEndPos( -0.183f, 1.27f, 0.88f );
	math::Vec3 faceEndTarget( 20.85f, 49.27f, -96.87f );

	const float STARTTIME_CAM0 = 4.0f;
	const float STARTTIME_CAM1 = 12.0f;

	const float ENDTIME_CAM0 = 10.0f;
	const float ENDTIME_CAM1 = 15.0f;
	const float ENDTIME_CAM2 = 15.0f;
	const float ENDTIME_CAM3 = 20.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
EndingState::EndingState( int mode, StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem )
{
	m_LastDelta = 0.0f;

	m_LoadTransitionState = LoadState_TransitionIn;
	m_TransitionAlpha = 255;
		
	m_GoToGameStart = false;
	m_GoToGameEnd = false;

	if( mode == 0 )
	{
		m_GoToGameStart = true;

		m_CamPos = shoulderIntroPos;
		m_CamTarget = shoulderIntroTarget;
	}
	else
	{
		m_GoToGameEnd = true;

		m_CamPos = screenOutroZoomPos;
		m_CamTarget = screenOutroZoomTarget;
	}

	m_Colonel = 0;
	m_EndingMesh = 0;

	m_EndingCameraIndex = 0;


	m_pSmokeEmitter = 0;
	m_pBurnEmitter = 0;

	m_pCupEmitter = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
EndingState::~EndingState()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void EndingState::Enter()
{
	math::Vec4Lite defaultColour( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );
	//renderer::OpenGL::GetInstance()->SetPerspective( GAME_FOV, static_cast<float>(core::app::GetOrientationWidth())/static_cast<float>(core::app::GetOrientationHeight()), 0.01f, 20.0f );

	// light0
	renderer::OpenGL::GetInstance()->EnableLight( 0 );
	math::Vec4 m_LightPos = math::Vec4( 4.5f, -2.5f, 1.8f, 0.0f );
	math::Vec4 m_LightDiffuse = math::Vec4( 0.89f, 1.0f, 0.93f, 1.0f );
	math::Vec4 m_LightAmbient = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	math::Vec4 m_LightSpecular = math::Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetLightPosition( 0, m_LightPos );
	renderer::OpenGL::GetInstance()->SetLightDiffuse( 0, m_LightDiffuse );
	renderer::OpenGL::GetInstance()->SetLightAmbient( 0, m_LightAmbient);
	renderer::OpenGL::GetInstance()->SetLightSpecular( 0, m_LightSpecular );	
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_CONSTANT_ATTENUATION, 0.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_LINEAR_ATTENUATION, 0.0f );
	renderer::OpenGL::GetInstance()->SetLightAttenuation( 0, GL_QUADRATIC_ATTENUATION, 0.25f );

	// outro
	m_EndingMesh = new mdl::ModelHGM;
	DBG_ASSERT( m_EndingMesh != 0 );
	m_EndingMesh->Load( "stages/c01/level.hgm", GL_LINEAR, GL_LINEAR, false );
	m_EndingMesh->SetModelGeneralFlags(mdl::MODELFLAG_GENERAL_AUTOCULLING | mdl::MODELFLAG_GENERAL_AABBCULL | mdl::MODELFLAG_GENERAL_SUBMESHCULL);
	m_EndingMesh->SetupAndFree();

	m_Colonel = new mdl::ModelHGA;
	DBG_ASSERT( m_Colonel != 0 );
	m_Colonel->Load( "hga/cutscene_larson_bind.hga", GL_LINEAR, GL_LINEAR );
	m_Colonel->LoadAnimation( "hga/cutscene_larson_mobile.hga", 0, true );
	m_Colonel->SetAnimation( 0 );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_TextElements.Load( "hui/cutscene.hui", srcAssetDims );

	//gDebugCamera.Enable();
	//gDebugCamera.Disable();

	m_EndingMesh->SetMeshDrawState( 3, false );
	m_EndingMesh->SetMeshDrawState( 4, false );
	
	if( m_GoToGameStart )
	{
		m_EndingMesh->SetMeshDrawState( 3, true );
		m_Timer = STARTTIME_CAM0;


		m_TextElements.ChangeElementText( TEXT_LINE1, "" );
		m_TextElements.ChangeElementText( TEXT_LINE2, "" );
	}
	else if( m_GoToGameEnd )
	{
		m_EndingMesh->SetMeshDrawState( 4, true );

		m_Timer = ENDTIME_CAM0;
		const char* sr = res::GetScriptString( 500 );
		m_TextElements.ChangeElementText( TEXT_LINE1, sr );
		sr = res::GetScriptString( 501 );
		m_TextElements.ChangeElementText( TEXT_LINE2, sr );
	}

	m_StartZoomOut = false;

	m_CigaretteOffset = math::Vec3( m_Colonel->GetBoneJointsPtr()[4].final.m41, m_Colonel->GetBoneJointsPtr()[4].final.m42, m_Colonel->GetBoneJointsPtr()[4].final.m43 );
	m_CigaretteOffset -= fagOffset;

	// emitters
	const res::EmitterResourceStore* er = 0;
	renderer::Texture texLoad;

	er = res::GetEmitterResource( 34 );
	DBG_ASSERT( er != 0 );

	m_pSmokeEmitter = new efx::Emitter;
	DBG_ASSERT( (m_pSmokeEmitter != 0) );

	m_pSmokeEmitter->Create( *er->block );
	m_pSmokeEmitter->SetPos( m_CigaretteOffset );
	m_pSmokeEmitter->SetRot( zeroVec );
	m_pSmokeEmitter->Update(0.0f);
	m_pSmokeEmitter->Enable();

	m_pSmokeEmitter->SetTimeOn( -1 );
	m_pSmokeEmitter->SetTimeOff( -1 );

	// grab the sprite resource
	res::SetupTexturesOnEmitter( m_pSmokeEmitter );

	// burn
/*	er = res::GetEmitterResource( 35 );
	DBG_ASSERT( er != 0 );

	m_pBurnEmitter = new efx::Emitter;
	DBG_ASSERT( (m_pBurnEmitter != 0) );

	m_pBurnEmitter->Create( *er->block );
	m_pBurnEmitter->SetPos( m_CigaretteOffset );
	m_pBurnEmitter->SetRot( zeroVec );

	m_pBurnEmitter->Enable();

	m_pBurnEmitter->SetTimeOn( -1.0f );
	m_pBurnEmitter->SetTimeOff( -1.0f );

	// grab the sprite resource
	if( er->block->emitterId != -1 )
	{
		const res::SpriteResourceStore* sr = 0;
		sr = res::GetSpriteResource(er->block->emitterId);

		GLuint texId = renderer::TextureLoad( "textures/fx.png", texLoad, renderer::TEXTURE_PNG, 0, false, sr->magFilter, sr->minFilter );
		m_pBurnEmitter->SetTexture(texId);
		m_pBurnEmitter->SetTextureUVOffset( sr->uOffset, sr->vOffset, sr->w, sr->h );
	}
*/

	//cup
	er = res::GetEmitterResource( 29 );
	DBG_ASSERT( er != 0 );

	m_pCupEmitter = new efx::Emitter;
	DBG_ASSERT( (m_pCupEmitter != 0) );

	m_pCupEmitter->Create( *er->block );
	m_pCupEmitter->SetPos( cupSteam );
	m_pCupEmitter->SetRot( zeroVec );

	m_pCupEmitter->Enable();

	m_pCupEmitter->SetTimeOn( -1 );
	m_pCupEmitter->SetTimeOff( -1 );

	// grab the sprite resource
	res::SetupTexturesOnEmitter( m_pCupEmitter );
	
	if( m_GoToGameStart )
	{
		AudioSystem::GetInstance()->StopBGMTrack();
		AudioSystem::GetInstance()->ClearBGMTrack();
		AudioSystem::GetInstance()->PlayBGMTrack( "mp3/Prisoner84-intro.mp3", false );
		AudioSystem::GetInstance()->SetBGMTrackVolume( 1.0f );	
	}
	else
	{
		AudioSystem::GetInstance()->StopBGMTrack();
		AudioSystem::GetInstance()->ClearBGMTrack();
		AudioSystem::GetInstance()->PlayBGMTrack( "mp3/Prisoner84-the-end.mp3", false );
		AudioSystem::GetInstance()->SetBGMTrackVolume( 1.0f );
	}

	// disable screen dim
#ifdef BASE_PLATFORM_iOS
	[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
#endif // BASE_PLATFORM_iOS
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void EndingState::Exit()
{

	AudioSystem::GetInstance()->StopBGMTrack();
	AudioSystem::GetInstance()->ClearBGMTrack();
	
	if( m_Colonel != 0 )
	{
		delete m_Colonel;
		m_Colonel = 0;
	}

	if( m_EndingMesh != 0 )
	{
		delete m_EndingMesh;
		m_EndingMesh = 0;
	}

	if( m_pSmokeEmitter != 0 )
	{
		delete m_pSmokeEmitter;
		m_pSmokeEmitter = 0;
	}

/*	if( m_pBurnEmitter != 0 )
	{
		delete m_pBurnEmitter;
		m_pBurnEmitter = 0;
	}
*/

	if( m_pCupEmitter != 0 )
	{
		delete m_pCupEmitter;
		m_pCupEmitter = 0;
	}

	// enable screen dim
#ifdef BASE_PLATFORM_iOS
	[[UIApplication sharedApplication] setIdleTimerDisabled:NO];
#endif // BASE_PLATFORM_iOS
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int EndingState::TransitionIn()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int EndingState::TransitionOut()
{
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void EndingState::Update( float deltaTime )
{
	m_LastDelta = deltaTime;

	if( gDebugCamera.IsEnabled() )
		gDebugCamera.Update( deltaTime );

	m_Colonel->Update( deltaTime );

	math::Vec3 currentHeadPos = math::Vec3( m_Colonel->GetBoneJointsPtr()[4].final.m41, m_Colonel->GetBoneJointsPtr()[4].final.m42, m_Colonel->GetBoneJointsPtr()[4].final.m43 );
	currentHeadPos -= m_CigaretteOffset;

	m_pSmokeEmitter->SetPos( currentHeadPos );
	m_pSmokeEmitter->Update( deltaTime );

	//m_pBurnEmitter->SetPos( currentHeadPos );
	//m_pBurnEmitter->Update( deltaTime );

	m_pCupEmitter->Update( deltaTime );

	// do any transitions
	if( UpdateLoadState() )
	{
		// returning true means the state has changed, this is not a valid class anymore
		return;
	}

	m_TextElements.Update( TOUCH_SIZE*0.5f, 1, deltaTime );

	/*if( m_LoadTransitionState == LoadState_None )
	{
		m_Timer += deltaTime;

		if( m_Timer >= 10.0f )
		{
			m_LoadTransitionState = LoadState_TransitionOut;
			return;
		}
	}*/

	if( !gDebugCamera.IsEnabled() )
	{
		if( m_GoToGameStart )
			ProcessStartCameras();
		else if( m_GoToGameEnd )
			ProcessEndingCameras();
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void EndingState::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->DisableLighting();


	if( gDebugCamera.IsEnabled() )
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.01f, 20.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

		math::Vec3 camPos = gDebugCamera.GetPosition();
		math::Vec3 camTarget = gDebugCamera.GetTarget();
		float camAngle = gDebugCamera.GetAngle();

		renderer::OpenGL::GetInstance()->SetLookAt( camPos.X, camPos.Y, camPos.Z, camTarget.X, camTarget.Y, camTarget.Z );
	
		float sinVal, cosVal;
		math::sinCos( &sinVal, &cosVal, math::DegToRad(camAngle) );

		snd::SoundManager::GetInstance()->SetListenerPosition( camPos.X, camPos.Y, camPos.Z );
		snd::SoundManager::GetInstance()->SetListenerOrientation( sinVal, 0.0f, -cosVal, 0.0f, 1.0f, 0.0f );
	}
	else
	{
		renderer::OpenGL::GetInstance()->SetNearFarClip( 0.01f, 20.0f );
		renderer::OpenGL::GetInstance()->SetupPerspectiveView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );


		renderer::OpenGL::GetInstance()->SetLookAt( m_CamPos.X, m_CamPos.Y, m_CamPos.Z, m_CamTarget.X, m_CamTarget.Y, m_CamTarget.Z );
	}

	renderer::OpenGL::GetInstance()->EnableLighting();
	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LEQUAL );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	m_Colonel->Draw();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );

	renderer::OpenGL::GetInstance()->DisableLighting();
	//math::Vec4 m_SceneAmbient = math::Vec4(0.2f, 0.5f, 0.2f, 1.0f);
	//glLightModelfv( GL_LIGHT_MODEL_AMBIENT, &m_SceneAmbient[0] );

	m_EndingMesh->Draw();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255,255,255,255 );
	m_pSmokeEmitter->Draw();
	//m_pBurnEmitter->Draw();

	m_pCupEmitter->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_TextElements.Draw();

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
bool EndingState::UpdateLoadState()
{
	switch( m_LoadTransitionState )
	{
		case LoadState_None:

		break;
		case LoadState_TransitionIn:
		{
			m_TransitionAlpha -= 4;

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
			m_TransitionAlpha += 4;
	
			core::app::SetSmoothUpdate(false);
			
			if( m_TransitionAlpha > 255 )
			{
				m_TransitionAlpha = 255;
				core::app::SetSmoothUpdate(true);

				m_LoadTransitionState = LoadState_None;

				if( m_GoToGameStart )
				{
					if( !AudioSystem::GetInstance()->IsPlaying() )
					{
						ChangeState( new IntroOutroState( IntroOutroState::IntroOutroState_Intro, *m_pStateManager, m_InputSystem) );
						return(true);
					}
				}
				else if( m_GoToGameEnd )
				{
					if( !AudioSystem::GetInstance()->IsPlaying() )
					{
						ChangeState( new IntroOutroState( IntroOutroState::IntroOutroState_Outro, *m_pStateManager, m_InputSystem) );
						return(true);
					}
				}
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
bool EndingState::ScreenTap()
{
	if( input::gInputState.TouchesData[input::FIRST_TOUCH].bRelease )
	{
		return(true);
	}
 
	return(false);
}

/////////////////////////////////////////////////////
/// Method: ProcessEndingCameras
/// Params: None
///
/////////////////////////////////////////////////////
void EndingState::ProcessStartCameras()
{
	switch( m_EndingCameraIndex )
	{
		case 0:
		{
			m_CamPos = shoulderIntroPos;
			m_CamTarget = shoulderIntroTarget;

			m_Timer -= m_LastDelta;
			if( m_Timer <= 0.0f )
			{
				m_EndingCameraIndex = 1;
				m_Timer = STARTTIME_CAM1;

				const char* sr = res::GetScriptString( 400 );
				m_TextElements.ChangeElementText( TEXT_LINE1, sr );
				sr = res::GetScriptString( 401 );
				m_TextElements.ChangeElementText( TEXT_LINE2, sr );
			}

		}break;
		case 1:
		{
			if( !m_StartZoomOut )
			{
				m_CamPos = shoulderIntroPos;
				m_CamTarget = shoulderIntroTarget;

				m_StartZoomOut = true;
			}
			else
			{
				const float LERP_TOLERANCE = 0.004f;

				m_CamPos = math::Lerp( m_CamPos, screenZoomPos, LERP_TOLERANCE );
				m_CamTarget = math::Lerp( m_CamTarget, screenZoomTarget, LERP_TOLERANCE );

				// finished
				m_Timer -= m_LastDelta;
				if( m_Timer <= 4.0f )
				{
					const char* sr = res::GetScriptString( 402 );
					m_TextElements.ChangeElementText( TEXT_LINE1, sr );
					sr = res::GetScriptString( 403 );
					m_TextElements.ChangeElementText( TEXT_LINE2, sr );
				}

				if( m_Timer <= 0.0f )
				{
					if( m_LoadTransitionState == LoadState_None )
					{
						m_LoadTransitionState = LoadState_TransitionOut;
						return;
					}
					m_Timer = 0.0f;
				}
			}

		}break;

		default:
			DBG_ASSERT(0);
			break;
	}
}

/////////////////////////////////////////////////////
/// Method: ProcessEndingCameras
/// Params: None
///
/////////////////////////////////////////////////////
void EndingState::ProcessEndingCameras()
{
	switch( m_EndingCameraIndex )
	{
		case 0:
		{
			if( !m_StartZoomOut )
			{
				m_CamPos = screenOutroZoomPos;
				m_CamTarget = screenOutroZoomTarget;

				m_Timer -= m_LastDelta;
				if( m_Timer <= 0.0f )
				{
					m_Timer = ENDTIME_CAM1;

					const char* sr = res::GetScriptString( 502 );
					m_TextElements.ChangeElementText( TEXT_LINE1, sr );
					sr = res::GetScriptString( 503 );
					m_TextElements.ChangeElementText( TEXT_LINE2, sr );

					m_StartZoomOut = true;
				}
			}
			else
			{
				const float LERP_TOLERANCE = 0.004f;

				m_CamPos = math::Lerp( m_CamPos, shoulderOutroPos, LERP_TOLERANCE );
				m_CamTarget = math::Lerp( m_CamTarget, shoulderOutroTarget, LERP_TOLERANCE );

				// finished
				m_Timer -= m_LastDelta;
				if( m_Timer <= 0.0f )
				{
					m_StartZoomOut = false;
					m_EndingCameraIndex = 1;
					m_Timer = ENDTIME_CAM2;

					const char* sr = res::GetScriptString( 504 );
					m_TextElements.ChangeElementText( TEXT_LINE1, sr );
					sr = res::GetScriptString( 505 );
					m_TextElements.ChangeElementText( TEXT_LINE2, sr );
				}
			}
		}break;
		case 1:
		{
			m_CamPos = facePos;
			m_CamTarget = faceTarget;

			m_Timer -= m_LastDelta;
			if( m_Timer <= 0.0f )
			{
				m_EndingCameraIndex = 2;
				m_Timer = ENDTIME_CAM3;
			}

		}break;
		case 2:
		{
			if( !m_StartZoomOut )
			{
				m_CamPos = facePos;
				m_CamTarget = faceTarget;

				const char* sr = res::GetScriptString( 506 );
				m_TextElements.ChangeElementText( TEXT_LINE1, sr );
				sr = res::GetScriptString( 507 );
				m_TextElements.ChangeElementText( TEXT_LINE2, sr );

				m_StartZoomOut = true;
			}
			else
			{
				const float LERP_TOLERANCE = 0.002f;

				m_CamPos = math::Lerp( m_CamPos, faceEndPos, LERP_TOLERANCE );
				m_CamTarget = math::Lerp( m_CamTarget, faceEndTarget, LERP_TOLERANCE );

				// finished
				m_Timer -= m_LastDelta;

				if( m_Timer <= 7.0f )
				{			
					const char* sr = res::GetScriptString( 508 );
					m_TextElements.ChangeElementText( TEXT_LINE1, sr );
					sr = res::GetScriptString( 509 );
					m_TextElements.ChangeElementText( TEXT_LINE2, sr );
				}

				if( m_Timer <= 0.0f )
				{
					m_EndingCameraIndex = 3;
					m_StartZoomOut = false;
					m_Timer = 0.0f;
				}
			}
		}break;
		case 3:
		{

			if( m_LoadTransitionState == LoadState_None )
			{
				m_LoadTransitionState = LoadState_TransitionOut;
				return;
			}
		}break;
		default:
			DBG_ASSERT(0);
			break;
	}
}
