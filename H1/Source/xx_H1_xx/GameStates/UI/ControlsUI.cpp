
/*===================================================================
	File: ControlsUI.cpp
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
#include "GameStates/UI/ControlsUI.h"
#include "GameStates/UI/UIFileLoader.h"

namespace
{
	math::Vec2 zeroVec;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ControlsUI::ControlsUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_CONTROLS )
{
	m_UITabletSD = 0;
	m_UITabletSD = new ControlsTabletSDUI( stateManager, inputSystem );
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ControlsUI::~ControlsUI()
{
	if( m_UITabletSD != 0 )
	{
		delete m_UITabletSD;
		m_UITabletSD = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::Enter()
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->Enter();
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::Exit()
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->Exit();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::PrepareTransitionIn()
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->PrepareTransitionIn();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::PrepareTransitionOut()
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->PrepareTransitionOut();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int ControlsUI::TransitionIn()
{
	if( m_UITabletSD != 0 )
		return m_UITabletSD->TransitionIn();
	return 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int ControlsUI::TransitionOut()
{
	if( m_UITabletSD != 0 )
		return m_UITabletSD->TransitionOut();

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void ControlsUI::Update( float deltaTime )
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->Update(deltaTime);
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::Draw()
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->Draw();	
}

/////////////////////////////////////////////////////
/// Method: SetupControls
/// Params: [in]tilt
///
/////////////////////////////////////////////////////
void ControlsUI::SetupControls( bool tilt )
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->SetupControls(tilt);	
}

/////////////////////////////////////////////////////
/// Method: SetSpeedLever
/// Params: [in]val
///
/////////////////////////////////////////////////////
void ControlsUI::SetSpeedLever( float val )
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->SetSpeedLever(val);	
}
	
/////////////////////////////////////////////////////
/// Method: DivePress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::DivePress()			
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletSD != 0 )
			return m_UITabletSD->DivePress();
	}

	return false; 
}
	
/////////////////////////////////////////////////////
/// Method: LiftPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::LiftPress()		
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletSD != 0 )
			return m_UITabletSD->LiftPress();
	}

	return false;
}
	
/////////////////////////////////////////////////////
/// Method: LeftPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::LeftPress()		
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletSD != 0 )
			return m_UITabletSD->LeftPress();
	}

	return false;
}
		
/////////////////////////////////////////////////////
/// Method: RightPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::RightPress()		
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletSD != 0 )
			return m_UITabletSD->RightPress();
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: ShootPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::ShootPress() 
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletSD != 0 )
			return m_UITabletSD->ShootPress();
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: AnalogueValues
/// Params: None
///
/////////////////////////////////////////////////////
const math::Vec2& ControlsUI::AnalogueValues()
{
	zeroVec.setZero();

	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletSD != 0 )
			return m_UITabletSD->AnalogueValues();
	}

	return zeroVec;
}
