
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
#include "MicroGameSliderHold.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_sliderhold.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_sliderhold.hui.h" // UI element list

namespace
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	const int MAX_TOUCH_TESTS = 3;
	const int MAX_RANGE = 10;

	const float MAX_SLIDER_HEIGHT = 199.0f;
	const float RESET_SPEED = 50.0f;
	const float SCROLL_SPEED = 25.0f;
	const float HOLD_TIME = 3.0f;

	float FINAL_MAX_SLIDER_HEIGHT = MAX_SLIDER_HEIGHT;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameSliderHold::MicroGameSliderHold( unsigned int slider1, unsigned int slider2, unsigned int slider3 )
{
	m_SliderModel = 0;

	FINAL_MAX_SLIDER_HEIGHT = MAX_SLIDER_HEIGHT*core::app::GetHeightScale();

	m_Slider1 = slider1;
	m_Slider2 = slider2;
	m_Slider3 = slider3;

	m_Slider1Value = 0;
	m_Slider2Value = 0;
	m_Slider3Value = 0;

	m_HoldTimer = HOLD_TIME;

	m_ScrollTextPos = math::Vec3( 77.0f*core::app::GetWidthScale(), 162.0f*core::app::GetHeightScale(), 0.0f );
	m_ScrollText = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_TickAudio = snd::INVALID_SOUNDBUFFER;
	m_ComputerLoopAudio = snd::INVALID_SOUNDBUFFER;
	m_ComputerLoopSource = snd::INVALID_SOUNDSOURCE;
	m_TickTime = 0.0f;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameSliderHold::~MicroGameSliderHold()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameSliderHold::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameSliderHold::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameSliderHold::OnEnter()
{
	int i=0;
	m_SliderModel = res::LoadModel( 2001 );
	DBG_ASSERT( (m_SliderModel != 0) );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=SLIDERHOLD_BGMESH; i <= SLIDERHOLD_TIMERMESH; ++i )
	{		
		m_SliderModel->SetMeshScale(i, scaleFactor);
	}

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_SliderUI.Load( "hui/microgame_sliderhold.hui", srcAssetDims );

	m_Slider1StartPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER1 );
	m_Slider2StartPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER2 );
	m_Slider3StartPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER3 );

	m_Range = static_cast<int>( (FINAL_MAX_SLIDER_HEIGHT - m_Slider1StartPos.Y) / MAX_RANGE );

	m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER1MESH_D, false );
	m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER2MESH_D, false );
	m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER3MESH_D, false );

	char text[UI_MAXSTATICTEXTBUFFER_SIZE];
	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0" );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_VAL1, text );
	
	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0" );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_VAL2, text );
	
	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "0" );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_VAL3, text );
	
	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%.2f", HOLD_TIME );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_TIME, text );	
	
	const char* sr = 0;
	snd::Sound sndLoad;

	sr = res::GetSoundResource( 120 );
	m_TickAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_TickAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 47 );
	m_ComputerLoopAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_ComputerLoopAudio != snd::INVALID_SOUNDBUFFER) );

	m_ComputerLoopSource = AudioSystem::GetInstance()->PlayAudio( m_ComputerLoopAudio, zeroVec, true, true );
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameSliderHold::OnExit()
{
	res::RemoveModel( m_SliderModel );
	m_SliderModel = 0;

	if( m_ComputerLoopSource != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio(m_ComputerLoopSource);
		m_ComputerLoopSource = snd::INVALID_SOUNDSOURCE;
	}

	snd::RemoveSound( m_TickAudio );
	snd::RemoveSound( m_ComputerLoopAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameSliderHold::Update( float deltaTime )
{
	// update UI elements
	m_SliderUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_SliderUI.CheckElementForSingleTouch( SLIDERHOLD_EXIT ) )
	{
		m_ReturnCode = false;
		m_Complete = true;
		return;
	}

// SLIDER1
	if(m_SliderUI.CheckElementForTouch( SLIDERHOLD_SLIDER1, UIFileLoader::EVENT_PRESS ) || 
	   m_SliderUI.CheckElementForTouch( SLIDERHOLD_SLIDER1, UIFileLoader::EVENT_HELD ) )
	{
		math::Vec3 touchPos = m_SliderUI.GetLastTouchPosition( SLIDERHOLD_SLIDER1 );

		if( touchPos.Y > FINAL_MAX_SLIDER_HEIGHT )
			touchPos.Y = FINAL_MAX_SLIDER_HEIGHT;

		if( touchPos.Y < m_Slider1StartPos.Y )
			touchPos.Y = m_Slider1StartPos.Y;

		math::Vec3 newPos = math::Vec3( m_Slider1StartPos.X, touchPos.Y, m_Slider1StartPos.Z );
		m_SliderUI.ChangeElementPosition( SLIDERHOLD_SLIDER1, newPos );

		math::Vec3 meshTrans = newPos - m_Slider1StartPos;
		m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER1MESH_N, meshTrans );

		m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER1MESH_D, true );
		m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER1MESH_D, meshTrans );
	}
	else
	{
		math::Vec3 currPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER1 );

		if( currPos.Y > m_Slider1StartPos.Y )
		{
			currPos.Y -= RESET_SPEED*deltaTime;

			if( currPos.Y < m_Slider1StartPos.Y )
				currPos.Y = m_Slider1StartPos.Y;

			m_SliderUI.ChangeElementPosition( SLIDERHOLD_SLIDER1, currPos );

			math::Vec3 meshTrans = currPos - m_Slider1StartPos;
			m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER1MESH_N, meshTrans );
		}
		
		m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER1MESH_D, false );
	}

// SLIDER 2
	if(m_SliderUI.CheckElementForTouch( SLIDERHOLD_SLIDER2, UIFileLoader::EVENT_PRESS ) || 
	   m_SliderUI.CheckElementForTouch( SLIDERHOLD_SLIDER2, UIFileLoader::EVENT_HELD ) )
	{
		math::Vec3 touchPos = m_SliderUI.GetLastTouchPosition( SLIDERHOLD_SLIDER2 );

		if( touchPos.Y > FINAL_MAX_SLIDER_HEIGHT )
			touchPos.Y = FINAL_MAX_SLIDER_HEIGHT;

		if( touchPos.Y < m_Slider2StartPos.Y )
			touchPos.Y = m_Slider2StartPos.Y;

		math::Vec3 newPos = math::Vec3( m_Slider2StartPos.X, touchPos.Y, m_Slider2StartPos.Z );
		m_SliderUI.ChangeElementPosition( SLIDERHOLD_SLIDER2, newPos );

		math::Vec3 meshTrans = newPos - m_Slider2StartPos;
		m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER2MESH_N, meshTrans );

		m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER2MESH_D, true );
		m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER2MESH_D, meshTrans );
	}
	else
	{
		math::Vec3 currPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER2 );

		if( currPos.Y > m_Slider2StartPos.Y )
		{
			currPos.Y -= RESET_SPEED*deltaTime;

			if( currPos.Y < m_Slider2StartPos.Y )
				currPos.Y = m_Slider2StartPos.Y;

			m_SliderUI.ChangeElementPosition( SLIDERHOLD_SLIDER2, currPos );

			math::Vec3 meshTrans = currPos - m_Slider2StartPos;
			m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER2MESH_N, meshTrans );
		}

		m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER2MESH_D, false );
	}

// SLIDER 3
	if(m_SliderUI.CheckElementForTouch( SLIDERHOLD_SLIDER3, UIFileLoader::EVENT_PRESS ) ||
	   m_SliderUI.CheckElementForTouch( SLIDERHOLD_SLIDER3, UIFileLoader::EVENT_HELD ) )
	{
		math::Vec3 touchPos = m_SliderUI.GetLastTouchPosition( SLIDERHOLD_SLIDER3 );

		if( touchPos.Y > FINAL_MAX_SLIDER_HEIGHT )
			touchPos.Y = FINAL_MAX_SLIDER_HEIGHT;
		if( touchPos.Y < m_Slider3StartPos.Y )
			touchPos.Y = m_Slider3StartPos.Y;

		math::Vec3 newPos = math::Vec3( m_Slider3StartPos.X, touchPos.Y, m_Slider3StartPos.Z );
		m_SliderUI.ChangeElementPosition( SLIDERHOLD_SLIDER3, newPos );

		math::Vec3 meshTrans = newPos - m_Slider3StartPos;
		m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER3MESH_N, meshTrans );

		m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER3MESH_D, true );
		m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER3MESH_D, meshTrans );
	}
	else
	{
		math::Vec3 currPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER3 );

		if( currPos.Y > m_Slider3StartPos.Y )
		{
			currPos.Y -= RESET_SPEED*deltaTime;

			if( currPos.Y < m_Slider3StartPos.Y )
				currPos.Y = m_Slider3StartPos.Y;

			m_SliderUI.ChangeElementPosition( SLIDERHOLD_SLIDER3, currPos );

			math::Vec3 meshTrans = currPos - m_Slider3StartPos;
			m_SliderModel->SetMeshTranslation( SLIDERHOLD_SLIDER3MESH_N, meshTrans );
		}
		
		m_SliderModel->SetMeshDrawState( SLIDERHOLD_SLIDER3MESH_D, false );
	}

	// update text
	math::Vec3 currPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER1 );
	m_Slider1Value = static_cast<int>( (currPos.Y - m_Slider1StartPos.Y) / m_Range );

	currPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER2 );
	m_Slider2Value = static_cast<int>( (currPos.Y - m_Slider2StartPos.Y) / m_Range );

	currPos = m_SliderUI.GetElementPosition( SLIDERHOLD_SLIDER3 );
	m_Slider3Value = static_cast<int>( (currPos.Y - m_Slider3StartPos.Y) / m_Range );

	char text[UI_MAXSTATICTEXTBUFFER_SIZE];
	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d", m_Slider1Value );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_VAL1, text );

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d", m_Slider2Value );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_VAL2, text );

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d", m_Slider3Value );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_VAL3, text );

	// holding
#if defined( BASE_PLATFORM_iOS ) || defined( BASE_PLATFORM_ANDROID )
	if( (m_Slider1Value == m_Slider1) &&
		(m_Slider2Value == m_Slider2) &&
		(m_Slider3Value == m_Slider3) )
	{
		m_HoldTimer -= deltaTime;
		
		m_TickTime += deltaTime;
		
		if( m_TickTime >= 1.0f )
		{
			AudioSystem::GetInstance()->PlayAudio( m_TickAudio, zeroVec, true );
			m_TickTime = 0.0f;
		}		

		if( m_HoldTimer < 0.0f )
		{
			m_HoldTimer = 0.0f;
			m_ReturnCode = true;
			m_Complete = true;
		}
	}
	else
	{
		m_HoldTimer = HOLD_TIME;
	}

	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%.2f", m_HoldTimer );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_TIME, text );
#else
	if( 
		(m_Slider2Value == m_Slider2)  )
	{
		m_HoldTimer -= deltaTime;

		m_TickTime += deltaTime;

		if( m_TickTime >= 1.0f )
		{
			AudioSystem::GetInstance()->PlayAudio( m_TickAudio, zeroVec, true );
			m_TickTime = 0.0f;
		}

		if( m_HoldTimer < 0.0f )
		{
			m_HoldTimer = 0.0f;
			m_ReturnCode = true;
			m_Complete = true;

			AudioSystem::GetInstance()->PlayAudio( m_TickAudio, zeroVec, true );
		}
	}
	else
	{
		m_HoldTimer = HOLD_TIME;
	}
	
	snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%.2f", m_HoldTimer );
	m_SliderUI.ChangeElementText( SLIDERHOLD_TEXT_TIME, text );
#endif 

	m_SliderModel->SetMeshTranslation( SLIDERHOLD_BGTEXTMESH, m_ScrollTextPos );

	m_ScrollText.X -= SCROLL_SPEED*deltaTime;
	m_SliderModel->SetMeshRotation( SLIDERHOLD_BGTEXTMESH, m_ScrollText );

	//if( m_ScrollText.X > 360.0f )
	//	m_ScrollText.X -= 360.0f;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameSliderHold::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_SliderModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_SliderUI.Draw();
}
