
/*===================================================================
	File: BackgroundUI.cpp
	Game: H8

	(C)Hidden Games
=====================================================================*/

#include "CoreBase.h"
#include "MathBase.h"
#include "RenderBase.h"
#include "SoundBase.h"
#include "CollisionBase.h"
#include "Input/Input.h"
#include "ModelBase.h"
#include "ScriptBase.h"

#include "H8Consts.h"
#include "H8.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/BackgroundUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/MainGameState.h"

//#include "GameStates/UI/GeneratedFiles/gameover.hgm.h"
#include "GameStates/UI/GeneratedFiles/overlay.hui.h"

namespace
{

}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
BackgroundUI::BackgroundUI()
{
	m_UIMesh = 0;
	m_GameData = GetScriptDataHolder()->GetGameData();

	m_HLayer1 = 0;
	m_HLayer2 = 0;
	m_HLayer3 = 0;

	m_VLayer1 = 0;
	m_VLayer2 = 0;
	m_VLayer3 = 0;

	m_HRotation1.setZero();
	m_HRotation2.setZero();
	m_HRotation3.setZero();

	m_VRotation1.setZero();
	m_VRotation2.setZero();
	m_VRotation3.setZero();

}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
BackgroundUI::~BackgroundUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void BackgroundUI::Enter()
{
	unsigned int i=0;
	math::Vec3 scaleFactor = math::Vec3( core::app::GetWidthScale(), core::app::GetHeightScale(), 1.0f );

	m_UIMesh = res::LoadModel( 0 );
	DBG_ASSERT( m_UIMesh != 0 );

	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_Elements.Load( "assets/bg/overlay.hui", srcDims, m_UIMesh );

	m_CylinderPos = m_Elements.GetElementPosition( TEXT_CYLINDER_POS );
	m_CylinderPos.Z = 0.0f;

	m_HLayer1 = res::LoadModel( 4 );
	DBG_ASSERT( m_HLayer1 != 0 );
	for( i=0; i < m_HLayer1->GetNumOpaqueMeshes(); ++i )	
	{
		m_HLayer1->SetMeshTranslation(i, m_CylinderPos);
		m_HLayer1->SetMeshScale( i, scaleFactor );
	}
	
	m_HLayer2 = res::LoadModel( 5 );
	DBG_ASSERT( m_HLayer2 != 0 );
	for( i=0; i < m_HLayer2->GetNumOpaqueMeshes(); ++i )	
	{
		m_HLayer2->SetMeshTranslation(i, m_CylinderPos);
		m_HLayer2->SetMeshScale( i, scaleFactor );
	}

	m_HLayer3 = res::LoadModel( 6 );
	DBG_ASSERT( m_HLayer3 != 0 );
	for( i=0; i < m_HLayer3->GetNumOpaqueMeshes(); ++i )		
	{
		m_HLayer3->SetMeshTranslation(i, m_CylinderPos);
		m_HLayer3->SetMeshScale( i, scaleFactor );
	}

	m_VLayer1 = res::LoadModel( 1 );
	DBG_ASSERT( m_VLayer1 != 0 );
	for( i=0; i < m_VLayer1->GetNumOpaqueMeshes(); ++i )
	{
		m_VLayer1->SetMeshTranslation(i, m_CylinderPos);
		m_VLayer1->SetMeshScale( i, scaleFactor );
	}
	m_VLayer2 = res::LoadModel( 2 );
	DBG_ASSERT( m_VLayer2 != 0 );
	for( i=0; i < m_VLayer2->GetNumOpaqueMeshes(); ++i )	
	{
		m_VLayer2->SetMeshTranslation(i, m_CylinderPos);
		m_VLayer2->SetMeshScale( i, scaleFactor );
	}
	m_VLayer3 = res::LoadModel( 3 );
	DBG_ASSERT( m_VLayer3 != 0 );
	for( i=0; i < m_VLayer3->GetNumOpaqueMeshes(); ++i )		
	{
		m_VLayer3->SetMeshTranslation(i, m_CylinderPos);
		m_VLayer3->SetMeshScale( i, scaleFactor );
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void BackgroundUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}

	if( m_HLayer1 != 0 )
	{
		res::RemoveModel( m_HLayer1 );
		m_HLayer1 = 0;
	}
	if( m_HLayer2 != 0 )
	{
		res::RemoveModel( m_HLayer2 );
		m_HLayer2 = 0;
	}
	if( m_HLayer3 != 0 )
	{
		res::RemoveModel( m_HLayer3 );
		m_HLayer3 = 0;
	}

	if( m_VLayer1 != 0 )
	{
		res::RemoveModel( m_VLayer1 );
		m_VLayer1 = 0;
	}
	if( m_VLayer2 != 0 )
	{
		res::RemoveModel( m_VLayer2 );
		m_VLayer2 = 0;
	}
	if( m_VLayer3 != 0 )
	{
		res::RemoveModel( m_VLayer3 );
		m_VLayer3 = 0;
	}

}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void BackgroundUI::Update( float deltaTime )
{
	unsigned int i=0;
	m_LastDeltaTime = deltaTime;

	//m_Elements.Update( TOUCH_SIZE_MENU, 1, deltaTime );

	// horizontal
	float speed = 0.0f;

	speed = m_GameData.BG_HLAYER1_SPEED + (static_cast<float>( GameSystems::GetInstance()->GetLevelCounter() ) * m_GameData.BG_HLAYER1_SPEED_INC_PER_LEVEL );
	if( std::abs(speed) > std::abs( m_GameData.BG_HLAYER1_MAX_SPEED ) )
		speed = m_GameData.BG_HLAYER1_MAX_SPEED;
	m_HRotation1.Y += speed;

	for( i=0; i < m_HLayer1->GetNumOpaqueMeshes(); ++i )		
		m_HLayer1->SetMeshRotation(i, m_HRotation1);

	speed = m_GameData.BG_HLAYER2_SPEED + (static_cast<float>( GameSystems::GetInstance()->GetLevelCounter() ) * m_GameData.BG_HLAYER2_SPEED_INC_PER_LEVEL );
	if( std::abs(speed) > std::abs( m_GameData.BG_HLAYER2_MAX_SPEED ) )
		speed = m_GameData.BG_HLAYER2_MAX_SPEED;
	m_HRotation2.Y += speed;

	for( i=0; i < m_HLayer2->GetNumOpaqueMeshes(); ++i )		
		m_HLayer2->SetMeshRotation(i, m_HRotation2);

	speed = m_GameData.BG_HLAYER3_SPEED + (static_cast<float>( GameSystems::GetInstance()->GetLevelCounter() ) * m_GameData.BG_HLAYER3_SPEED_INC_PER_LEVEL );
	if( std::abs(speed) > std::abs( m_GameData.BG_HLAYER3_MAX_SPEED ) )
		speed = m_GameData.BG_HLAYER3_MAX_SPEED;
	m_HRotation3.Y += speed;

	for( i=0; i < m_HLayer3->GetNumOpaqueMeshes(); ++i )		
		m_HLayer3->SetMeshRotation(i, m_HRotation3);

	// vertical
	speed = m_GameData.BG_VLAYER1_SPEED + (static_cast<float>( GameSystems::GetInstance()->GetLevelCounter() ) * m_GameData.BG_VLAYER1_SPEED_INC_PER_LEVEL );
	if( std::abs(speed) > std::abs( m_GameData.BG_VLAYER1_MAX_SPEED ) )
		speed = m_GameData.BG_VLAYER1_MAX_SPEED;
	m_VRotation1.X += speed;

	for( i=0; i < m_VLayer1->GetNumOpaqueMeshes(); ++i )		
		m_VLayer1->SetMeshRotation(i, m_VRotation1);

	speed = m_GameData.BG_VLAYER2_SPEED + (static_cast<float>( GameSystems::GetInstance()->GetLevelCounter() ) * m_GameData.BG_VLAYER2_SPEED_INC_PER_LEVEL );
	if( std::abs(speed) > std::abs( m_GameData.BG_VLAYER2_MAX_SPEED ) )
		speed = m_GameData.BG_VLAYER2_MAX_SPEED;
	m_VRotation2.X += speed;

	for( i=0; i < m_VLayer2->GetNumOpaqueMeshes(); ++i )		
		m_VLayer2->SetMeshRotation(i, m_VRotation2);

	speed = m_GameData.BG_VLAYER3_SPEED + (static_cast<float>( GameSystems::GetInstance()->GetLevelCounter() ) * m_GameData.BG_VLAYER3_SPEED_INC_PER_LEVEL );
	if( std::abs(speed) > std::abs( m_GameData.BG_VLAYER3_MAX_SPEED ) )
		speed = m_GameData.BG_VLAYER3_MAX_SPEED;
	m_VRotation3.X += speed;

	for( i=0; i < m_VLayer3->GetNumOpaqueMeshes(); ++i )		
		m_VLayer3->SetMeshRotation(i, m_VRotation3);

}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void BackgroundUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );

	//m_Elements.Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, m_GameData.BG_HLAYER1_BLEND_SRC, m_GameData.BG_HLAYER1_BLEND_DEST );
	if( m_HLayer1 != 0 )
		m_HLayer1->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, m_GameData.BG_HLAYER2_BLEND_SRC, m_GameData.BG_HLAYER2_BLEND_DEST);
	if( m_HLayer2 != 0 )
		m_HLayer2->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, m_GameData.BG_HLAYER3_BLEND_SRC, m_GameData.BG_HLAYER3_BLEND_DEST );
	if( m_HLayer3 != 0 )
		m_HLayer3->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, m_GameData.BG_VLAYER1_BLEND_SRC, m_GameData.BG_VLAYER1_BLEND_DEST );
	if( m_VLayer1 != 0 )
		m_VLayer1->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( true, m_GameData.BG_VLAYER2_BLEND_SRC, m_GameData.BG_VLAYER2_BLEND_DEST);
	if( m_VLayer2 != 0 )
		m_VLayer2->Draw();
	
	renderer::OpenGL::GetInstance()->BlendMode( true, m_GameData.BG_VLAYER3_BLEND_SRC, m_GameData.BG_VLAYER3_BLEND_DEST );
	if( m_VLayer3 != 0 )
		m_VLayer3->Draw();


	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}
