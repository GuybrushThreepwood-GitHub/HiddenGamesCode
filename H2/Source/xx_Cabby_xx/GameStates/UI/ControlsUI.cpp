
/*===================================================================
	File: ControlsUI.cpp
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
#include "GameStates/UI/ControlsUI.h"
#include "GameStates/UI/UIFileLoader.h"

namespace
{
	
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
ControlsUI::ControlsUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_CONTROLS )
{
	m_UIPhoneSD = 0;
	m_UIPhoneRetina = 0;
	m_UITabletSD = 0;
	m_UITabletRetina = 0;

	if( core::app::IsTablet() )
	{
		if( core::app::IsRetinaDisplay() )
		{
			m_UITabletRetina = new ControlsTabletRetinaUI( stateManager, inputSystem );
		}
		else
		{
			m_UITabletSD = new ControlsTabletSDUI( stateManager, inputSystem );
		}
	}
	else
	{
		if( core::app::IsRetinaDisplay() )
		{
			m_UIPhoneRetina = new ControlsPhoneRetinaUI( stateManager, inputSystem );
		}
		else
		{
			m_UIPhoneSD = new ControlsPhoneSDUI( stateManager, inputSystem );
		}
	}
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
ControlsUI::~ControlsUI()
{
	if( m_UITabletRetina != 0 )
	{
		delete m_UITabletRetina;
		m_UITabletRetina = 0;
	}

	if( m_UITabletSD != 0 )
	{
		delete m_UITabletSD;
		m_UITabletSD = 0;
	}

	if( m_UIPhoneSD != 0 )
	{
		delete m_UIPhoneSD;
		m_UIPhoneSD = 0;
	}

	if( m_UIPhoneRetina != 0 )
	{
		delete m_UIPhoneRetina;
		m_UIPhoneRetina = 0;
	}
}

/////////////////////////////////////////////////////
/// Method: Enter
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::Enter()
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->Enter();

	if( m_UITabletSD != 0 )
		m_UITabletSD->Enter();

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->Enter();

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->Enter();
}

/////////////////////////////////////////////////////
/// Method: Exit
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::Exit()
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->Exit();
	
	if( m_UITabletSD != 0 )
		m_UITabletSD->Exit();

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->Exit();

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->Exit();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::PrepareTransitionIn()
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->PrepareTransitionIn();

	if( m_UITabletSD != 0 )
		m_UITabletSD->PrepareTransitionIn();

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->PrepareTransitionIn();

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->PrepareTransitionIn();
}

/////////////////////////////////////////////////////
/// Method: PrepareTransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::PrepareTransitionOut()
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->PrepareTransitionOut();

	if( m_UITabletSD != 0 )
		m_UITabletSD->PrepareTransitionOut();

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->PrepareTransitionOut();

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->PrepareTransitionOut();
}

/////////////////////////////////////////////////////
/// Method: TransitionIn
/// Params: None
///
/////////////////////////////////////////////////////
int ControlsUI::TransitionIn()
{
	if( m_UITabletRetina != 0 )
		return m_UITabletRetina->TransitionIn();

	if( m_UITabletSD != 0 )
		return m_UITabletSD->TransitionIn();

	if( m_UIPhoneSD != 0 )
		return m_UIPhoneSD->TransitionIn();

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->TransitionIn();

	return 0;
}

/////////////////////////////////////////////////////
/// Method: TransitionOut
/// Params: None
///
/////////////////////////////////////////////////////
int ControlsUI::TransitionOut()
{
	if( m_UITabletRetina != 0 )
		return m_UITabletRetina->TransitionOut();

	if( m_UITabletSD != 0 )
		return m_UITabletSD->TransitionOut();

	if( m_UIPhoneSD != 0 )
		return m_UIPhoneSD->TransitionOut();

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->TransitionOut();

	return 0;
}

/////////////////////////////////////////////////////
/// Method: Update
/// Params: [in]deltaTime
///
/////////////////////////////////////////////////////
void ControlsUI::Update( float deltaTime )
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->Update(deltaTime);

	if( m_UITabletSD != 0 )
		m_UITabletSD->Update(deltaTime);

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->Update(deltaTime);	

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->Update(deltaTime);	
}

/////////////////////////////////////////////////////
/// Method: Draw
/// Params: None
///
/////////////////////////////////////////////////////
void ControlsUI::Draw()
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->Draw();

	if( m_UITabletSD != 0 )
		m_UITabletSD->Draw();

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->Draw();	

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->Draw();	
}

/////////////////////////////////////////////////////
/// Method: SetupControls
/// Params: [in]tilt
///
/////////////////////////////////////////////////////
void ControlsUI::SetupControls( int controlType )
{
	if( m_UITabletRetina != 0 )
		m_UITabletRetina->SetupControls(controlType);

	if( m_UITabletSD != 0 )
		m_UITabletSD->SetupControls(controlType);

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->SetupControls(controlType);

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->SetupControls(controlType);
}

/////////////////////////////////////////////////////
/// Method: PausePress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::PausePress()		
{ 
	if( m_UITabletRetina != 0 )
		return m_UITabletRetina->PausePress();

	if( m_UITabletSD != 0 )
		return m_UITabletSD->PausePress();

	if( m_UIPhoneSD != 0 )
		return m_UIPhoneSD->PausePress();

	if( m_UIPhoneRetina != 0 )
		return m_UIPhoneRetina->PausePress();

	return false;
}
	
/////////////////////////////////////////////////////
/// Method: UpPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::UpPress()			
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletRetina != 0 )
			return m_UITabletRetina->UpPress();

		if( m_UITabletSD != 0 )
			return m_UITabletSD->UpPress();

		if( m_UIPhoneSD != 0 )
			return m_UIPhoneSD->UpPress();

		if( m_UIPhoneRetina != 0 )
			return m_UIPhoneRetina->UpPress();
	}

	return false; 
}
	
/////////////////////////////////////////////////////
/// Method: DownPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::DownPress()		
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletRetina != 0 )
			return m_UITabletRetina->DownPress();

		if( m_UITabletSD != 0 )
			return m_UITabletSD->DownPress();

		if( m_UIPhoneSD != 0 )
			return m_UIPhoneSD->DownPress();

		if( m_UIPhoneRetina != 0 )
			return m_UIPhoneRetina->DownPress();
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
		if( m_UITabletRetina != 0 )
			return m_UITabletRetina->LeftPress();

		if( m_UITabletSD != 0 )
			return m_UITabletSD->LeftPress();

		if( m_UIPhoneSD != 0 )
			return m_UIPhoneSD->LeftPress();

		if( m_UIPhoneRetina != 0 )
			return m_UIPhoneRetina->LeftPress();
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
		if( m_UITabletRetina != 0 )
			return m_UITabletRetina->RightPress();

		if( m_UITabletSD != 0 )
			return m_UITabletSD->RightPress();

		if( m_UIPhoneSD != 0 )
			return m_UIPhoneSD->RightPress();

		if( m_UIPhoneRetina != 0 )
			return m_UIPhoneRetina->RightPress();
	}

	return false;
}

/////////////////////////////////////////////////////
/// Method: LandingGearPress
/// Params: None
///
/////////////////////////////////////////////////////
bool ControlsUI::LandingGearPress() 
{ 
	if( !core::app::IsPCOnly() )
	{
		if( m_UITabletRetina != 0 )
			return m_UITabletRetina->LandingGearPress();

		if( m_UITabletSD != 0 )
			return m_UITabletSD->LandingGearPress();

		if( m_UIPhoneSD != 0 )
			return m_UIPhoneSD->LandingGearPress();

		if( m_UIPhoneRetina != 0 )
			return m_UIPhoneRetina->LandingGearPress();
	}

	return false;
}
