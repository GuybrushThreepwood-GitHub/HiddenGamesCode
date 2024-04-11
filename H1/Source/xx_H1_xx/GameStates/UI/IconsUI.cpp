
/*===================================================================
	File: IconsUI.cpp
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
	m_UITabletSD = 0;
	m_UITabletSD = new IconsTabletSDUI( stateManager, inputSystem );
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
}

