
/*===================================================================
	File: MicroGameThumbScan.cpp
	Game: H4

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "CollisionBase.h"
#include "SoundBase.h"
#include "ModelBase.h"
#include "InputBase.h"

#include "AppConsts.h"
#include "H4.h"

#include "Audio/AudioSystem.h"

#include "Resources/SoundResources.h"
#include "Resources/ModelResources.h"
#include "MicroGameThumbScan.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_thumbscan.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_thumbscan.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;

	const float ERROR_MESSAGE_TIME = 1.0f;
	const float SUCCESS_MESSAGE_TIME = 2.0f;
	const float MAX_Y = 162.0f;
	const float SCAN_SPEED = 100.0f;

	float FINAL_MAX_Y = MAX_Y;
	float FINAL_SCAN_SPEED = SCAN_SPEED;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameThumbScan::MicroGameThumbScan( )
{
	m_ThumbUIModel = 0;

	FINAL_MAX_Y = MAX_Y * core::app::GetHeightScale();
	FINAL_SCAN_SPEED = SCAN_SPEED * core::app::GetHeightScale();

	m_ShowMessageTime = 0.0f;
	m_SuccessMessage = false;

	m_ScanAudioSource = snd::INVALID_SOUNDSOURCE;
	m_ScanAudio = snd::INVALID_SOUNDBUFFER;

	m_ErrorAudio = snd::INVALID_SOUNDBUFFER;
	m_SuccessAudio = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameThumbScan::~MicroGameThumbScan()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameThumbScan::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameThumbScan::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameThumbScan::OnEnter()
{
	int i=0;

	m_ThumbUIModel = res::LoadModel( 2007 );
	DBG_ASSERT( (m_ThumbUIModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_ThumbUI.Load( "hui/microgame_thumbscan.hui", srcAssetDims );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=THUMBSCAN_EXITMESH; i <= THUMBSCAN_ERROR; ++i )
	{		
		m_ThumbUIModel->SetMeshScale(i, scaleFactor);
	}

	m_ScanActive = false;
	m_ScanPos = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_SwapDir = false;

	m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNING, false );
	m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_ERROR, false );
	m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SUCCESS, false );
	m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNINGMESH, false );
	
	m_SuccessMessage = false;
	m_ShowMessageTime = 0.0f;

	const char* sr = 0;
	snd::Sound sndLoad;
	
	sr = res::GetSoundResource( 130 );
	m_ScanAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ScanAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 131 );
	m_ErrorAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ErrorAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 132 );
	m_SuccessAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_SuccessAudio != snd::INVALID_SOUNDBUFFER) );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameThumbScan::OnExit()
{
	res::RemoveModel( m_ThumbUIModel );
	m_ThumbUIModel = 0;

	snd::RemoveSound( m_ScanAudio );
	snd::RemoveSound( m_ErrorAudio );
	snd::RemoveSound( m_SuccessAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameThumbScan::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_ThumbUI.Update( TOUCH_SIZE, MAX_TOUCH_TESTS, deltaTime );

	if( m_ShowMessageTime > 0.0f )
	{
		m_ShowMessageTime -= deltaTime;

		if( m_ShowMessageTime <= 0.0f )
		{
			m_ShowMessageTime = 0.0f;

			if( m_SuccessMessage )
			{
				m_ReturnCode = true;
				m_Complete = true;
				return;
			}
			else
			{
				m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SUCCESS, false );
				m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNING, false );
				m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_ERROR, false );
			}
		}

	}

	if( m_SuccessMessage )
		return;

	if( m_ThumbUI.CheckElementForSingleTouch(THUMBSCAN_EXIT) )
	{
		m_ReturnCode = false;
		m_Complete = true;
	}

	if(m_ThumbUI.CheckElementForTouch(THUMBSCAN_THUMBAREA, UIFileLoader::EVENT_PRESS) || 
	   m_ThumbUI.CheckElementForTouch(THUMBSCAN_THUMBAREA, UIFileLoader::EVENT_HELD) )
	{
		if( !m_ScanActive )
		{
			m_ScanActive = true;

			m_ScanAudioSource = AudioSystem::GetInstance()->PlayAudio( m_ScanAudio, zeroVec, true, true );
		}
		else
		{
			if( !m_SwapDir )
			{
				if( m_ScanPos.Y < FINAL_MAX_Y )
				{
					m_ScanPos.Y += FINAL_SCAN_SPEED*deltaTime;
				}
				else
				{
					m_SwapDir = true;
				}
			}
			else
			{
				if( m_ScanPos.Y > 0.0f )
				{
					m_ScanPos.Y -= FINAL_SCAN_SPEED*deltaTime;
				}
				else
				{
					m_ShowMessageTime = SUCCESS_MESSAGE_TIME;

					if( m_ScanAudioSource != snd::INVALID_SOUNDSOURCE )
					{
						AudioSystem::GetInstance()->StopAudio( m_ScanAudioSource );
						m_ScanAudioSource = snd::INVALID_SOUNDSOURCE;
					}

					AudioSystem::GetInstance()->PlayAudio( m_SuccessAudio, zeroVec, true );

					m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SUCCESS, true );
					m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNING, false );
					m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_ERROR, false );

					m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNINGMESH, false );
					m_SuccessMessage = true;
					return;
				}
			}
		}
		
		m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNINGMESH, true );
		m_ThumbUIModel->SetMeshTranslation( THUMBSCAN_SCANNINGMESH, m_ScanPos );
		m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNING, true );
	}
	else
	{
		if( m_ScanActive )
		{
			m_SwapDir = false;

			m_ScanPos.Y = 0.0f;

			m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNINGMESH, false );
			m_ThumbUIModel->SetMeshTranslation( THUMBSCAN_SCANNINGMESH, m_ScanPos );

			m_ShowMessageTime = ERROR_MESSAGE_TIME;

			if( m_ScanAudioSource != snd::INVALID_SOUNDSOURCE )
			{
				AudioSystem::GetInstance()->StopAudio( m_ScanAudioSource );
				m_ScanAudioSource = snd::INVALID_SOUNDSOURCE;
			}

			AudioSystem::GetInstance()->PlayAudio( m_ErrorAudio, zeroVec, true );

			m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SCANNING, false );
			m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_SUCCESS, false );
			m_ThumbUIModel->SetMeshDrawState( THUMBSCAN_ERROR, true );
			
			m_ScanActive = false;
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameThumbScan::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );
	
	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	
	
	m_ThumbUIModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_ThumbUI.Draw();
}
