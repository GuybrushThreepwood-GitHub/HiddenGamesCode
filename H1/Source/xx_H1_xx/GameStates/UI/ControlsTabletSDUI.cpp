
/*===================================================================
	File: ControlsTabletSDUI.cpp
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

#include "GameStates/UI/UIIds.h"
#include "GameStates/UI/UIBaseState.h"
#include "GameStates/UI/ControlsTabletSDUI.h"
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
ControlsTabletSDUI::ControlsTabletSDUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_CONTROLSTABLETSD )
{
	m_Player = &GameSystems::GetInstance()->GetPlayer();
	m_UIMesh = 0;

	m_FinishedTransitionIn = true;
	m_FinishedTransitionOut = true;

	m_Dive = false;
	m_Lift = false;

	m_Left = false;
	m_Right = false;
	m_Shoot = false;

	m_PlacedAnalogue = false;
	m_AnalogueTouchIndex = -1;

	m_AnalogueVales.setZero();
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ControlsTabletSDUI::~ControlsTabletSDUI()
{

}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletSDUI::Enter()
{
	math::Vec2 srcDims( static_cast<float>( core::app::GetBaseAssetsWidth() ), static_cast<float>( core::app::GetBaseAssetsHeight() ) );
	m_UIMesh = res::LoadModel(2001);
	DBG_ASSERT( m_UIMesh != 0 );

	m_Elements.Load( "assets/ui/controls_tablet_sd.hui", srcDims, m_UIMesh );

    if (core::app::IstvOS())
    {
        m_Elements.ChangeElementDrawState(HUI_CONTROLS_SHOOT, false);
        m_Elements.ChangeElementDrawState(HUI_GLYPH_CROSSHAIR, false);
        
        bool controllerForLeverExists = true;
        
#ifdef BASE_PLATFORM_tvOS
        bool foundController = false;
        if( ([[GCController controllers] count] > 0) )
        {
            for( GCController* controller in [GCController controllers] )
            {
                if( controller.extendedGamepad ||
                   controller.gamepad )
                {
                    foundController = true;
                }
            }
        }
        
        controllerForLeverExists = foundController;
#endif
        if( controllerForLeverExists )
        {
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_GAUGE, true);
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_LEVER, true);
            
            m_Elements.ChangeElementDrawState(HUI_GFX_THROTTLE_GAUGE, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_THROTTLE_LEVER, true);
        }
        else
        {
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_GAUGE, false);
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_LEVER, false);
            
            m_Elements.ChangeElementDrawState(HUI_GFX_THROTTLE_GAUGE, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_THROTTLE_LEVER, false);
        }
    }
    
	m_LeverStartPos = m_Elements.GetElementPosition( HUI_GFX_THROTTLE_GAUGE );
	m_Elements.ChangeElementPosition( HUI_CONTROLS_THROTTLE_LEVER, m_LeverStartPos );

	const UIFileLoader::UIElement* data = m_Elements.GetElement(HUI_GFX_THROTTLE_GAUGE);
	DBG_ASSERT( data != 0 );

	m_LeverMaxMin.minVal = data->aabb.vBoxMin.Y;
	m_LeverMaxMin.maxVal = data->aabb.vBoxMax.Y;

	m_LeverRange = (m_LeverMaxMin.maxVal - m_LeverMaxMin.minVal);

	// convert to a value for the target speed
	float offset = (m_LeverStartPos.Y - m_LeverMaxMin.minVal) / m_LeverRange;

	math::Vec2 planeSpeedRange = m_Player->GetSpeedRange();
	float range = planeSpeedRange.maxVal - planeSpeedRange.minVal;

	float finalSpeed = planeSpeedRange.minVal + (range*offset);
	m_Player->SetTargetSpeed( finalSpeed );

	data = m_Elements.GetElement(HUI_GFX_ANALOGUE_RANGE);
	DBG_ASSERT( data != 0 );

	m_AnalogueRadius = data->sphere.fRadius;

	m_UIMesh->SetMeshDrawState( GFX_ANALOGUE_RANGE, false );
	m_UIMesh->SetMeshDrawState( GFX_ANALOGUE_JOYSTICK, false );
	
	m_GameData = GetScriptDataHolder()->GetGameData();


}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletSDUI::Exit()
{
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
void ControlsTabletSDUI::PrepareTransitionIn()
{
	m_FinishedTransitionIn = false;
	m_FadeTransition = 255;
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsTabletSDUI::PrepareTransitionOut()
{
	m_FinishedTransitionOut = false;
	m_FadeTransition = 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int ControlsTabletSDUI::TransitionIn()
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
int ControlsTabletSDUI::TransitionOut()
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
void ControlsTabletSDUI::Update( float deltaTime )
{
	m_LastDeltaTime = deltaTime;
    
    if (core::app::IstvOS())
    {
        bool controllerForLeverExists = true;
        
#ifdef BASE_PLATFORM_tvOS
        bool foundController = false;
        if( ([[GCController controllers] count] > 0) )
        {
            for( GCController* controller in [GCController controllers] )
            {
                if( controller.extendedGamepad ||
                   controller.gamepad )
                {
                    foundController = true;
                }
            }
        }
        
        controllerForLeverExists = foundController;
#endif
        if( controllerForLeverExists )
        {
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_GAUGE, true);
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_LEVER, true);
            
            m_Elements.ChangeElementDrawState(HUI_GFX_THROTTLE_GAUGE, true);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_THROTTLE_LEVER, true);
        }
        else
        {
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_GAUGE, false);
            m_UIMesh->SetMeshDrawState(GFX_THROTTLE_LEVER, false);
            
            m_Elements.ChangeElementDrawState(HUI_GFX_THROTTLE_GAUGE, false);
            m_Elements.ChangeElementDrawState(HUI_CONTROLS_THROTTLE_LEVER, false);
        }
    }

	m_Dive = false;
	m_Lift = false;

	m_Left = false;
	m_Right = false;
	m_Shoot = false;
	
	//m_Player = 0;//PhysicsWorld::GetPlayer();
	//if( m_Player == 0  )
	//	return;
	
	m_Elements.Update( TOUCH_SIZE_GAME, 3, deltaTime );

	if( m_Elements.CheckElementForTouch( HUI_CONTROLS_SHOOT, UIFileLoader::EVENT_PRESS )||
		m_Elements.CheckElementForTouch( HUI_CONTROLS_SHOOT, UIFileLoader::EVENT_HELD ))
	{
		m_Shoot = true;
	}
	
	if( m_Elements.CheckElementForTouch( HUI_CONTROLS_THROTTLE_LEVER, UIFileLoader::EVENT_PRESS )||
		m_Elements.CheckElementForTouch( HUI_CONTROLS_THROTTLE_LEVER, UIFileLoader::EVENT_HELD ))
	{
		math::Vec3 curPos = m_Elements.GetLastTouchPosition(HUI_CONTROLS_THROTTLE_LEVER);

		curPos.X = m_LeverStartPos.X;
		if( curPos.Y > m_LeverMaxMin.maxVal )
		{
			curPos.Y = m_LeverMaxMin.maxVal;
		}
		else if( curPos.Y < m_LeverMaxMin.minVal )
		{
			curPos.Y = m_LeverMaxMin.minVal;
		}

		m_UIMesh->SetMeshTranslation(GFX_THROTTLE_LEVER, curPos);
		m_Elements.ChangeElementPosition(HUI_CONTROLS_THROTTLE_LEVER, curPos);	

		// convert to a value for the target speed
		float offset = (curPos.Y - m_LeverMaxMin.minVal) / m_LeverRange;

		math::Vec2 planeSpeedRange = m_Player->GetSpeedRange();
		float range = planeSpeedRange.maxVal - planeSpeedRange.minVal;

		float finalSpeed = planeSpeedRange.minVal + (range*offset);
		m_Player->SetTargetSpeed( finalSpeed );
	}

	if( !m_PlacedAnalogue )
	{
		if( m_Elements.CheckElementForTouch( HUI_CONTROLS_ANALOGUE_AREA, UIFileLoader::EVENT_PRESS ) || 
			m_Elements.CheckElementForTouch( HUI_CONTROLS_ANALOGUE_AREA, UIFileLoader::EVENT_HELD ) )
		{

			math::Vec3 curPos = m_Elements.GetLastTouchPosition( HUI_CONTROLS_ANALOGUE_AREA );
			m_AnalogueTouchIndex = m_Elements.GetLastTouchIndex( HUI_CONTROLS_ANALOGUE_AREA );

			if( curPos.X < m_AnalogueRadius )
				curPos.X = m_AnalogueRadius;
			if( curPos.Y < m_AnalogueRadius )
				curPos.Y = m_AnalogueRadius;

			m_UIMesh->SetMeshTranslation(GFX_ANALOGUE_RANGE, curPos);
			m_UIMesh->SetMeshTranslation(GFX_ANALOGUE_JOYSTICK, curPos);

			m_Elements.ChangeElementPosition(HUI_GFX_ANALOGUE_RANGE, curPos);	
			m_Elements.ChangeElementPosition(HUI_CONTROLS_ANALOGUE_JOYSTICK, curPos);

			m_UIMesh->SetMeshDrawState( GFX_ANALOGUE_RANGE, true );
			m_UIMesh->SetMeshDrawState( GFX_ANALOGUE_JOYSTICK, true );

			m_AnalogueCenter = curPos;

			m_PlacedAnalogue = true;
		}
	}
	else
	{
		if( m_AnalogueTouchIndex != -1 )
		{
			if( input::gInputState.TouchesData[m_AnalogueTouchIndex].bRelease )
			{
				if( m_PlacedAnalogue )
				{
					m_UIMesh->SetMeshDrawState( GFX_ANALOGUE_RANGE, false );
					m_UIMesh->SetMeshDrawState( GFX_ANALOGUE_JOYSTICK, false );
					m_AnalogueTouchIndex = -1;
					m_PlacedAnalogue = false;

					m_AnalogueVales.setZero();
				}
			}
			else
			{
				if( m_PlacedAnalogue )
				{
					math::Vec3 newPos;
					m_AnalogueVales.setZero();
					newPos.X = static_cast<float>( input::gInputState.TouchesData[m_AnalogueTouchIndex].nTouchX );
					newPos.Y = static_cast<float>( input::gInputState.TouchesData[m_AnalogueTouchIndex].nTouchY );
					newPos.Z = m_AnalogueCenter.Z;

					math::Vec3 diff = newPos - m_AnalogueCenter;
					float distance = diff.length();
					if (distance > m_AnalogueRadius) 
					{
						newPos = m_AnalogueCenter + (diff / distance) * m_AnalogueRadius;
					}

					m_UIMesh->SetMeshTranslation(GFX_ANALOGUE_JOYSTICK, newPos);
					m_Elements.ChangeElementPosition(HUI_CONTROLS_ANALOGUE_JOYSTICK, newPos);

					diff = newPos - m_AnalogueCenter;
					distance = diff.length();
					
					if( distance != 0.0f )
					{
						if( std::abs(diff.X) < m_GameData.ANALOGUE_DEAD_ZONE)
							m_AnalogueVales.X = 0.0f;
						else
							m_AnalogueVales.X = diff.X / m_AnalogueRadius;
						
						if( std::abs(diff.Y) < m_GameData.ANALOGUE_DEAD_ZONE)
							m_AnalogueVales.Y = 0.0f;
						else
							m_AnalogueVales.Y = diff.Y / m_AnalogueRadius;
					}
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
void ControlsTabletSDUI::Draw()
{
	//m_Player = 0;//PhysicsWorld::GetPlayer();
	//if( m_Player == 0 )
	//	return;

	renderer::OpenGL::GetInstance()->SetNearFarClip( NEAR_CLIP_ORTHO, FAR_CLIP_ORTHO );
	renderer::OpenGL::GetInstance()->SetupOrthographicView( core::app::GetOrientationWidth(), core::app::GetOrientationHeight(), true );
	renderer::OpenGL::GetInstance()->DisableLighting();

	renderer::OpenGL::GetInstance()->SetColour4ub( 255, 255, 255, 255 );

	renderer::OpenGL::GetInstance()->DepthMode( false, GL_ALWAYS );
	renderer::OpenGL::GetInstance()->BlendMode( true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	if( m_UIMesh != 0 )
		m_UIMesh->Draw();

	m_Elements.Draw();

	renderer::OpenGL::GetInstance()->DepthMode( true, GL_LESS );
	renderer::OpenGL::GetInstance()->BlendMode( false, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

}

/////////////////////////////////////////////////////
/// Method: AnalogueValues
/// Params: None
///
/////////////////////////////////////////////////////
const math::Vec2& ControlsTabletSDUI::AnalogueValues()
{
	return m_AnalogueVales;
}

/////////////////////////////////////////////////////
/// Method: SetupControls
/// Params: [in]tilt
///
/////////////////////////////////////////////////////
void ControlsTabletSDUI::SetupControls( bool tilt )
{
	m_TiltControls = tilt;

	/*if( tilt )
	{
		m_UIMesh->SetMeshDrawState( BTN_LEFT, false );
		m_UIMesh->SetMeshDrawState( BTN_RIGHT, false );
	}
	else
	{
		m_UIMesh->SetMeshDrawState( BTN_LEFT, true );
		m_UIMesh->SetMeshDrawState( BTN_RIGHT, true );
	}*/
}

/////////////////////////////////////////////////////
/// Method: SetSpeedLever
/// Params: [in]val
///
/////////////////////////////////////////////////////
void ControlsTabletSDUI::SetSpeedLever( float val )
{
	math::Vec3 leverPos;

	// convert percent to lever
	leverPos.Y = m_LeverMaxMin.minVal + (m_LeverRange*val);

	leverPos.X = m_LeverStartPos.X;
	if( leverPos.Y > m_LeverMaxMin.maxVal )
	{
		leverPos.Y = m_LeverMaxMin.maxVal;
	}
	else if( leverPos.Y < m_LeverMaxMin.minVal )
	{
		leverPos.Y = m_LeverMaxMin.minVal;
	}

	m_UIMesh->SetMeshTranslation(GFX_THROTTLE_LEVER, leverPos);
	m_Elements.ChangeElementPosition(HUI_CONTROLS_THROTTLE_LEVER, leverPos);	
}
