
/*===================================================================
	File: MicroGameDials.cpp
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
#include "MicroGameDials.h"

// generated files
#include "GameStates/UI/GeneratedFiles/microgame_dials.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/microgame_dials.hui.h" // UI element list

namespace
{
	const int MAX_TOUCH_TESTS = 2;
	const float DIAL_RANGE = 100.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameDials::MicroGameDials( unsigned int dial1, unsigned int dial2, unsigned int dial3 )
{
	m_DialsModel = 0;

	m_Dials[0].dialValue = dial1;
	m_Dials[1].dialValue = dial2;
	m_Dials[2].dialValue = dial3;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameDials::~MicroGameDials()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameDials::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameDials::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameDials::OnEnter()
{
	int i=0;
	m_DialsModel = res::LoadModel( 2004 );
	DBG_ASSERT( (m_DialsModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_DialsUI.Load( "hui/microgame_dials.hui", srcAssetDims );

	for( i = 0; i < 3; ++i )
	{
		math::Vec3 pos = m_DialsUI.GetElementPosition( DIAL_SPINNER1+i );
		m_DialsModel->SetMeshTranslation( DIAL_SPINNER1MESH+i, pos );
	}

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=DIAL_BGMESH; i <= DIAL_SPINNER3MESH; ++i )
	{		
		m_DialsModel->SetMeshScale(i, scaleFactor);
	}

	m_Range = DIAL_RANGE/360.0f;

	/*const char* sr = 0;
	sr = res::GetSoundResource( 100 );

	snd::Sound sndLoad;
	m_KeyAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_KeyAudio != snd::INVALID_SOUNDBUFFER) );

	sr = res::GetSoundResource( 101 );
	m_KeyEnterAudio = snd::SoundLoad( sr, sndLoad );
	DBG_ASSERT( (m_KeyEnterAudio != snd::INVALID_SOUNDBUFFER) );*/

}

/////////////////////////////////////////////////////
/// Method: OnExit
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameDials::OnExit()
{
	res::RemoveModel( m_DialsModel );
	m_DialsModel = 0;

	//snd::RemoveSound( m_KeyAudio );
	//snd::RemoveSound( m_KeyEnterAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameDials::Update( float deltaTime )
{
	int i=0;
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_DialsUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if(m_DialsUI.CheckElementForTouch( DIAL_EXIT, UIFileLoader::EVENT_PRESS ) || 
	   m_DialsUI.CheckElementForTouch( DIAL_EXIT, UIFileLoader::EVENT_HELD ) )
	{
		m_Complete = true;
		m_ReturnCode = false;

		return;
	}

	bool allCorrect = true;

	for( i=0; i < 3; ++i )
	{
		if(m_DialsUI.CheckElementForTouch( DIAL_SPINNER1+i, UIFileLoader::EVENT_PRESS ) ||  
		   m_DialsUI.CheckElementForTouch( DIAL_SPINNER1+i, UIFileLoader::EVENT_HELD ) )
		{
			math::Vec3 dialPos = m_DialsUI.GetElementPosition( DIAL_SPINNER1+i );
			math::Vec3 touchPos = m_DialsUI.GetLastTouchPosition( DIAL_SPINNER1+i );
			math::Vec3 touchDir = touchPos - dialPos;

			touchDir.normalise();
			math::Vec3 north( 0.0f, 1.0f, 0.0f );
			
			float angle = math::AngleBetweenVectors( north, touchDir );

			if( touchPos.X > dialPos.X )
			{
				angle = (math::PI*2.0f) - angle;
			}
			else
			{
				angle = (math::PI) + (angle-math::PI);
			}

			math::Vec3 rot( 0.0f, 0.0f, math::RadToDeg(angle) );
			m_DialsModel->SetMeshRotation( DIAL_SPINNER1MESH+i, rot );

			char text[UI_MAXSTATICTEXTBUFFER_SIZE];
			snprintf( text, UI_MAXSTATICTEXTBUFFER_SIZE, "%d", static_cast<int>( (360.0f-math::RadToDeg(angle))*m_Range) );
			m_DialsUI.ChangeElementText( DIAL_TEXT1-i, text );

			m_Dials[i].angle = static_cast<int>((360.0f-math::RadToDeg(angle))*m_Range);
		}

		if( m_Dials[i].angle != m_Dials[i].dialValue )
			allCorrect = false;
	}

	if( allCorrect )
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
void MicroGameDials::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_DialsModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_DialsUI.Draw();
}
