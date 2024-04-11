
/*===================================================================
	File: MicroGameKeypad.cpp
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
#include "MicroGameCardSwipe.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_cardswipe.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_cardswipe.hui.h" // UI element list

namespace
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	const int MAX_TOUCH_TESTS = 1;

	const int CARD_COUNT = 5;

	const float MIN_X = 74.0f;
	const float MAX_X = 480.0f - 74.0f;

	const float MIN_Y = 90.0f + 20.0f;
	const float MAX_Y = 246.0f;

	const float ERROR_MESSAGE_TIME = 1.0f;
	const float SUCCESS_MESSAGE_TIME = 2.0f;

	const float SCROLL_SPEED = 25.0f;
	const float RESET_SPEED = 150.0f;

	float FINAL_MIN_X = MIN_X;
	float FINAL_MAX_X = MAX_X;
	float FINAL_MIN_Y = MIN_Y;
	float FINAL_MAX_Y = MAX_Y;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameCardSwipe::MicroGameCardSwipe( int cardIndex )
{
	m_SwipeUIModel = 0;

	m_CardIndex = cardIndex;

	FINAL_MIN_X = MIN_X * core::app::GetWidthScale();
	FINAL_MAX_X = MAX_X * core::app::GetWidthScale();
	FINAL_MIN_Y = MIN_Y * core::app::GetHeightScale();
	FINAL_MAX_Y = MAX_Y * core::app::GetHeightScale();

	m_ScrollTextPos = math::Vec3( 77.0f*core::app::GetWidthScale(), 162.0f* core::app::GetHeightScale(), 0.0f );
	m_ScrollText = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_SwipeActive = false;
	m_ShowMessageTime = 0.0f;
	m_SuccessMessage = false;

	m_UnlockAudio = snd::INVALID_SOUNDBUFFER;
	m_ErrorAudio = snd::INVALID_SOUNDBUFFER;
	m_ComputerLoopAudio = snd::INVALID_SOUNDBUFFER;

	m_ComputerLoopSource = snd::INVALID_SOUNDSOURCE;
	m_ErrorAudioTimer = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameCardSwipe::~MicroGameCardSwipe()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCardSwipe::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCardSwipe::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCardSwipe::OnEnter()
{
	int i=0;

	m_SwipeUIModel = res::LoadModel( 2006 );
	DBG_ASSERT( (m_SwipeUIModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_SwipeUI.Load( "hui/microgame_cardswipe.hui", srcAssetDims );

	m_CardStartPos = m_SwipeUI.GetElementPosition( CARDSWIPE_CARD );

	for( i=1; i <= CARD_COUNT; ++i )
	{
		m_SwipeUIModel->SetMeshTranslation(CARDSWIPE_CARD1MESH_D+((i*2)-2), m_CardStartPos);
		m_SwipeUIModel->SetMeshTranslation(CARDSWIPE_CARD1MESH_N+((i*2)-2), m_CardStartPos);

		m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_D+((i*2)-2), false);
		m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_N+((i*2)-2), false);
	}

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=CARDSWIPE_BGMESH; i <= CARDSWIPE_YELLOWLIGHTMESH; ++i )
	{		
		m_SwipeUIModel->SetMeshScale(i, scaleFactor);
	}

	m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_D+((m_CardIndex*2)-2), false);
	m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_N+((m_CardIndex*2)-2), true);

	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, true );
	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_SCANNINGMESH, false );
	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_UNLOCKEDMESH, false );	
	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_FAILEDMESH, false );

	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_REDLIGHTMESH, false );
	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_GREENLIGHTMESH, false );
	m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_YELLOWLIGHTMESH, false );
	
	m_SwipeUIModel->SetMeshTranslation( CARDSWIPE_BGTEXTMESH, m_ScrollTextPos );
	m_SwipeUIModel->SetMeshRotation( CARDSWIPE_BGTEXTMESH, m_ScrollText );	

	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 110 );
	m_UnlockAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_UnlockAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 111 );
	m_ErrorAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ErrorAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 47 );
	m_ComputerLoopAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ComputerLoopAudio != snd::INVALID_SOUNDBUFFER) );


	AudioSystem::GetInstance()->PlayAudio( m_ComputerLoopAudio, zeroVec, true, true );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCardSwipe::OnExit()
{
	res::RemoveModel( m_SwipeUIModel );
	m_SwipeUIModel = 0;

	if( m_ComputerLoopSource != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio(m_ComputerLoopSource);
		m_ComputerLoopSource = snd::INVALID_SOUNDSOURCE;
	}

	snd::RemoveSound( m_UnlockAudio );
	snd::RemoveSound( m_ErrorAudio );
	snd::RemoveSound( m_ComputerLoopAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameCardSwipe::Update( float deltaTime )
{
	// update UI elements
	m_SwipeUI.Update( TOUCH_SIZE, MAX_TOUCH_TESTS, deltaTime );

	m_ErrorAudioTimer -= deltaTime;
	if( m_ErrorAudioTimer < 0.0f )
		m_ErrorAudioTimer = 0.0f;

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
				if( !m_SwipeActive )
					m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, true );

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_UNLOCKEDMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_FAILEDMESH, false );

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_REDLIGHTMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_GREENLIGHTMESH, false );
			}
		}

	}

	if( m_SuccessMessage )
	{
		m_ScrollText.X -= SCROLL_SPEED*deltaTime;
		m_SwipeUIModel->SetMeshRotation( CARDSWIPE_BGTEXTMESH, m_ScrollText );
		return;
	}

	if( m_SwipeUI.CheckElementForSingleTouch(CARDSWIPE_EXIT) )
	{
		m_ReturnCode = false;
		m_Complete = true;
	}

	if(m_SwipeUI.CheckElementForTouch(CARDSWIPE_CARD, UIFileLoader::EVENT_PRESS) ||  
	   m_SwipeUI.CheckElementForTouch(CARDSWIPE_CARD, UIFileLoader::EVENT_HELD) )
	{
		math::Vec3 pos = m_SwipeUI.GetLastTouchPosition(CARDSWIPE_CARD);

		if( pos.X > FINAL_MAX_X )
			pos.X = FINAL_MAX_X;
		if( pos.X < FINAL_MIN_X )
			pos.X = FINAL_MIN_X;

		if( pos.Y > FINAL_MAX_Y )
			pos.Y = FINAL_MAX_Y;
		if( pos.Y < FINAL_MIN_Y )
			pos.Y = FINAL_MIN_Y;

		m_SwipeUI.ChangeElementPosition(CARDSWIPE_CARD, pos);
		m_SwipeUIModel->SetMeshTranslation(CARDSWIPE_CARD1MESH_N+((m_CardIndex*2)-2), pos);
		m_SwipeUIModel->SetMeshTranslation(CARDSWIPE_CARD1MESH_D+((m_CardIndex*2)-2), pos);

		m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_D+((m_CardIndex*2)-2), true);
		m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_N+((m_CardIndex*2)-2), false);

		if( !m_SwipeActive )
		{
			if( pos.X <= (175.0f*core::app::GetWidthScale()) &&
				pos.Y == FINAL_MIN_Y )
			{
				m_SwipeActive = true;

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_FAILEDMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_REDLIGHTMESH, false );

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_SCANNINGMESH, true );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_YELLOWLIGHTMESH, true );
			}
			else if( pos.Y == FINAL_MIN_Y )
			{
				m_ShowMessageTime = ERROR_MESSAGE_TIME;

				if( m_ErrorAudioTimer <= 0.0f )
				{
					AudioSystem::GetInstance()->PlayAudio( m_ErrorAudio, zeroVec, true );
					m_ErrorAudioTimer = 1.0f;
				}

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_FAILEDMESH, true );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_REDLIGHTMESH, true );

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_SCANNINGMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_YELLOWLIGHTMESH, false );
			}
		}
		else
		{
			if( pos.Y != FINAL_MIN_Y )
			{
				m_SwipeActive = false;

				m_ShowMessageTime = ERROR_MESSAGE_TIME;

				if( m_ErrorAudioTimer <= 0.0f )
				{
					AudioSystem::GetInstance()->PlayAudio( m_ErrorAudio, zeroVec, true );
					m_ErrorAudioTimer = 1.0f;
				}

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_SCANNINGMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_YELLOWLIGHTMESH, false );

				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, false );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_FAILEDMESH, true );
				m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_REDLIGHTMESH, true );
			}
			else
			{
				if( pos.X >= (350.0f*core::app::GetWidthScale()) )
				{
					m_SuccessMessage = true;
					m_ShowMessageTime = SUCCESS_MESSAGE_TIME;

					AudioSystem::GetInstance()->PlayAudio( m_UnlockAudio, zeroVec, true );

					m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_SCANNINGMESH, false );
					m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_YELLOWLIGHTMESH, false );

					m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, false );
					m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_UNLOCKEDMESH, true );
					m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_GREENLIGHTMESH, true );
				}
			}
		}
	}
	else
	{
		if( m_SwipeActive )
		{
			m_ShowMessageTime = ERROR_MESSAGE_TIME;

			if( m_ErrorAudioTimer <= 0.0f )
			{
				AudioSystem::GetInstance()->PlayAudio( m_ErrorAudio, zeroVec, true );
				m_ErrorAudioTimer = 1.0f;
			}

			m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_SCANNINGMESH, false );
			m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_YELLOWLIGHTMESH, false );

			m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_LOCKEDMESH, false );
			m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_FAILEDMESH, true );
			m_SwipeUIModel->SetMeshDrawState( CARDSWIPE_REDLIGHTMESH, true );
		}

		m_SwipeActive = false;

		math::Vec3 pos = m_SwipeUI.GetElementPosition(CARDSWIPE_CARD);

		if( pos.X > m_CardStartPos.X )
			pos.X -= RESET_SPEED*deltaTime;
		if( pos.X < m_CardStartPos.X )
			pos.X += RESET_SPEED*deltaTime;

		if( pos.Y > m_CardStartPos.Y )
			pos.Y -= RESET_SPEED*deltaTime;
		if( pos.Y < m_CardStartPos.Y )
			pos.Y += RESET_SPEED*deltaTime;

		if( std::fabs( pos.X - m_CardStartPos.X ) < 5.0f )
			pos.X = m_CardStartPos.X;

		if( std::fabs( pos.Y - m_CardStartPos.Y ) < 5.0f )
			pos.Y = m_CardStartPos.Y;

		m_SwipeUI.ChangeElementPosition(CARDSWIPE_CARD, pos);
		m_SwipeUIModel->SetMeshTranslation(CARDSWIPE_CARD1MESH_D+((m_CardIndex*2)-2), pos);
		m_SwipeUIModel->SetMeshTranslation(CARDSWIPE_CARD1MESH_N+((m_CardIndex*2)-2), pos);

		m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_D+((m_CardIndex*2)-2), false);
		m_SwipeUIModel->SetMeshDrawState(CARDSWIPE_CARD1MESH_N+((m_CardIndex*2)-2), true);
	}

	m_SwipeUIModel->SetMeshTranslation( CARDSWIPE_BGTEXTMESH, m_ScrollTextPos );

	m_ScrollText.X -= SCROLL_SPEED*deltaTime;
	m_SwipeUIModel->SetMeshRotation( CARDSWIPE_BGTEXTMESH, m_ScrollText );
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameCardSwipe::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );
	
	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );
	
	m_SwipeUIModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_SwipeUI.Draw();
}
