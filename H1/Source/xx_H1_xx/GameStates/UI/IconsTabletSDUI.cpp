
/*===================================================================
	File: IconsTabletSDUI.cpp
	Game: AirCadets

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

#include "H1Consts.h"
#include "H1.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"
#include "Profiles/ProfileManager.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/IconsTabletSDUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/icons_tablet_sd.hgm.h"
#include "GameStates/UI/GeneratedFiles/icons_tablet_sd.hui.h"

namespace
{
	const float AREA_RADIUS = 4.0f;

	math::Vec4Lite activeIcon( 255, 255, 255, 255 );
	math::Vec4Lite inactiveIcon( 255, 255, 255, 64 );
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
IconsTabletSDUI::IconsTabletSDUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_ICONSTABLETSD )
{
	m_Player = 0;//PhysicsWorld::GetPlayer();

	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;

	m_NumIconsDrawn = 0;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
IconsTabletSDUI::~IconsTabletSDUI()
{
	m_Player = 0;
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(2003);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/icons_tablet_sd.hui", srcDims, m_UIMesh );

	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );

	m_GameData = GetScriptDataHolder()->GetGameData();
	m_ActiveLevel = &GameSystems::GetInstance()->GetLevel();

	if( renderer::OpenGL::GetInstance()->GetIsRotated() )
	{
		m_HalfWidth = static_cast<const float>(core::app::GetOrientationHeight()/2);
		m_HalfHeight = static_cast<const float>(core::app::GetOrientationWidth()/2);
	
		m_OriWidth = static_cast<const float>(core::app::GetOrientationHeight());
		m_OriHeight = static_cast<const float>(core::app::GetOrientationWidth());	
	}
	else
	{
		m_HalfWidth = static_cast<const float>(core::app::GetOrientationWidth()/2);
		m_HalfHeight = static_cast<const float>(core::app::GetOrientationHeight()/2);
	
		m_OriWidth = static_cast<const float>(core::app::GetOrientationWidth());
		m_OriHeight = static_cast<const float>(core::app::GetOrientationHeight());
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Exit()
{
	m_ActiveLevel = 0;

	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
	
	m_Elements.Release();	
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int IconsTabletSDUI::TransitionIn()
{
	if( !m_FinishedTransitionIn )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition -= 15;
		if( m_FadeTransition <= 0 )
		{
			m_FadeTransition = 0;
			m_FinishedTransitionIn = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}

	// transition finished, make sure to do a clean draw
	Draw();
	return(0);
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int IconsTabletSDUI::TransitionOut()
{
	if( !m_FinishedTransitionOut )
	{
		// draw normal
		Draw();

		renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		// draw fade
		m_FadeTransition += 15;
		if( m_FadeTransition >= 255 )
		{
			m_FadeTransition = 255;
			m_FinishedTransitionOut = true;
		}
		DrawFullscreenQuad( 0.0f, m_FadeTransition );

		renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		return(1);
	}
	return(0);
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Update( float deltaTime )
{
	int i=0;
	m_LastDeltaTime = deltaTime;

	m_Elements.Update( TOUCH_SIZE_MENU, 2, deltaTime );

	const Level::TargetData* pData = m_ActiveLevel->GetTargetData();

	// disable all draws
	m_UIMesh->SetMeshDrawState( -1, false );
	m_Elements.ChangeElementDrawState( false );
	for( i=0; i < MAX_TARGET_DRAWS; ++i )
	{
		if( pData[i].inUse )
		{
			m_UIMesh->SetMeshDrawState( ICON_TARGET_1+i, true );

			m_Elements.ChangeElementPosition( HUI_ICON_TARGET_1+i, pData[i].unprojPoint );	
			m_Elements.ChangeElementDrawState( HUI_ICON_TARGET_1+i, true );	
		}
	}
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void IconsTabletSDUI::Draw()
{
	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

/////////////////////////////////////////////////////
/// Method: GetSafeIconPosition
/// Params: [in]iconPos, [in]oriWidth, [in]oriHeight
///
/////////////////////////////////////////////////////
math::Vec3& IconsTabletSDUI::GetSafeIconPosition( math::Vec3& iconPos, float oriWidth, float oriHeight )
{
/*	if( iconPos.X <= m_GameData.ICON_EDGE_CLOSEST )
		iconPos.X = m_GameData.ICON_EDGE_CLOSEST ;
	else if( iconPos.X >= (m_OriWidth-m_GameData.ICON_EDGE_CLOSEST) )
		iconPos.X = (m_OriWidth-m_GameData.ICON_EDGE_CLOSEST);

	// make sure not to hit the UI
	if( iconPos.Y <= m_GameData.ICON_LOWEST_POS ) // lower UI
		iconPos.Y = (m_GameData.ICON_LOWEST_POS );
	else if( iconPos.Y >= m_OriHeight-m_GameData.ICON_HIGHEST_POS ) // upper UI
		iconPos.Y = (m_OriHeight-m_GameData.ICON_HIGHEST_POS);
*/
	return iconPos;
}
