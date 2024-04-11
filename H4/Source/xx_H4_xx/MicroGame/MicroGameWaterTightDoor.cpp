
/*===================================================================
	File: MicroGameWaterTightDoor.cpp
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
#include "MicroGameWaterTightDoor.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_watertightdoor.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_watertightdoor.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;
	const float WHEEL_RANGE = 100.0f;

	const int VAL_100 = 100;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameWaterTightDoor::MicroGameWaterTightDoor(  )
{
	m_DoorModel = 0;

	m_TurnAngle = 0;
	m_TurnValue = VAL_100;

	m_TurnAudio = snd::INVALID_SOUNDBUFFER;
	m_AudioPlayValue = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameWaterTightDoor::~MicroGameWaterTightDoor()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWaterTightDoor::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWaterTightDoor::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWaterTightDoor::OnEnter()
{
	int i=0;

	m_DoorModel = res::LoadModel( 2010 );
	DBG_ASSERT( (m_DoorModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_DoorUI.Load( "hui/microgame_watertightdoor.hui", srcAssetDims );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=WATERTIGHTDOOR_BGMESH; i <= WATERTIGHTDOOR_WHEELMESH; ++i )
	{		
		m_DoorModel->SetMeshScale(i, scaleFactor);
	}

	math::Vec3 pos = m_DoorUI.GetElementPosition( WATERTIGHTDOOR_WHEEL );
	m_DoorModel->SetMeshTranslation( WATERTIGHTDOOR_WHEELMESH, pos );
	m_DoorModel->SetMeshTranslation( WATERTIGHTDOOR_SHADOWMESH, pos );

	m_LastVal = 0;
	m_LastAngle = 0.0f;

	m_Range = WHEEL_RANGE/360.0f;

	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 150 );
	m_TurnAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_TurnAudio != snd::INVALID_SOUNDBUFFER) );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWaterTightDoor::OnExit()
{
	res::RemoveModel( m_DoorModel );
	m_DoorModel = 0;

	snd::RemoveSound( m_TurnAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameWaterTightDoor::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_DoorUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if(m_DoorUI.CheckElementForTouch( WATERTIGHTDOOR_EXIT, UIFileLoader::EVENT_PRESS ) ||
	   m_DoorUI.CheckElementForTouch( WATERTIGHTDOOR_EXIT, UIFileLoader::EVENT_HELD ) )
	{
		m_Complete = true;
		m_ReturnCode = false;

		return;
	}

	if(m_DoorUI.CheckElementForTouch( WATERTIGHTDOOR_WHEEL, UIFileLoader::EVENT_PRESS ) || 
	   m_DoorUI.CheckElementForTouch( WATERTIGHTDOOR_WHEEL, UIFileLoader::EVENT_HELD ) )
	{
		math::Vec3 touchPos = m_DoorUI.GetLastTouchPosition( WATERTIGHTDOOR_WHEEL );
		math::Vec3 wheelPos = m_DoorUI.GetElementPosition( WATERTIGHTDOOR_WHEEL );

#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )
		int lastIndex = m_DoorUI.GetLastTouchIndex(WATERTIGHTDOOR_WHEEL);

		const input::TInputState::TouchData* pData = 0;
		pData = input::GetTouch(lastIndex);

		if( touchPos.X >= wheelPos.X )
		{
			if( pData->nYDelta < 0.0f )
			{
				m_LastAngle -= 180.0f*deltaTime;

				m_AudioPlayValue += 10.0f*deltaTime;
			}
		}
		else
		{
			if( pData->nYDelta > 0.0f )
			{
				m_LastAngle -= 180.0f*deltaTime;

				m_AudioPlayValue += 10.0f*deltaTime;
			}
		}

		m_TurnAngle = static_cast<int>( -m_LastAngle*m_Range );
#else // 
		if( input::gInputState.TouchesData[input::FIRST_TOUCH].bHeld )
		{
			if( touchPos.X >= wheelPos.X )
			{
				if( input::gInputState.nMouseYDelta < 0.0f )
				{
					m_LastAngle -= 180.0f*deltaTime;

					m_AudioPlayValue += 10.0f*deltaTime;
				}
			}
			else
			{
				if( input::gInputState.nMouseYDelta > 0.0f )
				{
					m_LastAngle -= 180.0f*deltaTime;

					m_AudioPlayValue += 10.0f*deltaTime;
				}
			}
		}

		m_TurnAngle = static_cast<int>( -m_LastAngle*m_Range );
#endif 

		if( m_AudioPlayValue > 4.0f )
		{
			AudioSystem::GetInstance()->PlayAudio( m_TurnAudio, zeroVec, true );
			m_AudioPlayValue = 0.0f;
		}

		math::Vec3 rot( 0.0f, 0.0f, m_LastAngle );
		m_DoorModel->SetMeshRotation( WATERTIGHTDOOR_WHEELMESH, rot );
		m_DoorModel->SetMeshRotation( WATERTIGHTDOOR_SHADOWMESH, rot );
	}
	else
	{
		m_AudioPlayValue = 0.0f;
	}

	if( m_TurnAngle >= m_TurnValue )
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
void MicroGameWaterTightDoor::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_DoorModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_DoorUI.Draw();
}
