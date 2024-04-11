
/*===================================================================
	File: MicroGameMap.cpp
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
#include "MicroGameMap.h"

// generated files
#include "GameStates/UI/GeneratedFiles/map.h" // HGM submesh list
#include "GameStates/UI/GeneratedFiles/map.hui.h" // UI element list

namespace
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	const int MAX_TOUCH_TESTS = 1;
	const float BLINK_TIME = 0.5f;
	const float SCROLL_SPEED = 25.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
MicroGameMap::MicroGameMap( int mapIndex, float playerPosX, float playerPosY, float playerRot )
{
	m_MapModel = 0;

	m_MapIndex = mapIndex;
	m_PlayerPosX = playerPosX;
	m_PlayerPosY = playerPosY;
	m_PlayerRot = playerRot;

	m_KeyToggle = false;

	m_ScrollTextPos = math::Vec3( 77.0f*core::app::GetWidthScale(), 162.0f*core::app::GetHeightScale(), 0.0f );
	m_ScrollText = math::Vec3( 0.0f, 0.0f, 0.0f );

	m_BlinkTime = 0.0f;
	m_BlinkToggle = true;

	m_ComputerLoopAudio = snd::INVALID_SOUNDBUFFER;
	m_ComputerLoopSource = snd::INVALID_SOUNDSOURCE;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
MicroGameMap::~MicroGameMap()
{

}

/////////////////////////////////////////////////////
/// Method: Initialise
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameMap::Initialise()
{

}

/////////////////////////////////////////////////////
/// Method: Release
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameMap::Release()
{

}

/////////////////////////////////////////////////////
/// Method: OnEnter
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameMap::OnEnter()
{
	int i=0;

	m_MapModel = res::LoadModel( 2011 );
	DBG_ASSERT( (m_MapModel != 0) );

	math::Vec2 srcAssetDims( static_cast<float>(core::app::GetBaseAssetsWidth()), static_cast<float>(core::app::GetBaseAssetsHeight()) );
	m_MapUI.Load( "hui/map.hui", srcAssetDims );

	for( i=MAP_1; i<=MAP_10; ++i )
		m_MapModel->SetMeshDrawState( i, false );

	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );
	for( i=MAP_BGMESH; i <= MAP_10; ++i )
	{		
		m_MapModel->SetMeshScale(i, scaleFactor);
	}

	m_MapModel->SetMeshDrawState( MAP_1+(m_MapIndex-1), true );

	math::Vec3 playerPos( m_PlayerPosX*core::app::GetWidthScale(), m_PlayerPosY*core::app::GetHeightScale(), 0.0f );
	math::Vec3 playerRot( 0.0f, 0.0f, m_PlayerRot );

	m_MapModel->SetMeshTranslation( MAP_CURSORMESH, playerPos );
	m_MapModel->SetMeshRotation( MAP_CURSORMESH, playerRot );

	m_MapModel->SetMeshDrawState( MAP_KEYMINMESH, !m_KeyToggle);
	m_MapModel->SetMeshDrawState( MAP_KEYMAXMESH, m_KeyToggle);

	m_MapUI.ChangeElementText( MAP_LEVELNAME, GameSystems::GetInstance()->GetLevelName() );
	
	m_MapModel->SetMeshTranslation( MAP_BGTEXTMESH, m_ScrollTextPos );
	m_MapModel->SetMeshRotation( MAP_BGTEXTMESH, m_ScrollText );	

	const char* sr = 0;
	snd::Sound sndLoad;

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
void MicroGameMap::OnExit()
{
	res::RemoveModel( m_MapModel );
	m_MapModel = 0;

	if( m_ComputerLoopSource != snd::INVALID_SOUNDSOURCE )
	{
		AudioSystem::GetInstance()->StopAudio(m_ComputerLoopSource);
		m_ComputerLoopSource = snd::INVALID_SOUNDSOURCE;
	}

	snd::RemoveSound( m_ComputerLoopAudio );
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void MicroGameMap::Update( float deltaTime )
{
	math::Vec3 zeroVec( 0.0f, 0.0f, 0.0f );

	// update UI elements
	m_MapUI.Update( TOUCH_SIZE*0.5f, MAX_TOUCH_TESTS, deltaTime );

	if( m_MapUI.CheckElementForSingleTouch( MAP_EXIT ) )
	{
		m_ReturnCode = false;
		m_Complete = true;
		return;
	}

	if( m_MapUI.CheckElementForSingleTouch( MAP_TOGGLEKEY ) )
	{
		m_KeyToggle = !m_KeyToggle;

		m_MapModel->SetMeshDrawState( MAP_KEYMINMESH, !m_KeyToggle);
		m_MapModel->SetMeshDrawState( MAP_KEYMAXMESH, m_KeyToggle);
	}

	m_BlinkTime += deltaTime;
	if( m_BlinkTime >= BLINK_TIME )
	{
		m_BlinkTime = 0.0f;

		m_BlinkToggle = !m_BlinkToggle;
		m_MapModel->SetMeshDrawState( MAP_CURSORMESH, m_BlinkToggle);
	}

	m_MapModel->SetMeshTranslation( MAP_BGTEXTMESH, m_ScrollTextPos );

	m_ScrollText.X -= SCROLL_SPEED*deltaTime;
	m_MapModel->SetMeshRotation( MAP_BGTEXTMESH, m_ScrollText );

	//if( m_ScrollText.X > 360.0f )
	//	m_ScrollText.X -= 360.0f;
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void MicroGameMap::Draw()
{
	renderer::OpenGL::GetInstance()->ClearColour( 0.0f, 0.0f, 0.0f, 1.0f );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -500.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	renderer::OpenGL::GetInstance()->AlphaMode( false, GL_ALWAYS, 0.0f );

	m_MapModel->Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	renderer::OpenGL::GetInstance()->SetNearFarClip( -100.0f, 100.0f );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );

	m_MapUI.Draw();
}
