
/*===================================================================
	File: MicroGameWiring.cpp
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
#include "MicroGameWiring.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_wiring.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_wiring.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 1;
	const float RESET_TIME = 2.0f;
	const float RESET_SPEED = 150.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameWiring::MicroGameWiring( int wireIndex1, int wireIndex2 )
{
	m_WiringModel = 0;

	m_CorrectWires = false;

	m_CorrectWire1 = wireIndex1;
	m_CorrectWire2 = wireIndex2;

	m_FirstWireIndex = -1;
	m_SecondWireIndex = -1;

	m_SelectingFirstWire = true;
	m_ResetTimer = 0.0f;

	m_SuccessAudio = snd::INVALID_SOUNDBUFFER;
	m_SparkAudio = snd::INVALID_SOUNDBUFFER;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameWiring::~MicroGameWiring()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWiring::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWiring::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWiring::OnEnter()
{
	m_WiringModel = res::LoadModel( 2003 );
	DBG_ASSERT( (m_WiringModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_WiringUI.Load( "hui/microgame_wiring.hui", srcAssetDims );

	// setup the wire list
	int i=0;
	for( i=0; i < MAX_WIRES; ++i )
	{
		m_WireList[i].startPos = m_WiringUI.GetElementPosition( WIRING_WIRE1+i );

		m_WiringModel->SetMeshTranslation( WIRING_WIREMESH1+i, m_WireList[i].startPos );
	}

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=WIRING_BGMESH; i <= WIRING_WIREMESH6; ++i )
	{		
		m_WiringModel->SetMeshScale(i, scaleFactor);
	}

	// WIRE1 
	m_WireList[0].minLimits = math::Vec3( 0.0f, 0.0f, 0.0f );
	m_WireList[0].maxLimits = math::Vec3( 240.0f*core::app::GetWidthScale(), 160.0f*core::app::GetHeightScale(), 0.0f );

	// WIRE2 
	m_WireList[1].minLimits = math::Vec3( 0.0f, 160.0f*core::app::GetHeightScale(), 0.0f );
	m_WireList[1].maxLimits = math::Vec3( 240.0f*core::app::GetWidthScale(), 320.0f*core::app::GetHeightScale(), 0.0f );

	// WIRE3 
	m_WireList[2].minLimits = math::Vec3( 0.0f, 160.0f*core::app::GetHeightScale(), 0.0f);
	m_WireList[2].maxLimits = math::Vec3( 240.0f*core::app::GetWidthScale(), 320.0f*core::app::GetHeightScale(), 0.0f );

	// WIRE4 
	m_WireList[3].minLimits = math::Vec3( 240.0f*core::app::GetWidthScale(), 160.0f*core::app::GetHeightScale(), 0.0f );
	m_WireList[3].maxLimits = math::Vec3( 480.0f*core::app::GetWidthScale(), 320.0f*core::app::GetHeightScale(), 0.0f);

	// WIRE5 
	m_WireList[4].minLimits = math::Vec3( 240.0f*core::app::GetWidthScale(), 160.0f*core::app::GetHeightScale(), 0.0f );
	m_WireList[4].maxLimits = math::Vec3( 480.0f*core::app::GetWidthScale(), 320.0f*core::app::GetHeightScale(), 0.0f );

	// WIRE6 
	m_WireList[5].minLimits = math::Vec3( 240.0f*core::app::GetWidthScale(), 0.0f, 0.0f );
	m_WireList[5].maxLimits = math::Vec3( 480.0f*core::app::GetWidthScale(), 160.0f*core::app::GetHeightScale(), 0.0f );

	const char* sr = 0;
	sr = res::GetSoundResource( 38 );

	snd::Sound sndLoad;
	m_SparkAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_SparkAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 180 );
	m_SuccessAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_SuccessAudio != snd::INVALID_SOUNDBUFFER) );
	
}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWiring::OnExit()
{
	res::RemoveModel( m_WiringModel );
	m_WiringModel = 0;

	snd::RemoveSound( m_SparkAudio );
	snd::RemoveSound( m_SuccessAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameWiring::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );
	int i=0;

	// update UI elements
	m_WiringUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_WiringUI.CheckElementForTouch( WIRING_EXIT ) )
	{
		m_Complete = true;
		m_ReturnCode = false;

		return;
	}

	// wait for audio to play
	if( m_CorrectWires )
	{
		m_ResetTimer -= deltaTime;

		if( m_ResetTimer < 0.0f )
		{
			m_Complete = true;
			m_ReturnCode = true;
		}
		return;
	}

	if( m_ResetTimer > 0.0f )
	{
		m_ResetTimer -= deltaTime;
		if( m_ResetTimer < 0.0f )
			m_ResetTimer = 0.0f;

		// reset
		for( i=0; i < MAX_WIRES; ++i )
		{
			math::Vec3 currPos = m_WiringUI.GetElementPosition( WIRING_WIRE1+i );

			if( currPos.X > m_WireList[i].startPos.X )
				currPos.X -= RESET_SPEED*deltaTime;
			else if( currPos.X < m_WireList[i].startPos.X )
				currPos.X += RESET_SPEED*deltaTime;

			if( currPos.Y > m_WireList[i].startPos.Y )
				currPos.Y -= RESET_SPEED*deltaTime;
			else if( currPos.Y < m_WireList[i].startPos.Y )
				currPos.Y += RESET_SPEED*deltaTime;

			if( std::fabs( currPos.X - m_WireList[i].startPos.X ) < 5.0f )
				currPos.X = m_WireList[i].startPos.X;

			if( std::fabs( currPos.Y - m_WireList[i].startPos.Y ) < 5.0f )
				currPos.Y = m_WireList[i].startPos.Y;

			m_WiringUI.ChangeElementPosition( WIRING_WIRE1+i, currPos );
			m_WiringModel->SetMeshTranslation( WIRING_WIREMESH1+i, currPos );
		}
		return;
	}

	int lastTouchId = -1;
	bool foundTouch = false;

	for( i=0; i < MAX_WIRES; ++i )
	{
		if( !foundTouch )
		{
			if(m_WiringUI.CheckElementForTouch( WIRING_WIRE1+i, UIFileLoader::EVENT_PRESS ) ||
			   m_WiringUI.CheckElementForTouch( WIRING_WIRE1+i, UIFileLoader::EVENT_HELD ) )
			{
				// don't allow moving the first wire
				if( !m_SelectingFirstWire )
					if( i == m_FirstWireIndex )
						continue;

				math::Vec3 touchPos = m_WiringUI.GetLastTouchPosition( WIRING_WIRE1+i );

				if( touchPos.X < m_WireList[i].minLimits.X )
					touchPos.X = m_WireList[i].minLimits.X;
				if( touchPos.X > m_WireList[i].maxLimits.X )
					touchPos.X = m_WireList[i].maxLimits.X;

				if( touchPos.Y < m_WireList[i].minLimits.Y )
					touchPos.Y = m_WireList[i].minLimits.Y;
				if( touchPos.Y > m_WireList[i].maxLimits.Y )
					touchPos.Y = m_WireList[i].maxLimits.Y;

				m_WiringUI.ChangeElementPosition( WIRING_WIRE1+i, touchPos );
				m_WiringModel->SetMeshTranslation( WIRING_WIREMESH1+i, touchPos );

				lastTouchId = i;
				foundTouch = true;
			}
		}
		
		if(!m_WiringUI.CheckElementForTouch( WIRING_WIRE1+i, UIFileLoader::EVENT_PRESS ) ||
		   !m_WiringUI.CheckElementForTouch( WIRING_WIRE1+i, UIFileLoader::EVENT_HELD ) )
		{
			if( i == m_FirstWireIndex )
				continue;

			math::Vec3 currPos = m_WiringUI.GetElementPosition( WIRING_WIRE1+i );

			if( currPos.X > m_WireList[i].startPos.X )
				currPos.X -= RESET_SPEED*deltaTime;
			else if( currPos.X < m_WireList[i].startPos.X )
				currPos.X += RESET_SPEED*deltaTime;

			if( currPos.Y > m_WireList[i].startPos.Y )
				currPos.Y -= RESET_SPEED*deltaTime;
			else if( currPos.Y < m_WireList[i].startPos.Y )
				currPos.Y += RESET_SPEED*deltaTime;

			if( std::fabs( currPos.X - m_WireList[i].startPos.X ) < 5.0f )
				currPos.X = m_WireList[i].startPos.X;

			if( std::fabs( currPos.Y - m_WireList[i].startPos.Y ) < 5.0f )
				currPos.Y = m_WireList[i].startPos.Y;

			m_WiringUI.ChangeElementPosition( WIRING_WIRE1+i, currPos );
			m_WiringModel->SetMeshTranslation( WIRING_WIREMESH1+i, currPos );
		}
	}

	if(m_WiringUI.CheckElementForTouch( WIRING_CENTRE, UIFileLoader::EVENT_PRESS ) || 
	   m_WiringUI.CheckElementForTouch( WIRING_CENTRE, UIFileLoader::EVENT_HELD ) )
	{
		if( m_SelectingFirstWire )
		{
			m_FirstWireIndex = lastTouchId;

			// lock the first wire
			if( m_FirstWireIndex != -1 )
			{
				m_SelectingFirstWire = false;

				AudioSystem::GetInstance()->PlayAudio( m_SparkAudio, zeroVec, true );

			}
		}
		else
			m_SecondWireIndex = lastTouchId;

		if( (m_FirstWireIndex != -1 && m_SecondWireIndex != -1) &&
			(m_SecondWireIndex != m_FirstWireIndex) )
		{
			if( !m_SelectingFirstWire )
			{
				if( (m_FirstWireIndex == m_CorrectWire1 && m_SecondWireIndex == m_CorrectWire2) ||
					(m_SecondWireIndex == m_CorrectWire1 && m_FirstWireIndex == m_CorrectWire2) )
				{
					AudioSystem::GetInstance()->PlayAudio( m_SparkAudio, zeroVec, true );

					AudioSystem::GetInstance()->PlayAudio( m_SuccessAudio, zeroVec, true );
					
					m_CorrectWires = true;
					m_ResetTimer = 2.0f;
				}
				else
				{
					AudioSystem::GetInstance()->PlayAudio( m_SparkAudio, zeroVec, true );

					// wrong two wires
					m_SelectingFirstWire = true;
					m_FirstWireIndex = -1;
					m_SecondWireIndex = -1;

					m_ResetTimer = RESET_TIME;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameWiring::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_WiringModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_WiringUI.Draw();
}
