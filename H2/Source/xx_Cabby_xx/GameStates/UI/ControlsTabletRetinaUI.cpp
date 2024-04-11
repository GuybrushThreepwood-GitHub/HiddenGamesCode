
/*===================================================================
	File: ControlsTabletRetinaUI.cpp
	Game: Cabby

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

#include "CabbyConsts.h"
#include "Cabby.h"

#include "StateManage/IState.h"

#include "Resources/ModelResources.h"
#include "Resources/StringResources.h"

#include "GameEffects/FullscreenEffects.h"

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/ControlsTabletRetinaUI.h"
#include "GameStates/UI/UIFileLoader.h"

#include "GameStates/UI/GeneratedFiles/controls_tablet_sd.hgm.h"
#include "GameStates/UI/GeneratedFiles/controls_tablet_sd.hui.h"

namespace
{
	
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ControlsTabletRetinaUI::ControlsTabletRetinaUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_CONTROLSTABLETSD )
{
	m_Player = PhysicsWorld::GetPlayer();
	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;

	m_Pause = false;
	m_UpwardThrust = false;
	m_DownwardThrust = false;

	m_Left = false;
	m_Right = false;
	m_LandingGear = false;
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ControlsTabletRetinaUI::~ControlsTabletRetinaUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletRetinaUI::Enter()
{
	math::Vec2 srcDims( 1536.0f, 2048.0f );
	m_UIMesh = res::LoadModel(2006);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/controls_tablet_retina.hui", srcDims, m_UIMesh );

	if( core::app::IsPCOnly() )
	{
		m_UIMesh->SetMeshDrawState( BTN_LEFT, false );
		m_UIMesh->SetMeshDrawState( BTN_RIGHT, false );
		m_UIMesh->SetMeshDrawState( BTN_UP, false );
		m_UIMesh->SetMeshDrawState( BTN_DOWN, false );
		m_UIMesh->SetMeshDrawState( BTN_LANDGEAR, false );
	}
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletRetinaUI::Exit()
{
	if( m_UIMesh != 0 )
	{
		res::RemoveModel( m_UIMesh );
		m_UIMesh = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletRetinaUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletRetinaUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int ControlsTabletRetinaUI::TransitionIn()
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
int ControlsTabletRetinaUI::TransitionOut()
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
void ControlsTabletRetinaUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;

	m_Pause = false;
	m_UpwardThrust = false;
	m_DownwardThrust = false;

	m_Left = false;
	m_Right = false;
	m_LandingGear = false;
	
	m_Player = PhysicsWorld::GetPlayer();
	if( m_Player == 0 || m_Player->IsDead() )
		return;
	
	m_Elements.Update( TOUCH_SIZE_GAME, 3, deltaTime );

	if( m_Elements.CheckElementForSingleTouch( HUI_CONTROLS_BTN_PAUSE ) )
	{
		m_Pause = true;
	}

	if( !m_TiltControls )
	{
		if( m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_LEFT, UIFileLoader::EVENT_PRESS ) ||
			m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_LEFT, UIFileLoader::EVENT_HELD ) )
		{
			m_Left = true;
		}
		else
		if( m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_RIGHT, UIFileLoader::EVENT_PRESS ) ||
			m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_RIGHT, UIFileLoader::EVENT_HELD ) )
		{
			m_Right = true;
		}
	}
	else
	{
		if( renderer::OpenGL::GetInstance()->GetRotationStyle() == renderer::VIEWROTATION_LANDSCAPE_BUTTON_RIGHT )
		{
			if( input::gInputState.Accelerometers[1] >= TILT_CONTROL_THRESHOLD )
				m_Left = true;
			if( input::gInputState.Accelerometers[1] <= -TILT_CONTROL_THRESHOLD )
				m_Right = true;		
		}
		else if( renderer::OpenGL::GetInstance()->GetRotationStyle() == renderer::VIEWROTATION_LANDSCAPE_BUTTON_LEFT )
		{
			if( input::gInputState.Accelerometers[1] <= -TILT_CONTROL_THRESHOLD )
				m_Left = true;
			if( input::gInputState.Accelerometers[1] >= TILT_CONTROL_THRESHOLD )
				m_Right = true;					
		}
	}

	if( m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_UP, UIFileLoader::EVENT_PRESS ) ||
		m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_UP, UIFileLoader::EVENT_HELD ) )
	{
		m_UpwardThrust = true;
	}
	else
	if( m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_DOWN, UIFileLoader::EVENT_PRESS ) ||
		m_Elements.CheckElementForTouch( HUI_CONTROLS_BTN_DOWN, UIFileLoader::EVENT_HELD ) )
	{
		m_DownwardThrust = true;
	}

	if( m_Elements.CheckElementForSingleTouch( HUI_CONTROLS_BTN_LANDGEAR ) )
	{
		m_LandingGear = true;
	}
	
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletRetinaUI::Draw()
{
	m_Player = PhysicsWorld::GetPlayer();
	if( m_Player == 0 || m_Player->IsDead() )
		return;

	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();
}

/////////////////////////////////////////////////////
/// Method: SetupControls
/// Params: [in]tilt
///
/////////////////////////////////////////////////////
void ControlsTabletRetinaUI::SetupControls( int controlType )
{
    if( !core::app::IsPCOnly() )
    {
        if(controlType == 0)
        {
            m_TiltControls = false;
            m_UIMesh->SetMeshDrawState( BTN_LEFT, true );
            m_UIMesh->SetMeshDrawState( BTN_RIGHT, true );
            m_UIMesh->SetMeshDrawState( BTN_UP, true );
            m_UIMesh->SetMeshDrawState( BTN_DOWN, true );
            m_UIMesh->SetMeshDrawState( BTN_LANDGEAR, true );
            
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_LEFT, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_RIGHT, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_UP, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_DOWN, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_LANDGEAR, true);
        }
        else
        if(controlType == 1)
        {
            m_TiltControls = true;
            m_UIMesh->SetMeshDrawState( BTN_LEFT, false );
            m_UIMesh->SetMeshDrawState( BTN_RIGHT, false );
            m_UIMesh->SetMeshDrawState( BTN_UP, true );
            m_UIMesh->SetMeshDrawState( BTN_DOWN, true );
            m_UIMesh->SetMeshDrawState( BTN_LANDGEAR, true );
            
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_LEFT, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_RIGHT, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_UP, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_DOWN, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_LANDGEAR, true);
        }
        else
        if(controlType == 2)
        {
            m_TiltControls = false;
            m_UIMesh->SetMeshDrawState( BTN_LEFT, false );
            m_UIMesh->SetMeshDrawState( BTN_RIGHT, false );
            m_UIMesh->SetMeshDrawState( BTN_UP, false );
            m_UIMesh->SetMeshDrawState( BTN_DOWN, false );
            m_UIMesh->SetMeshDrawState( BTN_LANDGEAR, false );
            
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_LEFT, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_RIGHT, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_UP, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_DOWN, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_BTN_LANDGEAR, false);
        }
    }
}