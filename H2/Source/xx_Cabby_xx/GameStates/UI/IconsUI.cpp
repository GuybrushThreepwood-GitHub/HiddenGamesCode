
/*===================================================================
	File: IconsUI.cpp
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
#include "GameStates/UI/IconsUI.h"
#include "GameStates/UI/UIFileLoader.h"

namespace
{
	const float AREA_RADIUS = 4.0f;
}

/////////////////////////////////////////////////////
/// Default Constructor
/// 
///
/////////////////////////////////////////////////////
IconsUI::IconsUI( StateManager& stateManager, InputSystem& inputSystem )
: IBaseGameState( stateManager, inputSystem, UI_ICONS )
{
	m_UIPhoneSD = 0;
	m_UIPhoneRetina = 0;
	m_UITabletSD = 0;

	if( core::app::IsTablet() )
	{
		if( core::app::IsRetinaDisplay() )
		{
			m_UITabletSD = new IconsTabletSDUI( stateManager, inputSystem );
		}
		else
		{
			m_UITabletSD = new IconsTabletSDUI( stateManager, inputSystem );
		}
	}
	else
	{
		if( core::app::IsRetinaDisplay() )
		{
			m_UIPhoneRetina = new IconsPhoneRetinaUI( stateManager, inputSystem );
		}
		else
		{
			m_UIPhoneSD = new IconsPhoneSDUI( stateManager, inputSystem );
		}
	}
}

/////////////////////////////////////////////////////
/// Default Destructor
/// 
///
/////////////////////////////////////////////////////
IconsUI::~IconsUI()
{
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
void IconsUI::Enter()
{
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
void IconsUI::Exit()
{
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
void IconsUI::PrepareTransitionIn()
{
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
void IconsUI::PrepareTransitionOut()
{
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
int IconsUI::TransitionIn()
{
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
int IconsUI::TransitionOut()
{
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
void IconsUI::Update( float deltaTime )
{
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
void IconsUI::Draw()
{
	if( m_UITabletSD != 0 )
		m_UITabletSD->Draw();

	if( m_UIPhoneSD != 0 )
		m_UIPhoneSD->Draw();	

	if( m_UIPhoneRetina != 0 )
		m_UIPhoneRetina->Draw();
}

