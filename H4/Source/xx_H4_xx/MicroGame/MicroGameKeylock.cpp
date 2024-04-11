
/*===================================================================
	File: MicroGameKeylock.cpp
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
#include "MicroGameKeylock.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_keylock.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_keylock.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;
	const float KEY_RANGE = 100.0f;

	const float MIN_X = 65.0f;
	const float MAX_X = 480.0f - 65.0f;

	const float MIN_Y = 37.5f;
	const float MAX_Y = 246.0f;

	float FINAL_MIN_X = MIN_X;
	float FINAL_MAX_X = MAX_X;

	float FINAL_MIN_Y = MIN_Y;
	float FINAL_MAX_Y = MAX_Y;

	const int KEY_COUNT = 4;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameKeylock::MicroGameKeylock( int keyIndex )
{
	m_KeylockModel = 0;

	m_KeyIndex = keyIndex;

	m_TurnValue = 25;

	m_KeyClickAudio = snd::INVALID_SOUNDBUFFER;
	
	FINAL_MIN_X = MIN_X * core::app::GetWidthScale();
	FINAL_MAX_X = MAX_X * core::app::GetWidthScale();

	FINAL_MIN_Y = MIN_Y * core::app::GetHeightScale();
	FINAL_MAX_Y = MAX_Y * core::app::GetHeightScale();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameKeylock::~MicroGameKeylock()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeylock::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeylock::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeylock::OnEnter()
{
	int i=0;

	m_KeylockModel = res::LoadModel( 2008 );
	DBG_ASSERT( (m_KeylockModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_KeylockUI.Load( "hui/microgame_keylock.hui", srcAssetDims );

	m_KeyInLock = false;
	
	for( i=1; i <= KEY_COUNT; ++i )
	{
		m_KeylockModel->SetMeshDrawState( KEYLOCK_KEYDRAGMESH_BLUE+((i*2)-2), false );
		m_KeylockModel->SetMeshDrawState( KEYLOCK_KEYINLOCKMESH_BLUE+((i*2)-2), false );
	}

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=KEYLOCK_BGMESH; i <= KEYLOCK_KEYINLOCKMESH_YELLOW; ++i )
	{		
		m_KeylockModel->SetMeshScale(i, scaleFactor);
	}

	m_KeyStartPos = m_KeylockUI.GetElementPosition( KEYLOCK_DRAGKEY );

	m_KeylockModel->SetMeshTranslation( KEYLOCK_KEYDRAGMESH_BLUE+((m_KeyIndex*2)-2), m_KeyStartPos );
	m_KeylockModel->SetMeshDrawState( KEYLOCK_KEYDRAGMESH_BLUE+((m_KeyIndex*2)-2), true );

	math::Vec3 pos = m_KeylockUI.GetElementPosition( KEYLOCK_KEYINLOCK );
	m_KeylockModel->SetMeshTranslation( KEYLOCK_KEYINLOCKMESH_BLUE+((m_KeyIndex*2)-2), pos );

	m_KeylockModel->SetMeshDrawState( KEYLOCK_SHADOWMESH, false );

	m_Range = KEY_RANGE/360.0f;

	const char* sr = 0;
	snd::Sound sndLoad;
	
	
	sr = res::GetSoundResource( 140 );
	m_KeyClickAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_KeyClickAudio != snd::INVALID_SOUNDBUFFER) );

}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeylock::OnExit()
{
	res::RemoveModel( m_KeylockModel );
	m_KeylockModel = 0;

	snd::RemoveSound( m_KeyClickAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameKeylock::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_KeylockUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( !m_HoldingKey &&
	   (m_KeylockUI.CheckElementForTouch( KEYLOCK_EXIT, UIFileLoader::EVENT_PRESS ) || 
		m_KeylockUI.CheckElementForTouch( KEYLOCK_EXIT, UIFileLoader::EVENT_HELD ) ) )
	{
		m_Complete = true;
		m_ReturnCode = false;

		return;
	}

	if( !m_KeyInLock )
	{
		if(m_KeylockUI.CheckElementForTouch( KEYLOCK_DRAGKEY, UIFileLoader::EVENT_PRESS ) ||  
		   m_KeylockUI.CheckElementForTouch( KEYLOCK_DRAGKEY, UIFileLoader::EVENT_HELD ) )
		{
			m_HoldingKey = true;

			math::Vec3 pos = m_KeylockUI.GetLastTouchPosition(KEYLOCK_DRAGKEY);

			if( pos.X > FINAL_MAX_X )
				pos.X = FINAL_MAX_X;
			if( pos.X < FINAL_MIN_X )
				pos.X = FINAL_MIN_X;

			if( pos.Y > FINAL_MAX_Y )
				pos.Y = FINAL_MAX_Y;
			if( pos.Y < FINAL_MIN_Y )
				pos.Y = FINAL_MIN_Y;

			m_KeylockUI.ChangeElementPosition(KEYLOCK_DRAGKEY, pos);
			m_KeylockModel->SetMeshTranslation(KEYLOCK_KEYDRAGMESH_BLUE+((m_KeyIndex*2)-2), pos);
		}
		else
			m_HoldingKey = false;
	}

	if( m_HoldingKey &&
		!m_KeyInLock &&
	   (m_KeylockUI.CheckElementForTouch( KEYLOCK_KEYHOLE, UIFileLoader::EVENT_PRESS ) ||
		m_KeylockUI.CheckElementForTouch( KEYLOCK_KEYHOLE, UIFileLoader::EVENT_HELD ) ) )
	{
		m_KeylockModel->SetMeshDrawState( KEYLOCK_KEYINLOCKMESH_BLUE+((m_KeyIndex*2)-2), true );

		m_KeylockModel->SetMeshDrawState( KEYLOCK_KEYDRAGMESH_BLUE+((m_KeyIndex*2)-2), false );

		m_KeylockModel->SetMeshDrawState( KEYLOCK_SHADOWMESH, true );

		m_KeyInLock = true;
		m_HoldingKey = false;

		AudioSystem::GetInstance()->PlayAudio( m_KeyClickAudio, zeroVec, true );
	}

	if( m_KeyInLock )
	{
		if(m_KeylockUI.CheckElementForTouch( KEYLOCK_ROTATEKEY, UIFileLoader::EVENT_PRESS ) || 
		   m_KeylockUI.CheckElementForTouch( KEYLOCK_ROTATEKEY, UIFileLoader::EVENT_HELD ) )
		{
			math::Vec3 dialPos = m_KeylockUI.GetElementPosition( KEYLOCK_ROTATEKEY );
			math::Vec3 touchPos = m_KeylockUI.GetLastTouchPosition( KEYLOCK_ROTATEKEY );
			math::Vec3 touchDir = touchPos - dialPos;

			touchDir.normalise();
			math::Vec3 north( 0.0f, 1.0f, 0.0f );
			
			float angle = math::AngleBetweenVectors( north, touchDir );

			if( (touchPos.X > dialPos.X) )
			{
				angle = (math::PI*2.0f) - angle;
			}
			else
			{
				// do not allow counter clockwise
				angle = 0.0f;
			}

			m_TurnAngle = static_cast<int>((360.0f-math::RadToDeg(angle))*m_Range);
			
			if( m_TurnAngle > 25 )
			{
				angle = 0.0f;
				m_TurnAngle = 0;
			}

			math::Vec3 rot( 0.0f, 0.0f, math::RadToDeg(angle) );
			m_KeylockModel->SetMeshRotation( KEYLOCK_KEYINLOCKMESH_BLUE+((m_KeyIndex*2)-2), rot );
		}
	}

	if( m_TurnAngle == m_TurnValue )
	{
		m_Complete = true;
		m_ReturnCode = true;
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameKeylock::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_KeylockModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_KeylockUI.Draw();
}
